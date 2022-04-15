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
	uint8_t buf[BT_MESH_MODEL_BUF_LEN(BT_MESH_LINUX_FOUNDATION_OP_SET,
					  BT_MESH_HERALD_PRESENCE_MSG_LEN)];
  const struct bt_mesh_herald_presence_server_cb *cb;
};



#define BT_MESH_MODEL_HERALD_PRESENCE_SERVER(srv, _pub) \
  BT_MESH_MODEL_CB(BT_MESH_HERALD_PRESENCE_SERVER_VENDOR_MODEL_ID, \
    bt_mesh_herald_presence_server_op, _pub, srv, &bt_mesh_herald_presence_server_cb)


/** @cond INTERNAL_HIDDEN */
extern const struct bt_mesh_model_op bt_mesh_herald_presence_server_op[];
extern const struct bt_mesh_model_cb bt_mesh_herald_presence_server_cb;
/** @endcond */






// MARK: Server side API for Herald to/from MESH Gateway to call.

int bt_mesh_herald_presence_share(uint8_t *macOfSix, int8_t rssi, 
	enum bt_mesh_model_herald_presence_status status);


#ifdef __cplusplus
}
#endif

// #endif // CONFIG_BT_MESH
#endif // platform __ZEPHYR__

#endif // HERALD_MESH_PRESENCE_SERVER_H
