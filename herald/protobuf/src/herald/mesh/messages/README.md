# Commands for Bluetooth MESH and ProtoBuf

We use ProtoBuf messages to communicate to the mesh modem app from a USB controller device.
We use similar messages over the wire on RabbitMQ to pass around commands and information
to and from the controller host.

We use protobuf as a language independent layer to define these messages. The primary
source of these is the herald-for-cpp project and the proto files can be found under
/herald/protobuf/mesh/*.proto. This is also where the C++ files are generated and
vendored (stored).