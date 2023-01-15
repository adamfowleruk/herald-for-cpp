//  Copyright 2021-2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

// This file holds models common to location server and client headers

#ifndef HERALD_MESH_LOCATION_H
#define HERALD_MESH_LOCATION_H

#ifdef __ZEPHYR__
#ifdef CONFIG_BT_MESH

#include "mesh.h"

#include <bluetooth/mesh/model_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Herald campus navigation capabilities via MESH.
 */

/**
 * \brief Herald location beacon model, adapted for MESH
 * and campus navigation.
 * @see herald::payload::beacon::ConcreteBeaconPayloadDataSupplierV1
 */
struct bt_mesh_model_herald_location_beacon {
  /** \brief Herald Location Country **/
  uint16_t country;
  /** \brief Herald Location State **/
  uint16_t state;
  /** \brief Herald Location Code (NOT beacon identifier) **/
  uint32_t code;
  /**
   * \brief Herald Location Beacon ID (Beacon identifier)
   * \since v2.2
   **/
  uint16_t id;

  // Note: We only supported limited extension fields
  /**
   * \brief Location further information URL
   * \note 0x13 in Herald Extended data area. MUST be https.
   **/
  char* ext_url;
  /** \brief Length of location URL **/
  uint16_t ext_url_length;  // URL GET is max 2048 thus 11 bytes for length

  // char* ext_name; // 0x10 in Herald Extended data area
  // size_t ext_name_length;

  // char* ext_location; // 0x11 in Herald Extended data area
  // size_t ext_location_length;

  // TODO public security key for beacon later
};

// Note: Message op codes and lengths set in mesh.h

#ifdef __cplusplus
}
#endif

#endif
#endif

#endif /* HERALD_MESH_LOCATION_H */
