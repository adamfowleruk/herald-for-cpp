
#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#define LOG_MODULE_NAME bt_mesh_herald_location_services_srv
#include "common/log.h"

#include "herald/mesh/mesh.h"
#include "herald/mesh/location_services_srv.h"

/* Location Services Server context of the primary element */
struct bt_mesh_herald_location_services_srv *herald_location_srv;

static int send_herald_location_services_srv_status(struct bt_mesh_model *model,
				     struct bt_mesh_msg_ctx *ctx)
{
	/* Needed size: opcode (2 bytes) + msg + MIC */
	BT_MESH_MODEL_BUF_DEFINE(msg, BT_MESH_LINUX_FOUNDATION_OP_STATUS, 
    BT_MESH_HERALD_LOCATION_SERVICES_MSG_MAXLEN_STATUS);

	bt_mesh_model_msg_init(&msg, BT_MESH_LINUX_FOUNDATION_OP_STATUS);

  // TODO append data to buffer to send from statuc message
	// net_buf_simple_add_u8(&msg, model->pub->period_div);

	if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
		BT_ERR("Unable to send Herald Location Services Server Status");
	}

	return 0;
}

static int herald_location_services_srv_get(struct bt_mesh_model *model,
			      struct bt_mesh_msg_ctx *ctx,
			      struct net_buf_simple *buf)
{
	BT_DBG("");

	return send_herald_location_services_srv_status(model, ctx);
}

static int herald_location_services_srv_set_unrel(struct bt_mesh_model *model,
				    struct bt_mesh_msg_ctx *ctx,
				    struct net_buf_simple *buf)
{
	// uint8_t period;

  //TODO fetch bool values

	// period = net_buf_simple_pull_u8(buf);
	// if (period > 15) {
	// 	BT_WARN("Prohibited period value %u", period);
	// 	return -EINVAL;
	// }

	// BT_DBG("period %u", period);

	// model->pub->period_div = period;

	return 0;
}

static int herald_location_services_srv_set(struct bt_mesh_model *model,
			     struct bt_mesh_msg_ctx *ctx,
			     struct net_buf_simple *buf)
{
	int err;

	BT_DBG("");

	err = herald_location_services_srv_set_unrel(model, ctx, buf);
	if (err) {
		return err;
	}

	return send_herald_location_services_srv_status(model, ctx);
}


const struct bt_mesh_model_op bt_mesh_herald_location_services_srv_op[] = {
	{ BT_MESH_LINUX_FOUNDATION_OP_GET,        BT_MESH_LEN_EXACT(0),   herald_location_services_srv_get },
	{ BT_MESH_LINUX_FOUNDATION_OP_SET_UNACK,  BT_MESH_LEN_EXACT(BT_MESH_HERALD_LOCATION_SERVICES_MSG_MAXLEN_SET), 
    herald_location_services_srv_set_unrel },   
	{ BT_MESH_LINUX_FOUNDATION_OP_SET,        BT_MESH_LEN_EXACT(BT_MESH_HERALD_LOCATION_SERVICES_MSG_MINLEN_SET),   
    herald_location_services_srv_set },
	BT_MESH_MODEL_OP_END,
};

static int herald_location_services_srv_init(struct bt_mesh_model *model)
{
	struct bt_mesh_herald_location_services_srv *srv = model->user_data;

	if (!srv) {
		BT_ERR("No Herald Location Services Server context provided");
		return -EINVAL;
	}

	if (!model->pub) {
		BT_ERR("Herald Location Services Server has no publication support");
		return -EINVAL;
	}

	// model->pub->update = herald_location_services_pub_update;

	// k_work_init_delayable(&srv->attn_timer, attention_off);

	srv->model = model;

	if (bt_mesh_model_in_primary(model)) {
		herald_location_srv = srv;
	}

	return 0;
}

const struct bt_mesh_model_cb bt_mesh_herald_location_services_srv_cb = {
	.init = herald_location_services_srv_init,
};