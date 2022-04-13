//  Copyright 2021 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

// This file holds models common to location services server and client headers

#ifndef HERALD_MESH_LOCATION_SERVICES_H
#define HERALD_MESH_LOCATION_SERVICES_H

#include "mesh.h"
#include "location_services_cli.h"
#include <bluetooth/mesh/model_types.h>

#ifdef __cplusplus
extern "C" {
#endif


/** Bluetooth MESH Herald presence values **/
enum bt_mesh_herald_location_services_cli_presence {
  BT_MESH_HERALD_PRESENCE_OBSERVED,
  BT_MESH_HERALD_PRESENCE_IDENTIFIED,
  BT_MESH_HERALD_PRESENCE_VANISHED
};

/**
 * \brief Herald presence message for a BLE node near a beacon
 */
struct bt_mesh_herald_location_services_presence {
  /** \brief BLE Mac Address **/
  uint8_t address[6];
  /** \brief The last RSSI observed **/
  int8_t rssi;
  /** \brief The current derived presence status for this observer **/
  bt_mesh_herald_location_services_cli_presence status;
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
