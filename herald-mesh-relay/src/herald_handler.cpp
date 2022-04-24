/*
 * Copyright 2020-2022 Herald Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#include "herald_handler.h"

// #include "../../herald/herald.h" // This is convenient, but leads to large
// binaries!
#include "herald/ble/ble_sensor_configuration.h"
#include "herald/ble/zephyr/concrete_ble_receiver.h"
// #include "herald/ble/zephyr/concrete_ble_transmitter.h"
// #include "herald/ble/zephyr/nordic_uart/nordic_uart_sensor_delegate.h"
#include "herald/datatype/date.h"
#include "herald/datatype/immediate_send_data.h"
#include "herald/datatype/location.h"
#include "herald/datatype/payload_data.h"
#include "herald/datatype/proximity.h"
#include "herald/datatype/sensor_state.h"
#include "herald/datatype/sensor_type.h"
#include "herald/datatype/target_identifier.h"
#include "herald/mesh/mesh.h"
#include "herald/mesh/presence.h"
#include "herald/mesh/presence_server.h"
// #include "herald/payload/beacon/beacon_payload_data_supplier.h"
// #include "herald/payload/extended/extended_data.h"
#include "herald/payload/fixed/fixed_payload_data_supplier.h"
#include "herald/sensor.h"
#include "herald/sensor_array.h"
#include "herald/sensor_delegate.h"
#include "herald/zephyr_context.h"

#include <kernel.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(app_herald, CONFIG_APP_LOG_LEVEL);
#define APP_DBG(_msg, ...) LOG_DBG(_msg, ##__VA_ARGS__);
#define APP_INF(_msg, ...) LOG_INF(_msg, ##__VA_ARGS__);
#define APP_ERR(_msg, ...) LOG_ERR(_msg, ##__VA_ARGS__);

struct k_thread herald_thread;
constexpr int stackMaxSize =
#ifdef CONFIG_BT_MAX_CONN
    2048 + (CONFIG_BT_MAX_CONN * 512)
// Was 12288 + (CONFIG_BT_MAX_CONN * 512), but this starved newlibc of HEAP
// (used in handling BLE connections/devices)
#else
    9192
#endif
// Since v2.1 - MEMORY ARENA extra stack reservation - See
// herald/datatype/data.h
#ifdef HERALD_MEMORYARENA_MAX
    + HERALD_MEMORYARENA_MAX
#else
    + 8192
#endif
    ;
K_THREAD_STACK_DEFINE(herald_stack,
                      stackMaxSize);  // Was 9192 for nRF5340 (10 conns), 2048
                                      // for nRF52832 (3 conns)

// When did we last send nearby presence data over mesh (TODO REPLACE THIS)
int lastShared = 0;

using namespace herald::datatype;
using namespace herald::ble;

char* str(const TargetIdentifier& ti) {
  return log_strdup(((std::string)ti).c_str());
}
// struct DummyDelegate{};
class AppLoggingDelegate {
 public:
  AppLoggingDelegate() = default;
  ~AppLoggingDelegate() = default;

  void sensor(SensorType sensor, const TargetIdentifier& didDetect) {
    // LOG_DBG("sensor didDetect");
    APP_DBG("sensor didDetect: %s",
            str(didDetect));  // May want to disable this - logs A LOT of info
  }

  /// Read payload data from target, e.g. encrypted device identifier from BLE
  /// peripheral after successful connection.
  void sensor(SensorType sensor, const PayloadData& didRead,
              const TargetIdentifier& fromTarget) {
    // LOG_DBG("sensor didRead");
    APP_DBG("sensor didRead: %s with payload: %s", str(fromTarget),
            log_strdup(didRead.hexEncodedString().c_str()));
  }

  /// Receive written immediate send data from target, e.g. important timing
  /// signal.
  void sensor(SensorType sensor, const ImmediateSendData& didReceive,
              const TargetIdentifier& fromTarget) {
    // LOG_DBG("sensor didReceive");
    APP_DBG("sensor didReceive: %s with immediate send data: %s",
            str(fromTarget), log_strdup(didReceive.hexEncodedString().c_str()));
  }

  /// Read payload data of other targets recently acquired by a target, e.g.
  /// Android peripheral sharing payload data acquired from nearby iOS
  /// peripherals.
  void sensor(SensorType sensor, const std::vector<PayloadData>& didShare,
              const TargetIdentifier& fromTarget) {
    APP_DBG("sensor didShare");
    // LOG_DBG("sensor didShare: %s", str(fromTarget) );
    // for (auto& p : didShare) {
    //   LOG_DBG(" - %s", log_strdup(p.hexEncodedString().c_str()));
    // }
  }

  /// Measure proximity to target, e.g. a sample of RSSI values from BLE
  /// peripheral.
  void sensor(SensorType sensor, const Proximity& didMeasure,
              const TargetIdentifier& fromTarget) {
    APP_DBG(
        "didMeasure: %s, fromTarget: %s",
        log_strdup(didMeasure.description().c_str()),
        log_strdup(
            ((std::string)BLEMacAddress(fromTarget.underlyingData())).c_str()));
    // LOG_DBG("sensor didMeasure: %s with proximity: %d", str(fromTarget),
    // didMeasure.value);

    // Now send a did detect message over mesh
    // Note: This frequency is managed by BLESensorConfiguration
    // For now, delay so many seconds
    int now = 0.001 * k_uptime_get();
    if (lastShared + 5 < now) {
      lastShared = now;
      uint8_t seenMac[6] = {0, 1, 2, 3, 4, 5};
    //   const auto& d = fromTarget.underlyingData();
    //   bool ok = d.uint8(0, *seenMac);
    //   ok = d.uint8(1, *(seenMac + 1));
    //   ok = d.uint8(2, *(seenMac + 2));
    //   ok = d.uint8(3, *(seenMac + 3));
    //   ok = d.uint8(4, *(seenMac + 4));
    //   ok = d.uint8(5, *(seenMac + 5));
      // TODO check the above return values
      int res = bt_mesh_herald_presence_share(
          seenMac, -40,
          bt_mesh_model_herald_presence_status::
              BT_MESH_HERALD_PRESENCE_OBSERVED);
      APP_DBG("Presence publishing result: %d", res);
    }
  }

  /// Detection of time spent at location, e.g. at specific restaurant between
  /// 02/06/2020 19:00 and 02/06/2020 21:00
  template <typename LocationT>
  void sensor(SensorType sensor, const Location<LocationT>& didVisit) {
    APP_DBG("sensor didVisit");
  }

  /// Measure proximity to target with payload data. Combines didMeasure and
  /// didRead into a single convenient delegate method
  void sensor(SensorType sensor, const Proximity& didMeasure,
              const TargetIdentifier& fromTarget,
              const PayloadData& withPayload) {
    // ERR so it stands out in the logging!
    APP_ERR(
        "didMeasure=%s, fromTarget=%s, withPayload=%s",
        log_strdup(didMeasure.description().c_str()),
        log_strdup(
            ((std::string)BLEMacAddress(fromTarget.underlyingData())).c_str()),
        log_strdup(withPayload.hexEncodedString().c_str()));
  }

  /// Sensor state update
  void sensor(SensorType sensor, const SensorState& didUpdateState) {
    APP_DBG("sensor didUpdateState");
  }
};

using MYUINT32 = unsigned long;

struct basic_venue {
  std::uint16_t country;
  std::uint16_t state;
  MYUINT32 code;  // C++ linker may balk, confusing uint with unsigned long
  std::string name;
};

static struct basic_venue joesPizza = {
    .country = 826, .state = 4, .code = 12345, .name = "Joe's Pizza"};

static struct basic_venue adamsFishShop = {
    .country = 826, .state = 3, .code = 22334, .name = "Adam's Fish Shop"};

static struct basic_venue maxsFineDining = {
    .country = 832, .state = 1, .code = 55566, .name = "Max's Fine Dining"};

static struct basic_venue erinsStakehouse = {
    .country = 826, .state = 4, .code = 123123, .name = "Erin's Stakehouse"};

// TODO replace the below with sub-venue extended data, with same venue code
static struct basic_venue adamsKitchen = {
    .country = 826, .state = 4, .code = 1234, .name = "Adam's Kitchen"};
static struct basic_venue adamsOffice = {
    .country = 826, .state = 4, .code = 2345, .name = "Adam's Office"};
static struct basic_venue adamsBedroom = {
    .country = 826, .state = 4, .code = 3456, .name = "Adam's Bedroom"};
static struct basic_venue adamsLanding = {
    .country = 826, .state = 4, .code = 5678, .name = "Adam's Landing"};
static struct basic_venue adamsPond = {
    .country = 826, .state = 4, .code = 6789, .name = "Adam's Pond"};
static struct basic_venue adamsLounge = {
    .country = 826, .state = 4, .code = 7890, .name = "Adam's Lounge"};

void herald_entry() {
  APP_DBG("Herald entry");
  k_sleep(K_MSEC(2000));  // pause so we have time to see Herald initialisation
                          // log messages. Don't do this in production!
  APP_DBG("Herald setup begins");

  using namespace herald;
  using namespace herald::payload;
//   using namespace herald::payload::beacon;
//   using namespace herald::payload::extended;

  // Create Herald sensor array
  ZephyrContextProvider zcp;
  Context ctx(zcp, zcp.getLoggingSink(), zcp.getBluetoothStateManager());
  // using CT =
  // Context<ZephyrContextProvider,ZephyrLoggingSink,BluetoothStateManager>;

  // Disable receiver / scanning mode - we're just transmitting our value
  BLESensorConfiguration config = ctx.getSensorConfiguration();  // copy ctor
  config.scanningEnabled = true;  // To see other nearby BLE devices
  config.advertisingEnabled = false; // TODO For now, until internal nav created
  ctx.setSensorConfiguration(config);

//   ConcreteExtendedDataV1 extendedData;
//   extendedData.addSection(ExtendedDataSegmentCodesV1::TextPremises,
//                           erinsStakehouse.name);

//   // TODO get this from configuration of the MESH element (Nav beacon model)
//   payload::beacon::ConcreteBeaconPayloadDataSupplierV1 pds(
//       erinsStakehouse.country, erinsStakehouse.state, erinsStakehouse.code,
//       extendedData);

  payload::fixed::ConcreteFixedPayloadDataSupplierV1 pds(0,0,0);
  
  AppLoggingDelegate appDelegate;
  SensorDelegateSet sensorDelegates(appDelegate);

  ConcreteBLESensor ble(ctx, ctx.getBluetoothStateManager(), pds,
                        sensorDelegates);
  SensorArray sa(ctx, pds, ble);

  // Start array (and thus start advertising)
  sa.start();

  int iter = 0;
  // APP_DBG("got iter!");
  // k_sleep(K_SECONDS(2));
  Date last;
  // APP_DBG("got last!");
  // k_sleep(K_SECONDS(2));
  int delay = 2500;  // KEEP THIS SMALL!!! This is how often we check to see if
                    // anything needs to happen over a connection.

  APP_DBG("Entering herald iteration loop");
  k_sleep(K_SECONDS(2));
  while (1) {
    k_sleep(K_MSEC(delay));
    Date now;
    if (iter > 4 /* && iter < 44 */) {  // some delay to allow us to see
                                         // advertising output
      // You could only do first 3 iterations so we can see the older log
      // messages without continually scrolling through log messages
      APP_DBG("Calling Sensor Array iteration");
      // k_sleep(K_SECONDS(2));
      sa.iteration(now - last);
    }

    if (0 == iter % (5000 / delay)) {
      APP_DBG("herald thread still running. Iteration: %d", iter);
      // runner.run(Date()); // Note: You may want to do this less or more
      // regularly depending on your requirements
      APP_ERR("Memory pages free in Data Arena: %d",
              herald::datatype::Data::getArena().pagesFree());
    }

    last = now;
    ++iter;
  }
}

/** MARK: HERALD HANDLER PUBLIC HEADER METHODS **/
[[maybe_unused]]
k_tid_t herald_pid;

void herald_initialise() {
  herald_pid = k_thread_create(
      &herald_thread, herald_stack, stackMaxSize,
      (k_thread_entry_t)herald_entry, NULL, NULL, NULL, -1, K_USER, K_NO_WAIT);
}

void herald_healthcheck() {
  // TODO implement this
}

bool herald_configure() {
  // TODO implement this - from mesh configuration
  return true;
}

bool herald_start() {
  // TODO implement this - put sa.start here
  return true;
}

bool herald_stop() {
  // TODO implement this - put sa.stop here
  return true;
}