//  Copyright 2021-2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

// This file holds models common to location services server and client headers

#ifndef HERALD_MESH_LOCATION_SERVICES_H
#define HERALD_MESH_LOCATION_SERVICES_H

#include "mesh.h"
#include <bluetooth/mesh/model_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Herald device and other Bluetooth LE Proximity Server.
 */

/** Bluetooth MESH Herald presence values **/
enum bt_mesh_model_herald_presence_status {
  BT_MESH_HERALD_PRESENCE_OBSERVED,
  BT_MESH_HERALD_PRESENCE_IDENTIFIED,
  BT_MESH_HERALD_PRESENCE_VANISHED
};

/**
 * \brief Herald presence message for a BLE node near a beacon
 */
struct bt_mesh_model_herald_presence {
  /** \brief BLE Mac Address **/
  uint8_t address[6];
  /** \brief The last RSSI observed **/
  int8_t rssi;
  /** \brief The current derived presence status for this observer **/
  enum bt_mesh_model_herald_presence_status status;
};



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
  uint16_t ext_url_length; // URL GET is max 2048 thus 11 bytes for length

  // char* ext_name; // 0x10 in Herald Extended data area
  // size_t ext_name_length;

  // char* ext_location; // 0x11 in Herald Extended data area
  // size_t ext_location_length;

  // TODO public security key for beacon later
};






/** Mandatory parameters for the Herald Location Services Set message. **/
struct bt_mesh_herald_location_services_set {
	/** Whether to advertise this beacon as a Herald Venue Beacon over BLE **/
	bool advertise_beacon;
  /** Whether to publish Bluetooth LE presence information over the MESH **/
  // bool share_presence;

  uint8_t venue_id; // temporary just to have something to set for now

  // TODO add other fields here from Venue Beacon Payload


	/**
	 * Transition time parameters for the state change, or NULL.
	 *
	 * When sending, setting the transition to NULL makes the receiver use
	 * its default transition time parameters, or 0 if no default transition
	 * time is set.
	 **/
	const struct bt_mesh_model_transition *transition;
};

/** Parameters for the Generic OnOff Status message. */
struct bt_mesh_herald_location_services_status {
	/** The present value of the Generic OnOff state. */
	bool present_advertising;
  // bool present_share_presence;
  uint8_t present_venue_id;

	/** The target value of the Generic OnOff state (optional). */
	bool target_advertising;
  // bool target_share_presence;
  uint8_t target_venue_id;

	/** Remaining time value in milliseconds. */
	int32_t remaining_time;
};

// Note: Message op codes and lengths set in mesh.h

#ifdef __cplusplus
}
#endif

#endif /* HERALD_MESH_LOCATION_SERVICES_H */
