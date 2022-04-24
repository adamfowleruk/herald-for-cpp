//  Copyright 2021-2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_MESH_H
#define HERALD_MESH_H

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Linux Foundation company code **/
#define BT_MESH_LINUX_FOUNDATION_VENDOR_COMPANY_ID    0x05f1

// Note: Vendor Model IDs kept here so this could become a common header file across all Linux Foundation MESH using projects

// Note: Model IDs starting 0x0yyy are used for foundation models with the Bluetooth SIG
// Note: Model IDs starting 0x1yyy are used for registered models with the Bluetooth SIG
/** \note Linux Foundation model IDs. Note: Herald using 0x2yyy for now. LF-wide approach TBD. **/

/// MARK: BEGIN LINUX FOUNDATION COMMON REUSABLE OP CODE SECTION

// TODO Verify that having the same opp in multiple models in the same
//      element is allowed. Some Zephyr documentation indicates it is not.

// TODO refactor the below into all-platform OPCODEs and a separate Zephyr OP definition file

/** \brief Get a server model's whole data **/
#define BT_MESH_LINUX_FOUNDATION_OP_GET BT_MESH_MODEL_OP_3(0x01, \
  BT_MESH_LINUX_FOUNDATION_VENDOR_COMPANY_ID)
/** \brief Set a server model's whole data **/
#define BT_MESH_LINUX_FOUNDATION_OP_SET BT_MESH_MODEL_OP_3(0x02, \
  BT_MESH_LINUX_FOUNDATION_VENDOR_COMPANY_ID)
/** \brief Set a server model's whole data without acknowledgement **/
#define BT_MESH_LINUX_FOUNDATION_OP_SET_UNACK BT_MESH_MODEL_OP_3(0x03, \
  BT_MESH_LINUX_FOUNDATION_VENDOR_COMPANY_ID)
/**
 * \brief Publish the status of a whole server model 
 * \note Also used as the Ack message to the SET command.
 **/
#define BT_MESH_LINUX_FOUNDATION_OP_STATUS BT_MESH_MODEL_OP_3(0x04, \
  BT_MESH_LINUX_FOUNDATION_VENDOR_COMPANY_ID)
// TODO determine if the next line is just a duplicate of the STATUS line
/** Send a message containing the full model **/
#define BT_MESH_LINUX_FOUNDATION_OP_MESSAGE BT_MESH_MODEL_OP_3(0x05, \
  BT_MESH_LINUX_FOUNDATION_VENDOR_COMPANY_ID)

/// MARK: END LINUX FOUNDATION COMMON REUSABLE OP CODE SECTION






/// MARK: BEGIN HERALD PROJECT SECTION

#define BT_MESH_HERALD_MESH_RELAY_PRODUCT_ID 0x0001
#define BT_MESH_HERALD_MESH_MODEM_PRODUCT_ID 0x0002
#define BT_MESH_HERALD_MESH_TAG_PRODUCT_ID   0x0003

// The following convert SEMVER Herald releases to BL MESH Product Versions
// Note: Beta1, 2 etc use the NEXT minor version number
#define BT_MESH_HERALD_PRODUCT_V121          0x0001
#define BT_MESH_HERALD_PRODUCT_CURRENT_VERSION BT_MESH_HERALD_PRODUCT_V121

/**
 * @brief A Herald MESH Relay Beacon can also advertise location
 * information to nearby Herald Venue Beacon Bluetooth LE clients.
 * This includes ownership, section, area information.
 * This is useful to maintain your own 'venue diary', which you
 * may later choose to share. (E.g. with a disease contact tracing team)
 * \note Allows the Herald Beacon to be programmed via MESH.
 **/

#define BT_MESH_HERALD_VENUE_BEACON_VENDOR_MODEL_ID 0x2001

/** \brief Beacon message length. Country (2), State(2), Venue (4), BeaconID
 * (2), url(0-2048) **/
#define BT_MESH_HERALD_VENUE_BEACON_MSG_MINLEN_SET 10
#define BT_MESH_HERALD_VENUE_BEACON_MSG_MAXLEN_SET 2058
// TODO verify that MESH itself and Zephyr and the nRF Mesh app don't crash with such a potentially big message
// TODO consider \x16 (https) as the first character in the above

// Note we re-use the generic get,set,status op codes from the Linux Foundation








/**
 * @brief A Herald MESH Relay beacon can publish presence information of
 * nearby BLE devices as messages to the mesh. This helps collate presence
 * and density information (E.g. for COVID gathering safety monitoring).
 */

/** \brief The model ID for Herald presence server information **/
#define BT_MESH_HERALD_PRESENCE_SERVER_VENDOR_MODEL_ID 0x2002

// Note we re-use the generic get,set,status op codes from the Linux Foundation
// Now set the minimum/maximum lengths for various operations
// Fetch status length
#define BT_MESH_HERALD_PRESENCE_MSG_LEN_GET 0


// Set status message min/max length
#define BT_MESH_HERALD_PRESENCE_MSG_MINLEN_SET 3
#define BT_MESH_HERALD_PRESENCE_MSG_MAXLEN_SET 5
// Get status (model) response / published message length
/** 
 * \brief Presence message length - 6 byte Mac address, 1 int8_t Last RSSI, 1
 * uint8_t presence flag. Does not include mesh header sizes.
 **/
#define BT_MESH_HERALD_PRESENCE_MSG_MINLEN_STATUS 2 // just mesh overhead
#define BT_MESH_HERALD_PRESENCE_MSG_MAXLEN_STATUS 11 // 8 bytes message + operation overhead (2) + tail (1)

/// MARK: END HERALD PROJECT SECTION

#ifdef __cplusplus
}
#endif

#endif
