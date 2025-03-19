# Herald Mesh Relay

This is an app that provides a MESH node and all associated functions.

Makes use of the Herald Protocol V2 write characteristic.

This sample has been available since 01 September 2024.

## Programming mesh relay identity

See the herald-venue-beacon sample's README.md file to see how to programme a board's identity prior to programming.

## Limitations

Currently encryption on the characteristic used is not implemented, and the mesh protocol
is limited to flood functionality. Flood retransmit protection is implemented.

## Disabled functions

All USB access is disabled, allowing for secure standalone operation. (USB Power will still work)