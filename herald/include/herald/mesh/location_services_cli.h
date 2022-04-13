//  Copyright 2021-2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_MESH_LOCATION_SERVICES_CLI_H
#define HERALD_MESH_LOCATION_SERVICES_CLI_H

#ifdef __ZEPHYR__
#ifdef CONFIG_BT_MESH

#include "mesh.h"
#include "location_services.h"

#include <bluetooth/mesh.h>
#include <bluetooth/mesh/model_types.h>

#ifdef __cplusplus
extern "C" {
#endif

// #define BT_MESH_HERALD_LOCATION_SERVICES_CLI_MSG_LEN_PRESENCE 1
// #define BT_MESH_HERALD_LOCATION_SERVICES_CLI_MSG_LEN_PRESENCE_GET 0

/** Fwd decl for Herald location services server model **/
struct bt_mesh_herald_location_services_cli;

/** The Mesh Model definition itself **/
#define BT_MESH_MODEL_HERALD_LOCATION_SERVICES_CLI(_herald_location_services) \
		BT_MESH_MODEL_VND_CB(BT_MESH_LINUX_FOUNDATION_VENDOR_COMPANY_ID, \
			BT_MESH_HERALD_LOCATION_SERVICES_CLI_VENDOR_MODEL_ID, \
			_bt_mesh_herald_location_services_cli_op, &(_herald_location_services)->pub, \
			BT_MESH_MODEL_USER_DATA(struct bt_mesh_herald_location_services_cli, \
						_herald_location_services), \
			&_bt_mesh_herald_location_services_cli_cb)

/** Bluetooth MESH location services client handlers **/
struct bt_mesh_herald_location_services_cli_handlers {
  /** \brief Called when the node is provisioned or loaded from storage
   * \param[in] cli Location Services Client instance that has been started
   */
  void (*const start)(struct bt_mesh_herald_location_services_cli *client);

  void (*const presence)(struct bt_mesh_herald_location_services_cli *client,
                         struct bt_mesh_msg_ctx *ctx,
                         bt_mesh_herald_location_services_presence presence);
};

struct bt_mesh_herald_location_services_cli {
  struct bt_mesh_model *model;
  struct bt_mesh_model_pub pub;
  // struct net_buf_simple pub_msg;
  // uint8_t buf[BT_MESH_MODEL_BUG_LEN(BT_MESH_HERALD_LOCATION_SERVICES_CLI_OP_MESSAGE,
  //   BT_MESH_HERALD_LOCATION_SERVICES_CLI_MSG_MAXLEN_MESSAGE)];
  const struct bt_mesh_herald_location_services_cli_handlers *handlers;
  bt_mesh_herald_location_services_cli_presence presence;
};

int bt_mesh_herald_location_services_cli_presence_set(struct bt_mesh_herald_location_services_cli *client,
  bt_mesh_herald_location_services_cli_presence presence);

int bt_mesh_herald_location_services_cli_presence_get(struct bt_mesh_herald_location_services_cli *client,
  uint16_t addr);

/** @cond INTERNAL_HIDDEN */
extern const struct bt_mesh_model_op _bt_mesh_herald_location_services_cli_op[];
extern const struct bt_mesh_model_cb _bt_mesh_herald_location_services_cli_cb;
/** @endcond */

#ifdef __cplusplus
}
#endif

#endif // CONFIG_BT_MESH
#endif // platform __ZEPHYR__

#endif
