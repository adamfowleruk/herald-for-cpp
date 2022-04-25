# Herald MESH Modem

This production ready application acts, once provisioned, as an
interlink between a computer and a Bluetooth MESH Network.

This is the Zephyr USB dongle side of the Modem. The other side
can be provided by a number of applications. The one we provide
is the [herald-mesh-rmq-adapter](../herald-mesh-rmq-adapter)
which enables a RabbitMQ reliable message broker to configure
and subscribe to a Herald Bluetooth MESH Network.

Note: Whilst the Herald MESH Modem provide Herald specific
high level commands, the low-level commands can be used with
any Bluetooth SIG MESH compliant MESH network.

## What is provided today

- Exposes the device as a USB CDC ACM device driver
- Negotiates tty settings successfully (115kbps)
- Basic command parser network

## TODOs

- Support protobufv3 for messaging format
- Receive an enable as relay message
- Receive a presence subscribe message (Chat group from nRF MESH app) and forward messages onto modem computer
- Receive a heartbeat subscribe message and send these updates to modem computer

## Later

- Allow setting of TTL for mesh nodes
- Allow MESH node attempted discovery (low and high addresses)
- Allow changing of application keys for remote element-model combinations from a command