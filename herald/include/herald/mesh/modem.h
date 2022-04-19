//  Copyright 2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_MESH_MODEM_H
#define HERALD_MESH_MODEM_H

#ifdef __ZEPHYR__
#ifdef CONFIG_USB_CDC_ACM 
//& CONFIG_BT_MESH
// TODO warning message on the above if included without these configured

#include "modem_codes.h"

#include <sys/ring_buffer.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file This file contains C headers for the Herald MESH Modem protocol.
 * \note This relies upon the Zephyr CDC_ACM driver which is widely supported
 *       in all operating systems.
 */

/**
 * @brief Holds references to BT MESH and USB (Modem) pointers
 */
struct bt_mesh_modem_context {
  
};

/**
 * @brief Contains callbacks for dynamic app functionality
 */
struct bt_mesh_modem_cb {
  // int (*set)(struct bt_mesh_herald_presence_server* srv,
  //            const struct bt_mesh_herald_presence_set* set,
  //            struct bt_mesh_herald_presence_status* rsp);
};

/**
 * @brief This assumes the device has been read and we're now 
 * synchronously processing commands in a buffer.
 * 
 * @param commands The incoming command buffer
 * @param response The outgoing response buffer
 * @return int 0 if successful
 */
int bt_mesh_modem_process_commands(
  struct ring_buf* commands, struct ring_buf* response);

#ifdef __cplusplus
}
#endif

#endif // CDC ACM and MESH support
#endif // Zephyr

#endif // END HERALD_MESH_MODEM_H
