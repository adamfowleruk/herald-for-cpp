/*
 * Copyright 2020-2022 Herald Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh/models.h>
#include <dk_buttons_and_leds.h> // TODO remove DK link here
#include "model_handler.h"

#include "herald/mesh/presence.h"
#include "herald/mesh/presence_server.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(meshmodel, CONFIG_APP_LOG_LEVEL);

/* Set up a repeating delayed work to blink the DK's LEDs when attention is
 * requested.
 */
static struct k_work_delayable attention_blink_work;
static bool attention;

static void attention_blink(struct k_work *work)
{
  static int idx;

  if (attention) {
    dk_set_leds(BIT((idx++) % 2));
    k_work_reschedule(&attention_blink_work, K_MSEC(30));
  } else {
    dk_set_leds(DK_NO_LEDS_MSK);
  }
}

static void attention_on(struct bt_mesh_model *mod)
{
  attention = true;
  k_work_reschedule(&attention_blink_work, K_NO_WAIT);
}

static void attention_off(struct bt_mesh_model *mod)
{
  /* Will stop rescheduling blink timer */
  attention = false;
}

static const struct bt_mesh_health_srv_cb health_server_cb = {
  .attn_on = attention_on,
  .attn_off = attention_off,
};

static struct bt_mesh_health_srv health_server = {
  .cb = &health_server_cb,
};

BT_MESH_HEALTH_PUB_DEFINE(health_pub, 0);


// HERALD PRESENCE SERVER CALLBACKS FROM HERE

static void presence_on()
{
  LOG_DBG("Presence On");
}

static void presence_off()
{
  LOG_DBG("Presence Off");
}

const struct bt_mesh_herald_presence_server_onoff_cb presence_cbs = {
  .on = presence_on,
  .off = presence_off,
};

static struct bt_mesh_elem elements[] = {
  BT_MESH_ELEM(
    1,
    BT_MESH_MODEL_LIST(
      BT_MESH_MODEL_CFG_SRV,
      BT_MESH_MODEL_HEALTH_SRV(&health_server, &health_pub),
    ),
    BT_MESH_MODEL_LIST(
    )
  ),
  BT_MESH_HERALD_PRESENCE_ELEM(2),
};

static const struct bt_mesh_comp comp = {
  .cid = BT_MESH_LINUX_FOUNDATION_VENDOR_COMPANY_ID,
  .elem = elements,
  .elem_count = ARRAY_SIZE(elements)
};

const struct bt_mesh_comp *model_handler_init(void)
{
  k_work_init_delayable(&attention_blink_work, attention_blink);
  bt_mesh_herald_presence_register_callbacks(&presence_cbs);

  return &comp;
}

