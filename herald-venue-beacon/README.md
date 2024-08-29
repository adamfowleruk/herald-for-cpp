# Herald Venue Beacon Quick Start guide

The Venue Beacon sample showcases how to write a Beacon 
for the nRF52840, nRF52832, and potentially other Zephyr
RTOS based embedded platforms.

This Beacon uses the Herald Beacon Payload specification
version 1.

**NOTE**: The Herald Project does not provide support for Zephyr RTOS itself - 
we only test on the nRF Connect SDK boards we have access to. Please
replicate any issues with those boards before reporting bugs with 
herald-for-cpp. It is likely the issue is with your Zephyr
config settings file or the specific version of Zephyr you are running.
There is a list of [Zephyr supported board](https://docs.zephyrproject.org/latest/boards/index.html)
 [External] that you can check, but your mileage may vary.

## Hardware requirements

Must have a Bluetooth 4.0 and above chip supported by Zephyr RTOS.
Must also have zephyr libc++ support. (highly likely)

## Setting up your environment

Se sure you've followed the following guides before starting. Note that we've recently (June 2023) moved from using the Nordic SDK that included Zephyr to instead use Zephyr directly as that process is now easier and more reliable. Do the following to set up your environment (one time):-

1. Follow the [Zephyr Getting Started Guide]() to ensure you have a working Zephyr system
  - Note: After the initial `west update` command you can `cd zephyr; git checkout v3.4.0; west update; west zephyr-export` to switch versions
1. Set up your board for programming - instructions vary
  - Instructions for [Maker Diary nRF52840-usb-dongle using nRF Connect (RECOMMENDED)](https://wiki.makerdiary.com/nrf52840-mdk-usb-dongle/programming/) [External]
  - Instructions for [Maker Diary nRF52832-usb-dongle using DAPLINK (RECOMMENDED)](https://wiki.makerdiary.com/nrf52832-mdk/nrf5-sdk/) [External]

## Set up an IDE

You may have a preferred IDE - we all do! If you use Visual 
Studio Code (free and open source) then you will benefit
from the settings and configurations already saved within
this project's Git repository, making your life much easier.

You will need the following extensions:-

- C/C++ Extension
- CMake Extension

You DO NOT need to use the Nordic Connect SDK plugin.

You can now open the herald-for-cpp/herald-venue-beacon folder
to load this specific CMake configuration. Don't open the
parent herald-for-cpp folder and expect the Zephyr RTOS/Beacon
sample to run - it won't - you need to open the herald-venue-beacon
folder only. See the next section for why.

## Compiling this application

Zephyr RTOS applications differ significantly in how they use
CMake for building an application. This is because an application
is not just your main.cpp file - rather it's an entire operating
system built in to a single binary with your app.

The Zephyr CMake extension automates much of this process but there
are a few rough edges.

For example, following the recommendation of using Herald as an
'external project' causes cmake on windows to not be given the
Zephyr environment information. Equally, when using CMake
subdirectories instead, the Zephyr includes, libraries, and compiler
settings cannot be linked to Herald.

Instead of this we have implemented the below process which is
much simpler for adopters:-

1. Import a herald.cmake settings file to get Herald source and header
variables
1. Import any C++17 libraries you need in addition to your source code
1. Add some extra lines to your app's sources
1. Now simply compile your app as normal!

You will need to specify the board you are using. We default to the nrf52840dk-nrf52840 if none is specified.

You then need to initialise a build once, and then execute the CMake build as many times as required.

Below are all these steps in succession:-

```sh
# We assume you have already set ZEPHYR_BASE
cd herald-for-cpp/herald-venue-beacon
export BOARD=nrf52dk_nrf52832
cmake -B ./build -DCMAKE_CXX_STANDARD=c++17 -DCMAKE_CXX_STANDARD_REQUIRED=ON
# You now have build files generated. If you switch boards, you need to delete the `build` folder and rerun the above
cmake --build ./build --config Debug --target all -j
```

Assume this works you now have a built app binary in `./build/zephyr/zephyr.hex`.

## Programming your board

You can now programme your app
to a connected board via a JLink interface (including via a Nordic Dev Kit board instead) by doing the following:-

```sh
west flash
```

Note: If the board you are flashing is secured/protected, you will need to run `west flash --recover` first before flashing the app.

If your board is connected to the reset pin properly then the app will now be running on your device and detectable over Bluetooth
via the Nordic Connect mobile app or the Herald Demo App (for iOS or Android). 
Its name will be 'Herald Venue Beacon' and so is easily discovered.

## Pre-programme venue details

You can create some pre-programmed data to be added to the device which configures the beacon name and ID.
This works by setting data in the 'storage' flash partition of the device.

You do this by creating an Intel format data hex file. For an example, see test-flash-storage.hex.
Note that this file is an example for the nrf52dk_nrf52832 board with its default flash memory layout.

This files content with explanation is below. Note: I've added spaces for ease of explanation:-

```txt
:02 0000 02 7A00 82
# The above specified an initial memory address, divided by 16. The above is 0007a000 - the start of the nrf52832 storage partition.
:17 0001 00 0001 0002 00000004 03E8 01F4 0078 544553545445535400 09
The above writes 17 data bytes - country uint16_t, state uint16_t, beacon ID code uint32_t, X-Y-Z position in cm (each uint16_t - x=10m, y=5m, z=1.2m), then text (TESTTEST) ended with the termination character (00 or \0 in C).
:00 0000 01 FF
# The above says that this is the end of the hex file
```

Note that the final number on each line is the two's complement of the modulo 256 of the entire line after the : character.
A good modulo calculator is here (Use the `CheckSum8 2s Complement` value): https://www.scadacore.com/tools/programming-calculators/online-checksum-calculator/

Note: See your DTS file for your board for the named `storage_partition` base address. E.g. for the nrf52dk_nrf52832: https://github.com/zephyrproject-rtos/zephyr/blob/db1a718341a3724b82c052e6e6b7db19251f7a22/boards/arm/nrf52dk_nrf52832/nrf52dk_nrf52832.dts#L222

### Generating position hex file

Use the `venuehex.py` file in this folder to generate the metadata hex file(s) you need.

Here is an example of programming 4 boards in a 2x2 beacon grid square with 2m sides with beacons at shoulder height (1.5m).

```sh
python venuehex.py -c 1 -s 1 -i 5 -x 0 -y 0 -z 150 -n B05 -o venue-b05-0-0-150.hex  
python venuehex.py -c 1 -s 1 -i 6 -x 200 -y 0 -z 150 -n B06 -o venue-b06-200-0-150.hex  
python venuehex.py -c 1 -s 1 -i 7 -x 200 -y 200 -z 150 -n B07 -o venue-b07-200-200-150.hex
python venuehex.py -c 1 -s 1 -i 8 -x 0 -y 200 -z 150 -n B08 -o venue-b08-0-200-150.hex   
```

Note: The instance (-i) flag must change for each device so the Herald demo app correctly identifies the beacons as unique, separate devices.

Note: The base memory address (-b) flag defaults to the base address for Nordic's own nRF52840 dongle (0x7A000 - or `-b 7A00`). For the Maker Diary nRF52840 USB dongle this should be specified as `-b CC00` (I.e. 0xCC000) as per the Zephyr.dts file in the build folder (search for storage_partition in here to find the value needed for your board). Note that the trailing `0` is removed when using the `-b` flag.

### Merging hex files

To merge your built Herald venue beacon programme with this configuration file, execute:-

```sh
mergehex -m build/zephyr/zephyr.hex venue-b05-200-0-0.hex   -o combined-b05.hex
```

Then be sure to flash `combined-b05.hex` to your device and NOT the zephyr.hex file as usual.

When you reboot your device you will see the correct data exposed in the Herald Venue Beacon in its Herald Payload area.

Note: If using the UF2 bootloader, read the section at the end of this readme to conver the hex file to a UF2 bootloader image.


## Ancillary instructions / extensions

You are now done, but if you need further information then the below may be useful.

### Recommended code layout

We highly recommend you link in the herald-for-cpp folder as a git submodule underneath your app's git tree.
This will enable you to easily switch between release and develop branches of code, latest fixes, and
be able to test any PRs you wish to make to the upstream herald-for-cpp project before submitting them.

Don't forget to regularly update the submodule folder. Also note that Herald has the 'fmt' library
as an existing submodule, so ensure your submodule depth for updates is at least 2.

### nRF52840 USB Dongle board

We use the nRF Connect Desktop programmer app.

1. Be sure to flash [openbootloader with DFU support](https://github.com/makerdiary/nrf52840-mdk-usb-dongle/tree/master/firmware/open_bootloader#change-to-open-bootloader-from-uf2-bootloader) [External] to your board - the UF2 bootloader doesn't support nRF Connect's programmer
1. Launch nRF Connect Desktop
1. Select and install, then open the Programmer app
1. Whilst holding down the reset button, plug in the dongle - this enters programming mode (note: you can plug it in and hit reset - prevents wear and tear of the usb contacts)
1. Now select the device from the Devices drop down
1. Drag and drop the herald-venue-beacon/build/zephyr/zephyr.hex file to the target device (removing any existing app that's installed, but leaving the bootloader alone)
1. Click the 'write' button

This will program the board, and automatically reset it.

### nRF52832 USB dongle board

This board supports DAPLINK, so you can simply drag/drop the hex file over in windows explorer (or similar).

1. Connect the board (no need to depress reset whilst doing so)
1. You will see a drive appear called 'DAPLINK'
1. Drag and drop the herald-venue-beacon/build/zephyr/zephyr.hex file to this drive (no need to delete other files)

Once this is done the board will program itself, disconnect, and reconnect.

**NOTE**: Be sure to hit the 'reset' button to launch the newly programmed app - it doesn't start automatically.

**NOTE**: Also the DAPLINK drive will automatically reconnect. This doesn't mean the beacon app isn't running.

### Maker Diary nRF52832 USB Dongle board

If using this board and the UF2 bootloader, you need to follow these programming instructions to convert the hex to a UF2. You can then simply drag/drop the file to programme the board:-

https://wiki.makerdiary.com/nrf52840-mdk-usb-dongle/programming/uf2boot/#flash-memory-layout

Once initially programmed - DOUBLE click the reset button to enter UF2 mode.

To install UF2 tools:-

```sh
cd c:\\ncs\\v2.6.1
pip3 install -r zephyr/scripts/requirements.txt     
pip3 install -r nrf/scripts/requirements.txt    
pip3 install -r bootloader/mcuboot/scripts/requirements.txt
py -3 -m pip install --pre -U git+https://github.com/makerdiary/uf2utils.git@main
```

To create a UF2 image:-

```sh
uf2conv -f 0xADA52840 -c -o app-b08.uf2 .\combined-b08.hex
```

Now drag and drop the image to the device.

Note: You may need to hit the reset button TWICE on the device to enter bootloader mode.

The device will reboot and start the application automatically.