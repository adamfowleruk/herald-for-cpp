//  Copyright 2021-2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_MESH_PRESENCE_SERVER_H
#define HERALD_MESH_PRESENCE_SERVER_H

#ifdef __ZEPHYR__
// #ifdef CONFIG_BT_MESH

#include "mesh.h"
#include "presence.h"

#include <bluetooth/mesh.h>
#include <bluetooth/mesh/model_types.h>
#include <bluetooth/mesh/models.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BT_MESH_HERALD_PRESENCE_SERVER_INIT(_handlers) \
  { \
    .handlers = _handlers, \
  }

// fwd decl
struct bt_mesh_herald_presence_server;

#define BT_MESH_HERALD_PRESENCE_SERVER_PUB_DEFINE(_name, _max_faults) \
  BT_MESH_MODEL_PUB_DEFINE(_name, NULL, (1 + 3 + (_max_faults)))

struct bt_mesh_herald_presence_server_cb {
  int (*set)(struct bt_mesh_herald_presence_server *srv, 
    const struct bt_mesh_herald_presence_set *set, 
    struct bt_mesh_herald_presence_status *rsp);
};

struct bt_mesh_herald_presence_server {
  struct bt_mesh_model *model;
  struct bt_mesh_model_pub pub;
  /** Publication message. */
  struct net_buf_simple pub_msg;
  /** Publication message buffer. */
  uint8_t buf[BT_MESH_MODEL_BUF_LEN(
      BT_MESH_LINUX_FOUNDATION_OP_SET,
      BT_MESH_HERALD_PRESENCE_MSG_MAXLEN_STATUS)];
  const struct bt_mesh_herald_presence_server_cb* cb;
};

/** @cond INTERNAL_HIDDEN */
static struct bt_mesh_model_pub presence_pub;
static struct bt_mesh_herald_presence_server presence_server;
/** @endcond */

/**
 * @brief Declares a Herald Presence Server
 * @note Try to use BT_MESH_HERALD_PRESENCE_ELEM in its entirety instead
 */
#define BT_MESH_MODEL_HERALD_PRESENCE_SERVER(srv, _pub)            \
  BT_MESH_MODEL_VND_CB(BT_MESH_LINUX_FOUNDATION_VENDOR_COMPANY_ID, \
      BT_MESH_HERALD_PRESENCE_SERVER_VENDOR_MODEL_ID,              \
      bt_mesh_herald_presence_server_op, _pub, srv,                \
      &bt_mesh_herald_presence_server_cb)

/** @cond INTERNAL_HIDDEN */
extern const struct bt_mesh_model_op bt_mesh_herald_presence_server_op[];
extern const struct bt_mesh_model_cb bt_mesh_herald_presence_server_cb;
/** @endcond */





/// MARK: BT MESH herald presence server on/off details

/**
 * @brief App level callbacks to be notified of Herald Presence Server activity
 * @see bt_mesh_herald_presence_register_callbacks
 */
struct bt_mesh_herald_presence_server_onoff_cb {
  void (*on)();
  void (*off)();
};

/** @cond INTERNAL_HIDDEN */
struct bt_mesh_herald_presence_server_onoff_ctx {
  struct bt_mesh_onoff_srv server;
  const struct bt_mesh_herald_presence_server_onoff_cb* callbacks;
  bool enabled;
};

extern struct bt_mesh_herald_presence_server_onoff_ctx presence_onoff_ctx;

/** @endcond */

/**
 * @brief Declares a Herald Presence OnOff Server Model
 * @note Try to use BT_MESH_HERALD_PRESENCE_ELEM in its entirety instead
 */
#define BT_MESH_MODEL_HERALD_PRESENCE_ONOFF_SERVER \
  BT_MESH_MODEL_ONOFF_SRV(&presence_onoff_ctx.server)

/**
 * @brief Create a Herald Presence Server pre-configured Element.
 * @param _id The Mesh Element index number (usually 2)
 * @param _srv The address of the apps bt_mesh_herald_presence_server struct
 * @param _pub The address of the apps presence server pub. @see
 * BT_MESH_HERALD_PRESENCE_SERVER_PUB_DEFINE
 */
#define BT_MESH_HERALD_PRESENCE_ELEM(_id) \
  BT_MESH_ELEM(_id, \
    BT_MESH_MODEL_LIST( \
      BT_MESH_MODEL_HERALD_PRESENCE_ONOFF_SERVER, \
    ), \
    BT_MESH_MODEL_LIST( \
      BT_MESH_MODEL_HERALD_PRESENCE_SERVER( \
        &presence_server, &presence_pub \
      ), \
    ) \
  )

// MARK: Server side API for Herald to/from MESH Gateway to call.

/**
 * @brief Is the presence server on off model set to on?
 *
 * @return true If presence server is enabled
 * @return false If presence server is disabled
 */
bool bt_mesh_herald_presence_enabled();

/**
 * @brief Register a set of herald presence server application level callbacks
 * 
 * @param cbs The application callbacks to register (may be partially populated)
 */
void bt_mesh_herald_presence_register_callbacks(
  const struct bt_mesh_herald_presence_server_onoff_cb* cbs);

/**
 * @brief Publish a nearby device message
 * 
 * @param macOfSix 6 byte ble mac address
 * @param rssi RSSI value (mean over last period)
 * @param status Whether a new update, or the device is out of range
 * @return int 0 for success if message send accepted, negative otherwise
 */
int bt_mesh_herald_presence_share(uint8_t *macOfSix, int8_t rssi, 
  enum bt_mesh_model_herald_presence_status status);


#ifdef __cplusplus
}
#endif

// #endif // CONFIG_BT_MESH
#endif // platform __ZEPHYR__

#endif // HERALD_MESH_PRESENCE_SERVER_H
