//  Copyright 2021 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#include "herald/ble/ble_sensor_configuration.h"

namespace herald {
namespace ble {

BLESensorConfiguration::BLESensorConfiguration()
  : serviceUUID(UUID::fromString("0000FCF6-0000-1000-8000-00805F9B34FB")),
    androidSignalCharacteristicUUID(UUID::fromString("f617b813-092e-437a-8324-e09a80821a11")),
    iosSignalCharacteristicUUID(UUID::fromString("0eb0d5f2-eae4-4a9a-8af3-a4adb02d4363")),
    payloadCharacteristicUUID(UUID::fromString("3e98c0f8-8f05-4829-a121-43e38f8933e7")),
    heraldProtocolV2CharacteristicUUID(UUID::fromString("136d8aa2-7072-437a-85fc-6104ed675bc0")),
    manufacturerIdForSensor(1521), // aka 0x05F1
    manufacturerIdForApple(76),
    signalCharacteristicActionWritePayload(std::byte(1)),
    signalCharacteristicActionWriteRSSI(std::byte(2)),
    signalCharacteristicActionWritePayloadSharing(std::byte(3)),
    signalCharacteristicActionWriteImmediate(std::byte(4)),
    payloadDataUpdateTimeInterval(TimeInterval::never()),
    payloadSharingExpiryTimeInterval(TimeInterval::minutes(5)),
    advertRefreshTimeInterval(TimeInterval::minutes(15)),
    peripheralCleanInterval(TimeInterval::minutes(2)),
    maxBluetoothConnections(20),
    advertisingEnabled(true),
    scanningEnabled(true),
#ifdef CONFIG_HERALD_PROTOCOL_V2    
    heraldProtocolV2Enabled(true),
#else
    heraldProtocolV2Enabled(false),
#endif
#ifdef CONFIG_HERALD_MESH
    heraldMeshEnabled(true)
#else
    heraldMeshEnabled(false)
#endif
{
  ;
}

BLESensorConfiguration::BLESensorConfiguration(const BLESensorConfiguration& other)
  : serviceUUID(other.serviceUUID),
    androidSignalCharacteristicUUID(other.androidSignalCharacteristicUUID),
    iosSignalCharacteristicUUID(other.iosSignalCharacteristicUUID),
    payloadCharacteristicUUID(other.payloadCharacteristicUUID),
    heraldProtocolV2CharacteristicUUID(other.heraldProtocolV2CharacteristicUUID),
    manufacturerIdForSensor(other.manufacturerIdForSensor),
    manufacturerIdForApple(other.manufacturerIdForApple),
    signalCharacteristicActionWritePayload(other.signalCharacteristicActionWritePayload),
    signalCharacteristicActionWriteRSSI(other.signalCharacteristicActionWriteRSSI),
    signalCharacteristicActionWritePayloadSharing(other.signalCharacteristicActionWritePayloadSharing),
    signalCharacteristicActionWriteImmediate(other.signalCharacteristicActionWriteImmediate),
    payloadDataUpdateTimeInterval(other.payloadDataUpdateTimeInterval),
    payloadSharingExpiryTimeInterval(other.payloadSharingExpiryTimeInterval),
    advertRefreshTimeInterval(other.advertRefreshTimeInterval),
    peripheralCleanInterval(other.peripheralCleanInterval),
    maxBluetoothConnections(other.maxBluetoothConnections),
    advertisingEnabled(other.advertisingEnabled),
    scanningEnabled(other.scanningEnabled),
    heraldProtocolV2Enabled(other.heraldProtocolV2Enabled),
    heraldMeshEnabled(other.heraldMeshEnabled)
{
  ;
}

}
}