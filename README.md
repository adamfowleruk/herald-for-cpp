# Herald for C++ (Native platforms) - Library

This repository contains a Herald Bluetooth Protocol and Payload
set of implementations for native platforms. This includes
Windows 10 desktop and Nordic Semiconductor or other Zephyr RTOS
capable boards. The boards used for development are Nordic Semiconductor
nRF52 and 53 development kits and derived dongles.

This particular code base is the core Herald library. It is
used by all examples, including Wearables (just like the Herald for
iOS and Herald for Android library use case), Bluetooth MESH 
Gateway, and Windows Desktop Herald communication examples.

This implementation was introduced in Herald v1.2.

## License and Copyright

Copyright 2020-2022 Herald Project Contributors

[![License: Apache-2.0](https://img.shields.io/badge/License-Apache2.0-yellow.svg)](https://opensource.org/licenses/Apache-2.0)

See LICENSE.txt and NOTICE.txt for details.

## Ready to use applications

The following applications are downloadable and ready to use on 
[Herald Hardware](https://github.com/theheraldproject/herald-hardware)
and other compatible devices.

Embedded Smart Beacon applications (Zephyr RTOS based):-
- [Herald Bluetooth MESH Gateway](herald-mesh-relay) - Create a Smart Building, connect devices, and build open source Smart Hospitals and Internet of Things applications
- [Herald Bluetooth MESH Modem](herald-mesh-modem) - Connect a Herald Bluetooth MESH network to your servers, connecting your organisations applications to the Herald Bluetooth MESH network

Server applications (Linux based):-
- [Herald Bluetooth MESH RabbitMQ Adapter](herald-mesh-rmq-adapter) - Control a Herald Bluetooth MESH Modem remotely by connecting it to a reliable message broker

## The Herald API for Embedded devices and C++ apps

The following are libraries available for your own projects:-

- [herald](/herald) - Core herald API (include as an external library or call herald.cmake directly to statically compile in to your app). Works on all platforms, but currently targets Windows, Linux and Zephyr for the main API, and Zephyr for the NFC & Bluetooth Low Energy and Bluetooth MESH transport layers. 
  - Adding a new platform just requires creating 4 classes for the new platform: Context, ConcreteBluetoothReceiver, ConcreteBluetoothTransmitter, SHA256
- [herald mesh](/herald/include/herald/mesh) provides higher level Zephyr MESH API for creating Smart Hospitals style Internet of Things environments
  - A Linux MESH Modem tty interaction adapter layer class is also provided called `herald::mesh::MeshModem`.

## Demonstration apps / libraries

The following non-production sample and demo apps are also available:-

- herald-tests - Herald core C++ API tests
- herald-venue-beacon - Herald Zephyr RTOS based app for Venue beacons as a replacement/supplement for QR code scanning when visiting business, bars, and restaurants. [See the separate README](./herald-venue-beacon/README.md)
- herald-wearable - Herald Zephyr RTOS based app for wearable devices. The equivalent of the herald-for-ios and herald-for-android demo apps for phones to enable Digital Contact Tracing

## Supported / tested platforms

The Herald team use the C++ API for specific use cases and so only provide support for a specific
subset of use cases. If you'd like to contribute platform support feel free to provide code, tests, documentation and raise a PR.

- Windows 64-bit with CLang 10.0+ for VS community edition 2017
  - We don't currently support the vsc++ compiler
- Linux 64-bit (We use Manjaro) 
- Zephyr RTOS / arm with arm-none-eabi-gcc 8.3+ for nRF52840, nRF52832
  - We specifically test using the [Nordic nRF Connect SDK Zephyr variant](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/zephyr.html) [External]

## Platforms supported in other Herald projects

We have similar API and apps for other platforms. Below are some useful references:-

- Other [Zephyr supported boards](https://docs.zephyrproject.org/latest/boards/index.html) [External] where these applications may work with some configuration changes
- We also have a higher level [Mobile app flutter API](https://github.com/theheraldproject/herald-for-flutter/) which includes UI components and supports multiple mobile devices. We also have native lower-level mobile device support:-
  - Android devices see the separate [herald-for-android](https://github.com/theheraldproject/herald-for-android/) project
  - iOS devices see the separate [herald-for-ios](https://github.com/theheraldproject/herald-for-ios) project

## Herald API implementation differences

Some thin wrappers were not required in the C++ version compared
to the Java and Swift versions as the base primitives were already
accesible in C++17. These include:-

- uint64 (std::uint64)
- uint32 (std::uint32)
- uint16 (std::uint16)
- uint8 (std::uint8)
- float16 (std::float)
- strings (std::string)
- tuple<A,B> (std::tuple<A,B>)
- triple<A,B,C> (std::tuple<A,B,C>)
- callback (unused - syncrhonous only, threading abstracted outside of Herald. See Content class for details.)
- BLETimer -> Not implemented. Android specific (Android has a bug in its Timer implementation that makes they awake unreliably)
- Context -> Specific to an application's Context on Android, implemented here in case some platforms have similar requirements. E.g. the ZephyrContext derived class as Zephyr OS has this requirement around Bluetooth state handling.

We've also added some classes to make porting easier. We may fall back in their implementation
C++ files on some platforms where known utility libraries are always present. Current list includes:-

- herald::datatype::uuid - high level UUID interface with just the functionality required by Herald and no more.
- herald::datatype::base64string - encodes and decodes a Data instance as a Base64 string
- herald::datatype::sha256 - SHA256 functionality, currently supported on Windows (OpenSSL-3), Zephyr (TineCrypt or MbedTLS) and Linux (OpenSSL-3)

## Implementation details

Any trivial wrapper classes have been implemented 
as structs.

Any Interfaces from Java and Swift have not been implemented, with their
derived classes being instead defined as templates. The code base has been heavily
refactored in V2.0+ to use references only and avoid using any pointers,
including smart pointers. This allows us to be able to predict and
restrict memory use at compile time, and provide for maximum memory
safety.

This code base implements Bluetooth Low Energy (BLe)
implementations on Zephyr/nRF Connect as standard. We also support USB (MESH modem),
Bluetooth MESH (Smart Hospitals), and NFC (secure out of band provisioning of MESH devices).

## What isn't implemented

User interface code - only embedded applications and the Herald libraries are in
this project repository. We anticipate the 'user interface' to be provided by
mobile applications paired with wearables or beacons via the 
[Herald Flutter API](https://github.com/theheraldproject/herald-for-flutter)
or via cloud native web applications using the `herald-mesh-rmq-adapter` to
interact with a Herald Bluetooth MESH network.

## What is implemented

This repository is the primary development reference API for all current and future
Herald project software work. New API additions are implemented here first. This
is because embedded devices are often much more constrained, and so ensuring
functionality first works here helps ensure a simple interface and higher
performance on other platforms.

## Specific platform limitations

The Herald API cannot use all available modern C++ features due to
some hardware, OS' and libc++ libraries not supporting those functions.
Where possible we always use modern C++17 techniques and STL functions.
On specific platforms we also check for local utility libraries rather
than respecify our own (E.g. base64, random number generation, uuid).

### Zephyr OS limitations

Note: In the v2.0 release we have removed all internal use of std::unique_ptr and std::shared_ptr in favour of templates and static sizes for things like SensorDelegates throughout the code base. This is to avoid Zephyr memory management bugs and improve (i.e. to drastically lower) memory use (SRAM) on Nordic Semiconductor and other embedded devices.
 
We also use noexcept rather than throw exceptions for the same reason.
This is particularly useful for embedded development, but also provides
a robust C++ API for other platforms. 

Note that Zephyr doesn't support RTTI in C++. See the [Zephyr C++ limitations](https://docs.zephyrproject.org/latest/reference/kernel/other/cxx_support.html) [External] page for full C++ compatibility details.
Note that this page is out of date somewhat. The 'new' keyword, for example, is supported in Zephyr although it is very buggy.

### Mac OS limitations

Mac OS is not a primary development platform today and so some features do not work there. These are:-

- No builds of the herald-tests project will succeed due to the gcov code coverage library not being present on MAC OS
- The herald-mesh-rmq-adapter app only supports Linux (its only TCP&TLS implementation)

### Windows limitations

For some reason the same Zephyr app build ends up with lower SRAM usage when built on Linux rather than Windows. Other than that everything else works fine. Windows 10 and Linux Manjaro are the primary development platforms.

These features do not work on Windows:-
- The herald-mesh-rmq-adapter app only supports Linux (its only TCP&TLS implementation)

## Building with Code Coverage

1. Open Visual Studio Code
1. Perform a CMake build using CLang on Windows in Debug mode
1. Execute the following in the `build` folder on the command line: ```cmake .. -DCMAKE_BUILD_TYPE=Debug -DCODE_COVERAGE=ON``` to add code coverage support
1. Open the CMake tools tab in Visual Studio Code
1. Expand 'herald-tests'
1. Run the 'ccov-report' utility