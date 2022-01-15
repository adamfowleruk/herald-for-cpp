/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <bluetooth/mesh.h>
#include "herald/mesh/mesh.h"
#include "herald/mesh/location_services_cli.h"
#include "mesh/net.h"
#include "mesh/transport.h"
#include <string.h>

#include <logging/log.h>
LOG_MODULE_DECLARE(location_services);

// BUILD_ASSERT(BT_MESH_MODEL_BUF_LEN(BT_MESH_HERALD_LOCATION_SERVICES_CLI_OP_MESSAGE,
// 				   BT_MESH_HERALD_LOCATION_SERVICES_CLI_MSG_MAXLEN_MESSAGE) <=
// 		    BT_MESH_RX_SDU_MAX,
// 	     "The message must fit inside an application SDU.");
// BUILD_ASSERT(BT_MESH_MODEL_BUF_LEN(BT_MESH_HERALD_LOCATION_SERVICES_CLI_OP_MESSAGE,
// 				   BT_MESH_HERALD_LOCATION_SERVICES_CLI_MSG_MAXLEN_MESSAGE) <=
// 		    BT_MESH_TX_SDU_MAX,
// 	     "The message must fit inside an application SDU.");

static void encode_presence(struct net_buf_simple *buf,
			    enum bt_mesh_herald_location_services_cli_presence presence)
{
	bt_mesh_model_msg_init(buf, BT_MESH_LINUX_FOUNDATION_OP_SET);
	net_buf_simple_add_u8(buf, presence);
}

static int handle_presence(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
			   struct net_buf_simple *buf)
{
	struct bt_mesh_herald_location_services_cli *client = model->user_data;
	enum bt_mesh_herald_location_services_cli_presence presence;

	presence = net_buf_simple_pull_u8(buf);

	if (client->handlers->presence) {
		client->handlers->presence(client, ctx, presence);
	}

	return 0;
}

static int handle_presence_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
			       struct net_buf_simple *buf)
{
	struct bt_mesh_herald_location_services_cli *client = model->user_data;

	BT_MESH_MODEL_BUF_DEFINE(msg, BT_MESH_LINUX_FOUNDATION_OP_SET,
				 BT_MESH_HERALD_LOCATION_SERVICES_CLI_MSG_LEN_PRESENCE);

	encode_presence(&msg, client->presence);

	(void) bt_mesh_model_send(client->model, ctx, &msg, NULL, NULL);

	return 0;
}

const struct bt_mesh_model_op _bt_mesh_herald_location_services_cli_op[] = {
	{
		BT_MESH_LINUX_FOUNDATION_OP_SET,
		BT_MESH_LEN_EXACT(BT_MESH_HERALD_LOCATION_SERVICES_CLI_MSG_LEN_PRESENCE),
		handle_presence
	},
	{
		BT_MESH_LINUX_FOUNDATION_OP_GET,
		BT_MESH_LEN_EXACT(BT_MESH_HERALD_LOCATION_SERVICES_CLI_MSG_LEN_PRESENCE_GET),
		handle_presence_get
	},
	BT_MESH_MODEL_OP_END,
};


static int bt_mesh_herald_location_services_cli_update_handler(struct bt_mesh_model *model)
{
	struct bt_mesh_herald_location_services_cli *client = model->user_data;

	/* Continue publishing current presence. */
	encode_presence(model->pub->msg, client->presence);

	return 0;
}

#ifdef CONFIG_BT_SETTINGS
static int bt_mesh_herald_location_services_cli_settings_set(struct bt_mesh_model *model,
					 const char *name,
					 size_t len_rd,
					 settings_read_cb read_cb,
					 void *cb_arg)
{
	struct bt_mesh_herald_location_services_cli *client = model->user_data;

	if (name) {
		return -ENOENT;
	}

	ssize_t bytes = read_cb(cb_arg, &client->presence,
				sizeof(client->presence));
	if (bytes < 0) {
		return bytes;
	}

	if (bytes != 0 && bytes != sizeof(client->presence)) {
		return -EINVAL;
	}

	return 0;
}
#endif

static int bt_mesh_herald_location_services_cli_init(struct bt_mesh_model *model)
{
	struct bt_mesh_herald_location_services_cli *client = model->user_data;

	client->model = model;

	// net_buf_simple_init_with_data(&client->pub_msg, client->buf,
	// 			      sizeof(client->buf));
	// client->pub.msg = &client->pub_msg;
	client->pub.update = bt_mesh_herald_location_services_cli_update_handler;

	return 0;
}

static int bt_mesh_herald_location_services_cli_start(struct bt_mesh_model *model)
{
	struct bt_mesh_herald_location_services_cli *client = model->user_data;

	if (client->handlers->start) {
		client->handlers->start(client);
	}

	return 0;
}

static void bt_mesh_herald_location_services_cli_reset(struct bt_mesh_model *model)
{
	struct bt_mesh_herald_location_services_cli *client = model->user_data;

	client->presence = BT_MESH_HERALD_PRESENCE_OBSERVED;

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		(void) bt_mesh_model_data_store(model, true, NULL, NULL, 0);
	}
}

const struct bt_mesh_model_cb _bt_mesh_herald_location_services_cli_cb = {
	.init = bt_mesh_herald_location_services_cli_init,
	.start = bt_mesh_herald_location_services_cli_start,
#ifdef CONFIG_BT_SETTINGS
	.settings_set = bt_mesh_herald_location_services_cli_settings_set,
#endif
	.reset = bt_mesh_herald_location_services_cli_reset,
};

int bt_mesh_herald_location_services_cli_presence_set(struct bt_mesh_herald_location_services_cli *client,
			 enum bt_mesh_herald_location_services_cli_presence presence)
{
	if (presence != client->presence) {
		client->presence = presence;

		if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
			(void) bt_mesh_model_data_store(client->model, true,
						NULL, &presence,
						sizeof(client->presence));
		}
	}

	encode_presence(client->model->pub->msg, client->presence);

	return bt_mesh_model_publish(client->model);
}

int bt_mesh_herald_location_services_cli_presence_get(struct bt_mesh_herald_location_services_cli *client,
				  uint16_t addr)
{
	struct bt_mesh_msg_ctx ctx = {
		.addr = addr,
		.app_idx = client->model->keys[0],
		.send_ttl = BT_MESH_TTL_DEFAULT,
		.send_rel = true,
	};

	BT_MESH_MODEL_BUF_DEFINE(buf, BT_MESH_LINUX_FOUNDATION_OP_GET,
				 BT_MESH_HERALD_LOCATION_SERVICES_CLI_MSG_LEN_PRESENCE_GET);
	bt_mesh_model_msg_init(&buf, BT_MESH_LINUX_FOUNDATION_OP_GET);

	return bt_mesh_model_send(client->model, &ctx, &buf, NULL, NULL);
}







int bt_mesh_herald_presence_message_send(struct bt_mesh_chat_cli *chat, struct bt_mesh_herald_location_services_presence *msg) {
	struct net_buf_simple *buf = chat->model->pub->msg;

	bt_mesh_model_msg_init(buf, BT_MESH_LINUX_FOUNDATION_OP_MESSAGE);

	net_buf_simple_add_u8(buf, msg->address[0]);
	net_buf_simple_add_u8(buf, msg->address[1]);
	net_buf_simple_add_u8(buf, msg->address[2]);
	net_buf_simple_add_u8(buf, msg->address[3]);
	net_buf_simple_add_u8(buf, msg->address[4]);
	net_buf_simple_add_u8(buf, msg->address[5]);
	net_buf_simple_add_i8(buf, msg->rssi);
	net_buf_simple_add_u8(buf, msg->presence);

	return bt_mesh_model_publish(chat->model);
};