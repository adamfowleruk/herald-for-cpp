/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include "../../herald/herald.h"

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>

#include <zephyr/settings/settings.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/services/bas.h>

#include <zephyr/kernel_structs.h>
// #include <sys/thread_stack.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/hwinfo.h>

#include <inttypes.h>

#include <zephyr/logging/log.h>

#include <zephyr/storage/flash_map.h>

LOG_MODULE_REGISTER(app, CONFIG_APP_LOG_LEVEL);
#define APP_DBG(_msg,...) LOG_DBG(_msg,##__VA_ARGS__);
#define APP_INF(_msg,...) LOG_INF(_msg,##__VA_ARGS__);
#define APP_ERR(_msg,...) LOG_ERR(_msg,##__VA_ARGS__);

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* The maximum name length allowed in Flash storage */
#define MAX_NAME_LENGTH 40

// The below fails with nRF SDK 2.6.1 as its defined in an nrf log include - so we leave commented out now
// void k_sys_fatal_error_handler(unsigned int reason, const z_arch_esf_t *esf) {
// 	// LOG_PANIC();
// 	while (1) {
// 		// do nothing
// 	}
// }

struct k_thread herald_thread;
constexpr int stackMaxSize = 
#ifdef CONFIG_BT_MAX_CONN
	2048 + (CONFIG_BT_MAX_CONN * 512)
	// Was 12288 + (CONFIG_BT_MAX_CONN * 512), but this starved newlibc of HEAP (used in handling BLE connections/devices)
#else
	9192
#endif
// Since v2.1 - MEMORY ARENA extra stack reservation - See herald/datatype/data.h
#ifdef HERALD_MEMORYARENA_MAX
  + HERALD_MEMORYARENA_MAX
#else
  + 8192
#endif
;
K_THREAD_STACK_DEFINE(herald_stack, 
	stackMaxSize
); // Was 9192 for nRF5340 (10 conns), 2048 for nRF52832 (3 conns)


struct DummyDelegate {};

using MYUINT32 = unsigned long;

struct basic_venue {
	std::uint16_t country;
	std::uint16_t state;
	MYUINT32 code; // C++ linker may balk, confusing unsigned int with unsigned long
	std::string name;
};

// static struct basic_venue joesPizza = {
// 	.country = 826,
// 	.state = 4,
// 	.code = 12345,
// 	.name = "Joe's Pizza"
// };

// static struct basic_venue adamsFishShop = {
// 	.country = 826,
// 	.state = 3,
// 	.code = 22334,
// 	.name = "Adam's Fish Shop"
// };

// static struct basic_venue maxsFineDining = {
// 	.country = 832,
// 	.state = 1,
// 	.code = 55566,
// 	.name = "Max's Fine Dining"
// };

// static struct basic_venue erinsStakehouse = {
// 	.country = 826,
// 	.state = 4,
// 	.code = 123123,
// 	.name = "Erin's Stakehouse"
// };

// TODO replace the below with sub-venue extended data, with same venue code
// static struct basic_venue adamsKitchen = {
//   .country = 826,
// 	.state = 4,
// 	.code = 1234,
// 	.name = "Adam's Kitchen"
// };
// static struct basic_venue adamsOffice = {
//   .country = 826,
// 	.state = 4,
// 	.code = 2345,
// 	.name = "Adam's Office"
// };
// static struct basic_venue adamsBedroom = {
//   .country = 826,
// 	.state = 4,
// 	.code = 3456,
// 	.name = "Adam's Bedroom"
// };
// static struct basic_venue adamsLanding = {
//   .country = 826,
// 	.state = 4,
// 	.code = 5678,
// 	.name = "Adam's Landing"
// };
// static struct basic_venue adamsPond = {
//   .country = 826,
// 	.state = 4,
// 	.code = 6789,
// 	.name = "Adam's Pond"
// };
// static struct basic_venue adamsLounge = {
//   .country = 826,
// 	.state = 4,
// 	.code = 7890,
// 	.name = "Adam's Lounge"
// };

// static struct basic_venue estimoteGarage = {
//   .country = 826,
// 	.state = 4,
// 	.code = 666,
// 	.name = "Estimote Herald"
// };

// static struct basic_venue beaconBasic1 = {
//   .country = 1,
// 	.state = 1,
// 	.code = 1,
// 	.name = "Herald-01"
// };
// static struct basic_venue beaconBasic2 = {
//   .country = 2,
// 	.state = 2,
// 	.code = 2,
// 	.name = "Herald-02"
// };
// static struct basic_venue beaconBasic3 = {
//   .country = 3,
// 	.state = 3,
// 	.code = 3,
// 	.name = "Herald-03"
// };
// static struct basic_venue beaconBasic4 = {
//   .country = 4,
// 	.state = 4,
// 	.code = 4,
// 	.name = "Herald-04"
// };

// A Herald Mesh Beacon will advertise just like a Herald Venue Beacon, but with a name becoming the Mesh Node ID

static struct basic_venue noVenue = {
    .country = 0,
	.state = 0,
	.code = 0,
	.name = "UnconfiguredHeraldMeshRelay"
};

void herald_entry() {
	APP_DBG("Herald entry");
	k_sleep(K_MSEC(10000)); // pause so we have time to see Herald initialisation log messages. Don't do this in production!
	APP_DBG("Herald setup begins");

	using namespace herald;
	using namespace herald::payload;
	using namespace herald::payload::beacon;
	using namespace herald::payload::extended;

	// Create Herald sensor array
	ZephyrContextProvider zcp;
	Context ctx(zcp,zcp.getLoggingSink(),zcp.getBluetoothStateManager());
	// using CT = Context<ZephyrContextProvider,ZephyrLoggingSink,BluetoothStateManager>;
	
	// Disable receiver / scanning mode - we're just transmitting our value
	BLESensorConfiguration config = ctx.getSensorConfiguration(); // copy ctor
    // config.heraldProtocolV2Enabled = true; // Enable Herald V2 mesh underlying protocol
    // config.heraldMeshEnabled true; // Enable the mesh
    // The above two are set via Kconfig now. See bottom of ../prj.conf for details
	config.scanningEnabled = true; // Required to find other mesh devices
	// config.advertisingEnabled = true; // default
	ctx.setSensorConfiguration(config);

	// Sun 02 July 2023 - ADDED DYNAMIC BEACON CONFIGURATION FROM FLASH STORAGE
	// Attempt to read Beacon configuration from raw flash
	// If it works, use that for our beacon information
	// If it fails, default to an empty configuration
	std::uint16_t country = noVenue.country;
	std::uint16_t state = noVenue.state;
	std::uint32_t code = noVenue.code;

    // TODO lon lat for Mesh Relay BUT only over encrypted channel, not in advertisement (TBD)

	// X, Y, Z positions in CM from a common origin
	std::uint16_t x = 0;
	std::uint16_t y = 0;
	std::uint16_t z = 0;
	std::string name = std::string(noVenue.name); // force copy

	const struct flash_area *storageArea;
	int err = flash_area_open(FIXED_PARTITION_ID(storage_partition), &storageArea);

	if (err < 0) {
		APP_DBG("Error opening storage flash area");
	} else {
		APP_DBG("Attempting to read flash storage...");
		// 8 bytes for numeric data, 32 max for venue name
		uint8_t buffer[MAX_NAME_LENGTH];
		int readRet = flash_area_read(storageArea, 0, buffer, MAX_NAME_LENGTH);
		if (0 == readRet) {
			// Read OK
			// Check values are not all FF or 00
			size_t nonEmptyCount = 0;
			size_t terminationCharPosition = 0;
			for (size_t pos = 0;pos < MAX_NAME_LENGTH;++pos) {
				if (buffer[pos] != 0 && buffer[pos] != 0xff) {
					++nonEmptyCount;
				}
				if (pos >= 8 && 0 == terminationCharPosition && buffer[pos] == (uint8_t)'\0') {
					// found the first char termination character
					terminationCharPosition = pos;
				}
			}
			if (0 == nonEmptyCount) {
				APP_DBG("Read flash configuration area is empty. Using default configuration.");
			} else {
				APP_DBG("Found configuration");
				country = (uint16_t)(*buffer);
				state = (uint16_t)(*(buffer + 2));
				code = (std::uint32_t)(*(buffer + 4));

				// Read the X, Y, Z position of the beacon (in cm, max 655.35m from origin)
				x = (uint16_t)(*(buffer + 8));
				y = (uint16_t)(*(buffer + 10));
				z = (uint16_t)(*(buffer + 12));

				if (0 == terminationCharPosition) {
					terminationCharPosition = MAX_NAME_LENGTH;
				}
				size_t strLength = terminationCharPosition - 14;
				char cbuffer[MAX_NAME_LENGTH - 14 + 1];
				cbuffer[strLength] = '\0';
				// Read the title string
				for (size_t strPos = 14;strPos < terminationCharPosition;++strPos) {
					cbuffer[strPos - 14] = (char)(*(buffer + strPos));
				}
				name = std::string(cbuffer);
				APP_DBG("Configuration successfully read from flash storage");
			}
		} else {
			APP_DBG("Error reading flash storage area");
		}
		flash_area_close(storageArea);
	}

	ConcreteExtendedDataV1 extendedData;
	extendedData.addSection(ExtendedDataSegmentCodesV1::TextPremises, name);
	Data xyz;
	xyz.append(x);
	xyz.append(y);
	xyz.append(z);
	extendedData.addSection(ExtendedDataSegmentCodesV1::LocalGridPosition, xyz);

	payload::beacon::ConcreteBeaconPayloadDataSupplierV1 pds(
		country,
		state,
		code,
		extendedData
	);
	
	herald::ble::nordic_uart::NordicUartSensorDelegate nus(ctx);

	// this is unusual, but required. Really we should log activity to serial BLE or similar
	DummyDelegate appDelegate;
	SensorDelegateSet sensorDelegates(appDelegate, nus);
	
	ConcreteBLESensor ble(ctx, ctx.getBluetoothStateManager(), pds, sensorDelegates);
	SensorArray sa(ctx,pds,ble);

	// Start array (and thus start advertising)
	sa.start();

	std::uint64_t iter = 0;
	// APP_DBG("got iter!");
	// k_sleep(K_SECONDS(2));
	Date last;
	// APP_DBG("got last!");
	// k_sleep(K_SECONDS(2));
	int delay = 250; // KEEP THIS SMALL!!! This is how often we check to see if anything needs to happen over a connection.
	
	APP_DBG("Entering herald iteration loop");
	k_sleep(K_SECONDS(2));
	while (1) {
		k_sleep(K_MSEC(delay)); 
		Date now;
		if (iter > 40 /* && iter < 44 */ ) { // some delay to allow us to see advertising output
			// You could only do first 3 iterations so we can see the older log messages without continually scrolling through log messages
			APP_DBG("Calling Sensor Array iteration");
			// k_sleep(K_SECONDS(2));
			sa.iteration(now - last);
		}
		
		if (0 == iter % (5000 / delay)) {
			APP_DBG("herald thread still running. Iteration: %lld", iter);
			// runner.run(Date()); // Note: You may want to do this less or more regularly depending on your requirements
			APP_ERR("Memory pages free in Data Arena: %d", herald::datatype::Data::getArena().pagesFree());

			// Send routine message - just our node name and iteration tick for now
			// TODO make this a better status message
			Data toSend;
			toSend.append(extendedData.payload());
			toSend.append(iter);
			ble.queueMessageForSending(toSend,RSSI(-99));
		}

		last = now;
		++iter;
	}
}


int main(void)
{
	// The below LED device lines have changed since at least Zephyr 3.4.0
	if (!gpio_is_ready_dt(&led)) {
		return 1;
	}

	int ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 1;
	}

	APP_DBG("Logging test");

	// Start herald entry on a new thread in case of errors, or needing to do something on the main thread
	[[maybe_unused]]
	k_tid_t herald_pid = k_thread_create(&herald_thread, herald_stack, stackMaxSize,
			(k_thread_entry_t)herald_entry, NULL, NULL, NULL,
			-1, K_USER,
			K_NO_WAIT);

	// herald_entry();
	// NOTE Above only works if CONFIG_MAIN_STACK_SIZE=2048 is set in prj.conf

	/* Implement notification. At the moment there is no suitable way
	 * of starting delayed work so we do it here
	 */
	while (1) {
		k_sleep(K_SECONDS(2));
		
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			return 1;
		}

		APP_DBG("main thread still running");

		// TODO Add logic here to detect failure in Herald thread, and restart to resume as necessary
	}

	return 0;
};
