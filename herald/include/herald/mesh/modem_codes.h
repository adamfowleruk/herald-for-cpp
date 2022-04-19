//  Copyright 2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_MESH_MODEM_CODES_H
#define HERALD_MESH_MODEM_CODES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum bt_mesh_modem_command {
const uint16_t
  // General modem system commands (no mesh effects)
  bt_mesh_modem_command_hello = 0x0000,
  // bt_mesh_modem_command_status = 0x0001,

  // // Low-level generic MESH Model commands (maps directly onto BL Mesh Ops)
  // bt_mesh_modem_command_get = 0x0010,
  // bt_mesh_modem_command_set = 0x0011,
  // bt_mesh_modem_command_subscribe = 0x0012,
  // bt_mesh_modem_command_unsubscribe =
  //     0x0013,  // unsubscribes group from mesh model
  // bt_mesh_modem_command_publish = 0x0014,
  // bt_mesh_modem_command_send = 0x0015,

  // // Higher-level Herald MESH Model commands (quicker)
  // bt_mesh_modem_command_switch_modem =
  //     0x0f00,  // set modem node for all current Herald subscriptions
  // // The above is useful for a quick HA/DR failover scenario to a
  // // different RabbitMQ connected server (with its own modem)
  // bt_mesh_modem_command_make_full_gateway =
  //     0x0f01,  // presence (all LE), Location beacon, etc.
  // // Also configures all appropriate subscriptions

  // // Node management high level commands
  // // store_update = 0x0f51,       // store modem app update in flash
  // // validate_update = 0x0f52,    // update modem from update file (no
  // reboot)
  // // update_and_reboot = 0x0f53,  // order mesh node to reboot with update
  // // store_binary = 0x0f54,   // store binary file for distribution (for
  // onward
  // // tx)
  // // delete_binary = 0x0f55,  // delete (actually just removes record of)
  // binary
  // // distribute_binary = 0x0f56,  // distribute stored binary between mesh
  // nodes
  // // store_update_from_binary =
  // //     0x0f57,              // instruct node to update app from nearby
  // binary
  // // receive_frame = 0x0f58,  // send data from of binary to usb modem
  // // receive_final_frame = 0x0f59,  // send final frame, including size

  // // // MESH networking debugging functionality
  // // mesh_nearby_nodes = 0x0f70,  // ask a mesh node to send info on other
  // nodes
  // // mesh_ping =
  // //     0x0f71,  // send packet between 2 nodes, asking both to send
  // result

  // // // LE networking status functionality
  // // le_connection_info = 0x0f80,  // ask for max/used/avail LE connection
  // info

  // // Final catch all command values
  // bt_mesh_modem_command_length = 0xfffe,  // size of dynamic value being
  // sent
  bt_mesh_modem_command_boundary =
      0xffff  // command boundary (for overflow detection)
  // }
;

/**
 * MARK: Example from the above to update a distant mesh node:-
 * Command:
 * cmd=store_binary,cid=456,id=1,framesize=128bytes,size=89543bytes,sha256=134...,boundary
 * Response: resp=ok,cid=456,boundary
 * Command: cmd=receive_frame,cid=457,id=1,seq=1,xor=0x0547,boundary
 * Response: resp=ok,cid=457,boundary
 *   Various of the above, multiple commands and cids, same binary id
 *   End with receive final frame, which includes the length of last frame
 *
 * Command:
 * cmd=distribute_binary,cid=458,id=1,fromid=0000(modem),toid=0102,boundary This
 * is a long term operation, so generates multiple responses over time Response:
 * resp=ok,cid=458,boundary Generate pub/priv key pairs Send pub for modem and
 * priv for node to 0102, with binary id=1,sha256 Open LE connection from modem
 * to 0102, negotiating l2cap packet size Response:
 * resp=op_started,cid=458,boundary Multiple frames of Herald LE unconnected
 * transfer, using secured model Response:
 * resp=op_progress,cid=458,progress=05(%),boundary Multiple frames of Herald LE
 * unconnected transfer, using secured model Response:
 * resp=op_progress,cid=458,progress=63(%),boundary Multiple frames of Herald LE
 * unconnected transfer, using secured model Response:
 * resp=op_progress,cid=458,progress=97(%),boundary Multiple frames of Herald LE
 * unconnected transfer, using secured model LE connection closed Fetch sha-256
 * confirmation from node01 for binary and confirm Response:
 * resp=op_succeeded,cid=458,boundary
 *
 * Command:
 * cmd=store_update_from_binary,cid=459,id=1,fromid=0102,toid=0105,boundary
 * Response: resp=ok,cid=459,boundary
 * Response: resp=op_started,cid=459,boundary
 * Response: resp=op_progress,cid=459,progress=06(%),boundary
 * Response: resp=op_progress,cid=459,progress=56(%),boundary
 * Response: resp=op_progress,cid=459,progress=94(%),boundary
 *   Asks over mesh for update validation
 * Response: resp=op_succeeded,cid=459,boundary
 *
 * Command: cmd=validate_update,cid=460,onnode=0105,boundary
 * Response: resp=ok,cid=460,boundary
 * Command: cmd=update_and_reboot,cid=461,onnode=0105,boundary
 * Response: resp=ok,cid=461,boundary
 *   OK when command confirmed received
 * Response: resp=op_started,cid=461,boundary
 *   Sent when node sends update status of rebooting
 * Response: resp=op_succeeded,cid=461,boundary
 *   Sent when node sends update status of success on reconnecting to mesh
 */

// enum bt_mesh_modem_response {
const uint16_t
  // General response commands (direct response)
  bt_mesh_modem_response_ok = 0x0000,
  // bt_mesh_modem_response_unknown = 0x0001,
  // bt_mesh_modem_response_invalid = 0x0002,

  // // Dynamic information data
  // bt_mesh_modem_response_model_status =
  //     0x0010,  // For all published messages arriving at the modem

  // // Long term operation monitoring
  // bt_mesh_modem_response_op_started = 0x0ff0,
  // bt_mesh_modem_response_op_progress = 0x0ff1,
  // bt_mesh_modem_response_op_failed = 0x0ff2,
  // bt_mesh_modem_response_op_succeeded = 0x0ff3,

  // // Final catch all response values
  // bt_mesh_modem_response_length = 0xfffe,  // size of dynamic value being
  // sent
  bt_mesh_modem_response_boundary =
      0xffff  // command boundary (for overflow detection)
  // }
;

#ifdef __cplusplus
}
#endif

#endif