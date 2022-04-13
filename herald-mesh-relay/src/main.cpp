/*
 * Copyright 2020-2021 Herald Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh/models.h>
#include <bluetooth/mesh/dk_prov.h>
#include <dk_buttons_and_leds.h>
#include <drivers/gpio.h>
#include "model_handler.h"
#include "lb_service_handler.h"

#include "herald/mesh/mesh.h"
#include "herald/mesh/location_services_srv.h"

#include <logging/log.h>
// namespace applogging {
  LOG_MODULE_REGISTER(app, CONFIG_APP_LOG_LEVEL);
	#define APP_DBG(_msg,...) LOG_DBG(_msg,##__VA_ARGS__);
	#define APP_INF(_msg,...) LOG_INF(_msg,##__VA_ARGS__);
	#define APP_ERR(_msg,...) LOG_ERR(_msg,##__VA_ARGS__);
// }

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
#define LED0	DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN	DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS	DT_GPIO_FLAGS(LED0_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led0 devicetree alias is not defined"
#define LED0	""
#define PIN	0
#define FLAGS	0
#endif


static void bt_ready(int err)
{
	if (err) {
		APP_DBG("Bluetooth init failed (err %d)", err);
		return;
	}

	APP_DBG("Bluetooth initialised");

	dk_leds_init();
	dk_buttons_init(NULL);

	err = bt_mesh_init(bt_mesh_dk_prov_init(), model_handler_init());
	if (err) {
		APP_DBG("Initializing mesh failed (err %d)", err);
		return;
	}

	if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}

	// To reset provisioning status
	//bt_mesh_prov_reset(); // prov.h - NOT accessible from an app
	// bt_mesh_reset();

	// Set that we want both MESH and GATT for MESH
	bt_mesh_prov_enable(bt_mesh_prov_bearer_t(unsigned(BT_MESH_PROV_ADV) | unsigned(BT_MESH_PROV_GATT)));

	APP_DBG("Mesh started");

	lbs_handler_init();
}

void main(void)
{
	k_sleep(K_SECONDS(6));

	const struct device *dev;
	bool led_is_on = true;
	int ret;

	dev = device_get_binding(LED0);
	if (dev == NULL) {
		return;
	}

	ret = gpio_pin_configure(dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
	if (ret < 0) {
		return;
	}

	int err;

	APP_DBG("Initialising...");

	err = bt_enable(bt_ready);
	if (err) {
		APP_DBG("Bluetooth init failed (err %d)", err);
	}

	uint8_t dummyMac[6] = {0,1,2,3,4,5};

	// Regular debug output to show the app is still running
	int iter = 0;
	while (1) {
		k_sleep(K_SECONDS(2));
		gpio_pin_set(dev, PIN, (int)led_is_on);
		led_is_on = !led_is_on;

		APP_DBG("Herald Relay main thread still running");

		// TODO Add logic here to detect failure in Herald thread, and restart to resume as necessary

		// Fake presence publishing for now
		++iter;
		if (iter > 10 && 0 == iter % 5) {
			APP_DBG("Publishing presence message...");
			int res = bt_mesh_herald_presence_share(dummyMac, -40, 
				bt_mesh_herald_location_services_cli_presence::BT_MESH_HERALD_PRESENCE_OBSERVED);
			APP_DBG("Presence publishing result: %d", res);
		}
	}
}
