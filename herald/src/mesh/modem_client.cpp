// Copyright 2022 Herald Project Contributors
// SPDX-License-Identifier: Apache-2.0
//

#include "herald/mesh/modem_client.h"

namespace herald {
namespace mesh {
  
MeshModem::MeshModem(const int serial_port,
                     const struct bt_mesh_modem_client_cb callbacks)
  : port(serial_port),
    cb(callbacks)
{
  // Do nothing else
}

MeshModem::~MeshModem() = default;

void
MeshModem::pause()
{
  // TODO do something
}

void
MeshModem::resume()
{
  // TODO do something
}

int
MeshModem::write(uint8_t* buffer, size_t sz)
{
  // TODO do something
  return 0;
}

}  // namespace mesh
}  // namespace herald