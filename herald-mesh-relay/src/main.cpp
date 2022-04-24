/*
 * Copyright 2020-2022 Herald Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#include "herald_handler.h"
#include "model_handler.h"
#include "no_oob_prov.h" // TODO replace me with something secure

#include <bluetooth/bluetooth.h>
// #include <bluetooth/mesh/dk_prov.h> // TODO replace this with a new herald mesh provisioning configuration rather than the dev kit or no_oob_prov one
#include <bluetooth/mesh/models.h>

#include <drivers/gpio.h>

#include <kernel.h>
#include <dk_buttons_and_leds.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(app, CONFIG_APP_LOG_LEVEL);
#define APP_DBG(_msg, ...) LOG_DBG(_msg, ##__VA_ARGS__);
#define APP_INF(_msg, ...) LOG_INF(_msg, ##__VA_ARGS__);
#define APP_ERR(_msg, ...) LOG_ERR(_msg, ##__VA_ARGS__);

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
#define LED0 DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS DT_GPIO_FLAGS(LED0_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led0 devicetree alias is not defined"
#define LED0 ""
#define PIN 0
#define FLAGS 0
#endif

static void prov_complete(uint16_t net_idx, uint16_t src) {
  // Called when provisioning is finished (successfully)

  // Start herald entry on a new thread in case of errors, or needing to do
  // something on the main thread
  // herald_initialise();

  // TODO split the above into general scanning for nearby
  // and advertising a beacon if (and only if) the mesh model
  // configuration has that data

  // Now start any configured functionality
  // herald_start();
}

static void prov_reset() {
  // Stop Herald and other items, to allow the caller to restart provisioning
  herald_stop();
}

static const struct no_oob_prov_cb prov_cbs = {
  .provisioned = prov_complete,
  .reset = prov_reset
};

static void bt_ready(int err) {
  if (err) {
    APP_DBG("Bluetooth init failed (err %d)", err);
    return;
  }

  APP_DBG("Bluetooth initialised");

  dk_leds_init();
  dk_buttons_init(NULL);

  // Note: Following provisioning the prov_complete function is called
  //       After which, if configured, the Herald LE gateway is enabled
  err = bt_mesh_init(bt_mesh_no_oob_prov_init(&prov_cbs), model_handler_init());
  if (err) {
    APP_DBG("Initializing mesh failed (err %d)", err);
    return;
  }

  if (IS_ENABLED(CONFIG_SETTINGS)) {
    settings_load();
  }

  // Set that we want both MESH and GATT for MESH
  bt_mesh_prov_enable(bt_mesh_prov_bearer_t(unsigned(BT_MESH_PROV_ADV) |
                                            unsigned(BT_MESH_PROV_GATT)));

  APP_DBG("Mesh started");
}

void main(void) {
  APP_DBG("App Main");
  k_sleep(K_SECONDS(6));

  const struct device* dev;
  bool led_is_on = true;
  int ret;

  dev = device_get_binding(LED0);
  if (dev == NULL) {
    APP_DBG("No LED0 device");
    return;
  }

  ret = gpio_pin_configure(dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
  if (ret < 0) {
    APP_DBG("No GPIO PIN");
    return;
  }

  int err;

  APP_DBG("Initialising...");

  err = bt_enable(bt_ready);
  if (err) {
    APP_DBG("Bluetooth init failed (err %d)", err);
  }
  APP_DBG("Bluetooth MESH initialised");

  // Regular debug output to show the app is still running
  int iter = 0;
  while (1) {
    k_sleep(K_SECONDS(2));
    gpio_pin_set(dev, PIN, (int)led_is_on);
    led_is_on = !led_is_on;

    APP_DBG("Herald Relay main thread still running");

    // TODO Add logic here to detect failure in Herald thread, and restart to
    // resume as necessary

    // Regular health checks
    ++iter;
    if (iter > 10 && 0 == iter % 5) {
      APP_DBG("Performing Herald Healthcheck...");
      herald_healthcheck();
    }
  }
}
