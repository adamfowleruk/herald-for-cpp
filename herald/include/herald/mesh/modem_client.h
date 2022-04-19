// Copyright 2022 Herald Project Contributors
// SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_MESH_MODEM_CLIENT_H
#define HERALD_MESH_MODEM_CLIENT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file Defines a client API for interacting over a tty device
 * to a Herald MESH modem.
 */

/**
 * @brief A set of callbacks for the mesh modem client
 */
struct bt_mesh_modem_client_cb {
  int (*read)(uint8_t* buffer,size_t sz);
};

#ifdef __cplusplus
} // ends extern C linking

// TODO include a C++ class wrapper here too

namespace herald {
namespace mesh {

class MeshModem {
public:
  MeshModem(const int serial_port, const struct bt_mesh_modem_client_cb callbacks);
  ~MeshModem();

  void pause();
  void resume();

  int write(uint8_t* buffer, size_t sz);
private:
  int port;
  const struct bt_mesh_modem_client_cb cb;
};

}
}

#endif // c++

#endif // include