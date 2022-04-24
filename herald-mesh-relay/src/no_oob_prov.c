/*
 * Copyright 2022 Herald Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#include "no_oob_prov.h"

#include <bluetooth/mesh.h>

#include <drivers/hwinfo.h>

#include <logging/log.h>

LOG_MODULE_REGISTER(no_oob_bt_mesh_prov, CONFIG_APP_LOG_LEVEL);

static struct k_work_delayable oob_work;

// Storage location of extern const struct defined in no_oob_prov.h
const struct no_oob_prov_cb* prov_callbacks;

static void oob_timer_handler(struct k_work* work) {
  // TODO do something
}

static void prov_complete(uint16_t net_idx, uint16_t src) {
  LOG_DBG("Provisioning complete. Address: 0x%04x\n", src);
  if (prov_callbacks && prov_callbacks->provisioned) {
    prov_callbacks->provisioned(net_idx, src);
  }
}

static void prov_reset(void) {
  if (prov_callbacks && prov_callbacks->reset) {
    prov_callbacks->reset();
  }
  bt_mesh_prov_enable(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT);
}

static uint8_t dev_uuid[16];

static const struct bt_mesh_prov prov = {
    .uuid = dev_uuid,
    .output_actions = (0), // No OOB
    .complete = prov_complete,
    .reset = prov_reset,
};

const struct bt_mesh_prov* bt_mesh_no_oob_prov_init(
    const struct no_oob_prov_cb* cbs) {
  prov_callbacks = cbs;
  /* Generate an RFC-4122 version 4 compliant UUID.
    * Format:
    *
    * 0                   1                   2                   3
    * 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    * |                          time_low                             |
    * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    * |       time_mid                |         time_hi_and_version   |
    * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    * |clk_seq_hi_res |  clk_seq_low  |         node (0-1)            |
    * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    * |                         node (2-5)                            |
    * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    *
    * Where the 4 most significant bits of time_hi_and_version shall be
    * 0b0010 and the 2 most significant bits of clk_seq_hi_res shall be
    * 0b10. The remaining fields have no required values, and are fetched
    * from the HW info device ID. The fields are encoded in big endian
    * format.
    *
    * https://tools.ietf.org/html/rfc4122
    */
  hwinfo_get_device_id(dev_uuid, sizeof(dev_uuid));
  dev_uuid[6] = (dev_uuid[6] & BIT_MASK(4)) | BIT(6);
  dev_uuid[8] = (dev_uuid[8] & BIT_MASK(6)) | BIT(7);

  k_work_init_delayable(&oob_work, oob_timer_handler);

  return &prov;
}
