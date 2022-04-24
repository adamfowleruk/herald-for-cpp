/*
 * Copyright 2022 Herald Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef NO_OOB_PROV_H__
#define NO_OOB_PROV_H__

#include <bluetooth/mesh.h>

#ifdef __cplusplus
extern "C" {
#endif

// Define callbacks for main module
struct no_oob_prov_cb {
  void (*provisioned)(uint16_t net_idx, uint16_t src);
  void (*reset)();
};

extern const struct no_oob_prov_cb* prov_callbacks;

/** @brief Dummy provisioner with no OOB support
 *
 * @return The provisioning properties to pass to @em bt_mesh_init().
 */
const struct bt_mesh_prov* bt_mesh_no_oob_prov_init(
  const struct no_oob_prov_cb* prov_cbs);

#ifdef __cplusplus
}
#endif

#endif /* NO_OOB_PROV_H__ */
