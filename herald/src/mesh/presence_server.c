
#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>
#include <bluetooth/mesh/models.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#define LOG_MODULE_NAME bt_mesh_herald_presence_server
#include "common/log.h"

#include "herald/mesh/mesh.h"
#include "herald/mesh/presence.h"
#include "herald/mesh/presence_server.h"

/**
 * @file Herald Presence MESH Model Server. Used by Herald Presence 
 * Servers (herald-mesh-relay app instances) to provide a Herald
 * Presence reporting function via MESH message publishing.
 */

/* Location Services Server context of the primary element */
struct bt_mesh_herald_presence_server *herald_presence_server;

static int send_herald_presence_server_status(struct bt_mesh_model *model,
             struct bt_mesh_msg_ctx *ctx)
{
  /* Needed size: opcode (2 bytes) + msg + MIC */
  BT_MESH_MODEL_BUF_DEFINE(msg, BT_MESH_LINUX_FOUNDATION_OP_STATUS, 
    BT_MESH_HERALD_PRESENCE_MSG_MAXLEN_STATUS);

  bt_mesh_model_msg_init(&msg, BT_MESH_LINUX_FOUNDATION_OP_STATUS);

  // TODO append data to buffer to send from status message
  // net_buf_simple_add_u8(&msg, model->pub->period_div);

  if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
    BT_ERR("Unable to send Herald Location Services Server Status");
  }

  return 0;
}

static int herald_presence_server_get(struct bt_mesh_model *model,
            struct bt_mesh_msg_ctx *ctx,
            struct net_buf_simple *buf)
{
  BT_DBG("");

  return send_herald_presence_server_status(model, ctx);
}

static int herald_presence_server_set_unrel(struct bt_mesh_model *model,
            struct bt_mesh_msg_ctx *ctx,
            struct net_buf_simple *buf)
{
  // uint8_t period;

  //TODO fetch bool values

  // period = net_buf_simple_pull_u8(buf);
  // if (period > 15) {
  //   BT_WARN("Prohibited period value %u", period);
  //   return -EINVAL;
  // }

  // BT_DBG("period %u", period);

  // model->pub->period_div = period;

  return 0;
}

static int herald_presence_server_set(struct bt_mesh_model *model,
           struct bt_mesh_msg_ctx *ctx,
           struct net_buf_simple *buf)
{
  int err;

  BT_DBG("");

  err = herald_presence_server_set_unrel(model, ctx, buf);
  if (err) {
    return err;
  }

  return send_herald_presence_server_status(model, ctx);
}


const struct bt_mesh_model_op bt_mesh_herald_presence_server_op[] = {
  { BT_MESH_LINUX_FOUNDATION_OP_GET,        BT_MESH_LEN_EXACT(0),   herald_presence_server_get },
  { BT_MESH_LINUX_FOUNDATION_OP_SET_UNACK,  BT_MESH_LEN_EXACT(BT_MESH_HERALD_PRESENCE_MSG_MAXLEN_SET), 
    herald_presence_server_set_unrel },   
  { BT_MESH_LINUX_FOUNDATION_OP_SET,        BT_MESH_LEN_EXACT(BT_MESH_HERALD_PRESENCE_MSG_MINLEN_SET),   
    herald_presence_server_set },
  BT_MESH_MODEL_OP_END,
};

static int herald_presence_server_start(struct bt_mesh_model *model)
{
  BT_DBG("herald_presence_server_start called");
  return 0;
}

static int herald_presence_server_init(struct bt_mesh_model *model)
{
  BT_DBG("herald_presence_server_init called");
  struct bt_mesh_herald_presence_server *srv = 
    (struct bt_mesh_herald_presence_server*)model->user_data;

  if (!srv) {
    BT_ERR("No Herald Location Services Server context provided");
    return -EINVAL;
  }

  if (!model->pub) {
    BT_ERR("Herald Location Services Server has no publication support");
    return -EINVAL;
  }

  // model->pub->update = herald_presence_pub_update;

  // k_work_init_delayable(&srv->attn_timer, attention_off);

  srv->model = model;

  // TODO change this to a secondary as it's now separated out
  if (bt_mesh_model_in_primary(model)) {
    herald_presence_server = srv;
  }

  return 0;
}

const struct bt_mesh_model_cb bt_mesh_herald_presence_server_cb = {
  .init = herald_presence_server_init,
  .start = herald_presence_server_start,
};

// TODO MOVE THESE SO THEY ARE HANDLED BY THE PRESENCE SERVER C FILE INSTEAD
// THEN DEFINE AN APP-LEVEL CALLBACK AS PER
// bt_mesh_herald_presence_register_callbacks

// Define herald location services server handlers
static int presence_updated(struct bt_mesh_herald_presence_server* srv,
                            const struct bt_mesh_herald_presence_set* set,
                            struct bt_mesh_herald_presence_status* rsp);

static const struct bt_mesh_herald_presence_server_cb presence_handlers = {
    .set = presence_updated};

static int presence_updated(struct bt_mesh_herald_presence_server* srv,
                            const struct bt_mesh_herald_presence_set* set,
                            struct bt_mesh_herald_presence_status* rsp) {
  LOG_DBG("presence_updated() called");
  // TODO do work
  return 0;
}

BT_MESH_HERALD_PRESENCE_SERVER_PUB_DEFINE(presence_pub, 11);

static struct bt_mesh_herald_presence_server presence_server = {
  .cb = &presence_handlers
};

// FWD DECL
static void presence_onoff_set(struct bt_mesh_onoff_srv* srv,
                               struct bt_mesh_msg_ctx* ctx,
                               const struct bt_mesh_onoff_set* set,
                               struct bt_mesh_onoff_status* rsp);

// FWD DECL
static void presence_onoff_get(struct bt_mesh_onoff_srv* srv,
                               struct bt_mesh_msg_ctx* ctx,
                               struct bt_mesh_onoff_status* rsp);

static const struct bt_mesh_onoff_srv_handlers onoff_handlers = {
    .set = presence_onoff_set,
    .get = presence_onoff_get,
};

/**
 * MARK: Presence on off server callbacks
 */

struct bt_mesh_herald_presence_server_onoff_ctx presence_onoff_ctx = {
  .server  = BT_MESH_ONOFF_SRV_INIT(&onoff_handlers),
  .enabled = false,
};

static void presence_onoff_status(struct bt_mesh_onoff_status *status)
{
  // We always immediately change, so report the current status
  status->remaining_time = 0;
  status->target_on_off = presence_onoff_ctx.enabled;
  status->present_on_off = presence_onoff_ctx.enabled;
}

static void presence_onoff_set(struct bt_mesh_onoff_srv* srv,
                               struct bt_mesh_msg_ctx* ctx,
                               const struct bt_mesh_onoff_set* set,
                               struct bt_mesh_onoff_status* rsp)
{
  struct bt_mesh_herald_presence_server_onoff_ctx* presence_ctx =
    CONTAINER_OF(srv, struct bt_mesh_herald_presence_server_onoff_ctx, server);

  // Copy over value
  bool wasEnabled = presence_ctx->enabled;
  presence_ctx->enabled = set->on_off;

  // TODO handle specified transition time

  // respond if an ack requested
  if (rsp) {
    presence_onoff_status(rsp);
  }

  // check if status has changed, and thus an application event needs firing
  if (wasEnabled != set->on_off && presence_ctx->callbacks) {
    if (set->on_off) {
      if (NULL != presence_ctx->callbacks->on) {
        presence_ctx->callbacks->on();
      }
    } else {
      if (NULL != presence_ctx->callbacks->off) {
        presence_ctx->callbacks->off();
      }
    }
  }
}

static void presence_onoff_get(struct bt_mesh_onoff_srv* srv,
                               struct bt_mesh_msg_ctx* ctx,
                               struct bt_mesh_onoff_status* rsp)
{
  presence_onoff_status(rsp);
}

/*
 * Server side API for a Herald Presence Server app to call.
 */
void bt_mesh_herald_presence_register_callbacks(
  const struct bt_mesh_herald_presence_server_onoff_cb* cbs)
{
  presence_onoff_ctx.callbacks = cbs;
}

bool bt_mesh_herald_presence_enabled()
{
  return presence_onoff_ctx.enabled;
}

int bt_mesh_herald_presence_share(
  uint8_t* macOfSix, int8_t rssi,
  enum bt_mesh_model_herald_presence_status status)
{
  // Ensure our server handle has been initialised
  if (NULL == herald_presence_server) {
    return -1;
  }

  // Fetch model
  struct bt_mesh_model* mdl = herald_presence_server->model;
  BT_DBG("Got base mesh model");

  // Ensure we've been bound to a publishing destination by our provisioner
  if (mdl->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
    BT_ERR("Does not have publication address");
    return -2;
  }

  // Fetch model message buffer
  struct net_buf_simple* msg = mdl->pub->msg;

  BT_DBG("Max model message size: %d", mdl->pub->msg->size);
  BT_DBG("Model publish address: %d", mdl->pub->addr);
  BT_DBG("Model publish key: %d", mdl->pub->key);
  BT_DBG("Model publish TTL: %d", mdl->pub->ttl);

  // Reset model message buffer
  bt_mesh_model_msg_init(msg, BT_MESH_LINUX_FOUNDATION_OP_STATUS);
  BT_DBG("Max model message size now: %d", mdl->pub->msg->size);

  // append data to buffer to send from status message
  net_buf_simple_add_u8(msg, macOfSix[0]);
  net_buf_simple_add_u8(msg, macOfSix[1]);
  net_buf_simple_add_u8(msg, macOfSix[2]);
  net_buf_simple_add_u8(msg, macOfSix[3]);
  net_buf_simple_add_u8(msg, macOfSix[4]);
  net_buf_simple_add_u8(msg, macOfSix[5]);
  net_buf_simple_add_u8(msg, (uint8_t)rssi);
  net_buf_simple_add_u8(msg, (uint8_t)status);
  BT_DBG("Message length to send: %d", msg->len);

  // share presence
  return bt_mesh_model_publish(mdl);
}
