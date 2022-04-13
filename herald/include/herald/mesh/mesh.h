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

/** \brief Get a model's whole data **/
#define BT_MESH_LINUX_FOUNDATION_OP_GET BT_MESH_MODEL_OP_3(0x01, \
  BT_MESH_LINUX_FOUNDATION_VENDOR_COMPANY_ID)
/** \brief Set a model's whole data **/
#define BT_MESH_LINUX_FOUNDATION_OP_SET BT_MESH_MODEL_OP_3(0x02, \
  BT_MESH_LINUX_FOUNDATION_VENDOR_COMPANY_ID)
/** \brief Set a model's whole data without acknowledgement **/
#define BT_MESH_LINUX_FOUNDATION_OP_SET_UNACK BT_MESH_MODEL_OP_3(0x03, \
  BT_MESH_LINUX_FOUNDATION_VENDOR_COMPANY_ID)
/** \brief Get the status of a whole model Set operation **/
#define BT_MESH_LINUX_FOUNDATION_OP_STATUS BT_MESH_MODEL_OP_3(0x04, \
  BT_MESH_LINUX_FOUNDATION_VENDOR_COMPANY_ID)
/** Send a message containing the full model **/
#define BT_MESH_LINUX_FOUNDATION_OP_MESSAGE BT_MESH_MODEL_OP_3(0x05, \
  BT_MESH_LINUX_FOUNDATION_VENDOR_COMPANY_ID)

/// MARK: END LINUX FOUNDATION COMMON REUSABLE OP CODE SECTION






/// MARK: BEGIN HERALD PROJECT SECTION

/**
 * @brief A Herald MESH Relay beacon can publish presence information of 
 * nearby BLE devices as messages to the mesh. This helps collate presence
 * and density information (E.g. for COVID gathering safety monitoring).
 */

#define BT_MESH_HERALD_LOCATION_SERVICES_CLI_VENDOR_MODEL_ID 0x2001

/** \brief Presence message length - 6 byte Mac address, 1 int8_t Last RSSI, 1 uint8_t presence flag. Does not include mesh header sizes **/
#define BT_MESH_HERALD_LOCATION_SERVICES_PRESENCE_MSG_LEN 8

// Note we re-use the generic get,set,status op codes from the Linux Foundation

/**
 * @brief A Herald MESH Relay Beacon can also advertise location
 * information to nearby Herald Venue Beacon Bluetooth LE clients.
 * This includes ownership, section, area information.
 * This is useful to maintain your own 'venue diary', which you
 * may later choose to share. (E.g. with a disease contact tracing team)
 */

/** Location Services / Herald Venue Beacon interlink **/
#define BT_MESH_HERALD_LOCATION_SERVICES_SVR_VENDOR_MODEL_ID 0x2002

// Note we re-use the generic get,set,status op codes from the Linux Foundation
// Now set the minimum/maximum lengths for various operations
// Fetch status length
#define BT_MESH_HERALD_LOCATION_SERVICES_MSG_LEN_GET 0
// Set status message min/max length
#define BT_MESH_HERALD_LOCATION_SERVICES_MSG_MINLEN_SET 3
#define BT_MESH_HERALD_LOCATION_SERVICES_MSG_MAXLEN_SET 5
// Get status (model) response / published message length
#define BT_MESH_HERALD_LOCATION_SERVICES_MSG_MINLEN_STATUS 2 // just mesh overhead
#define BT_MESH_HERALD_LOCATION_SERVICES_MSG_MAXLEN_STATUS 11 // 8 bytes message + operation overhead (2) + tail (1)

/// MARK: END HERALD PROJECT SECTION

#ifdef __cplusplus
}
#endif

#endif
