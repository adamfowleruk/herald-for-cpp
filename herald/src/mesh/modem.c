//  Copyright 2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#include "herald/mesh/modem_codes.h"
#include "herald/mesh/modem.h"

// #include "herald/mesh/mesh.h"
// #include "herald/mesh/presence.h"
// #include "herald/mesh/presence_client.h"

#include <sys/ring_buffer.h>

// #include <logging/log.h>
// LOG_MODULE_DECLARE(bt_mesh_modem, LOG_LEVEL_INF);

static uint64_t bt_mesh_modem_version = 0x00010000;  // Sem Ver 0.1.0-release

int bt_mesh_modem_process_commands(struct ring_buf* commands,
                                   struct ring_buf* response) {
  
  size_t sz = ring_buf_size_get(commands);

  // reusable variables
  uint16_t cmd;      // command code
  uint16_t cid;      // command ID
  uint32_t success;  // read/write success code

  // Assume we're starting at the beginning of a buffer that hasn't overflown
  while (5 < sz) { // must have at least command, command id, boundary
    // 1. Read first two bytes as the command
    ring_buf_get(commands, (uint8_t*)&cmd, 2);
    
    ring_buf_get(commands, (uint8_t*)&cid, 2);
    

    // 2. Now process the command

    // TODO add error handling for each put
    // TODO ensure the output buffer has space before we write to it
    switch (cmd) {
      case bt_mesh_modem_command_hello:
        ring_buf_put(response, (uint8_t*)&bt_mesh_modem_response_ok, 2);
        // put the command ID back to the modem
        ring_buf_put(response, (uint8_t*)&cid, 2);
        break;
      // case bt_mesh_modem_command_status:
      //   break;

      // // model commands
      // case bt_mesh_modem_command_get:
      //   break;
      // case bt_mesh_modem_command_set:
      //   break;
      // case bt_mesh_modem_command_subscribe:
      //   break;
      // case bt_mesh_modem_command_unsubscribe:
      //   break;
      // case bt_mesh_modem_command_publish:
      //   break;
      // case bt_mesh_modem_command_send:
      //   break;

      // higher level herald convenience commands
      default:
        // unknown command
        // LOG_ERR("Unknown command code %d (command ID %d)",cmd,cid);
        // success = ring_buf_put(response,
                              //  (uint8_t*)&bt_mesh_modem_response_unknown, 2);
        // put the command ID back to the modem
        ring_buf_put(response, (uint8_t*)&cid, 2);
    }
    // send boundary
    ring_buf_put(response, (uint8_t*)&bt_mesh_modem_response_boundary, 2);
    // TODO auto advance to next boundary here (to 0xffff)
    sz = ring_buf_size_get(commands);
  }  // end while has more commands

  return 0;
}