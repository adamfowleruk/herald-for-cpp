# Herald Bluetooth MESH Modem RabbitMQ Adapter Application

This application is a Linux only application that links a
[herald-mesh-modem](../herald-mesh-modem)
attached to the computer/server via USB to a RabbitMQ reliable
message broker control plane.

This allows data to flow between an event driven microservices application 
environment and a Smart Hospitals network built using beacons enrolled in a
[herald-mesh-relay](../herald-mesh-relay)
Bluetooth MESH network.

MESH administrative and mesh model publish/subscribe functionality is supported.

## A note on Bluetooth MESH standards compliance

Where possible we re-use existing Bluetooth SIG standard MESH models. These are
mainly concerned with MESH administration and smart lighting and other building
control use cases. 

For our healthcare use cases in Herald we've created new MESH models but have tried
to do so in a way that they are generally applicable beyond a hospital. The
Location MESH model, for example, supports internal building navigation in
any Bluetooth MESH network, supporting clients that are Bluetooth LE based such
as mobile phones.

We intend to try out new model ideas within this project and then, subject to adoption
as a vendor model by the whole Linux Foundation (Not just Linux Foundation Public
Health, of which the Herald Project is a part), or adoption as a standard model
by the Bluetooth SIG, we'll look to upstream the models in to Zephyr RTOS.

## Features

Added in v2.1 (First release):-
- Test commands (hello, status)
- Default fixed subscriptions to all Presence messages (Herald LE and other LE devices)
- Programming of Mesh relay nodes' Venue Beacon and Location metadata for site navigation