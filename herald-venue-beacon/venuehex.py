#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# 
# Creates a valid venue.hex file and merges it with the Zephyr hex file from
# your build. Does not programme a board.

import argparse
from textwrap import wrap

def accumulate(hexstring: str) -> int:
    total = 0
    for pos in range(0,len(hexstring),2):
        partial = hexstring[pos:pos+2]
        # print(f'{partial}')
        if ".0" == partial:
            partial = "00"
        total += int(partial,base=16)
    return total

def checksum(hexstring: str) -> str:
    return '%02X' % (-(accumulate(hexstring) % 256) & 0xFF)

def main():
    # 1. Get command line parameters
    parser = argparse.ArgumentParser(
                    prog='venuehex',
                    description='Creates a Venue Beacon hex file.',
                    epilog='Creates a Herald Venue Beacon configuration hex file and merges it with your zephyr hex file.')
    parser.add_argument("-c","--country",type=int,default=0,help="ISO8266 Country Code for Beacon (uint16)")
    parser.add_argument("-s","--state",type=int,default=0,help="A Country's internal State Code for Beacon (uint16)")
    parser.add_argument("-i","--idcode",type=int,default=0,help="Beacon instance identifier Code allocated (uint32)")
    parser.add_argument("-x","--x",type=int,default=0,help="A Beacon X position in cm (uint16)")
    parser.add_argument("-y","--y",type=int,default=0,help="A Beacon Y position in cm (uint16)")
    parser.add_argument("-z","--z",type=int,default=0,help="A Beacon Z position in cm (uint16)")
    parser.add_argument("-n","--name",default="UnconfiguredHeraldBeacon",help="Beacon instance text name (max: 32 characters. ASCII)")
    parser.add_argument("-b","--base",default="7A00",help="The base address for this device's storage partition (divided by 16. Hex)")
    parser.add_argument("-o","--output",default="venue.hex",help="The output file (default: venue.hex)")
    args = vars(parser.parse_args()) # Exits on error
    # print(args)
    # print("Name:-")
    # print(args["name"])
    # print("Name done")

    # 2. Now generate the beacon hex file
    vhex = open(args["output"],"w")
    # 2a. Address positional header
    hexline1 = "02000002" + args["base"].upper() # TODO Convert the address as in the DTS to this format ourselves
    vhex.write(":" + hexline1 + checksum(hexline1) + "\n")
    # 2b. Configuration data
    namestring = args["name"]
    namestring = ""
    for c in args["name"]:
        namestring += ("%02X" % (ord(c))).zfill(2) # Hex of the ASCII decimal value of each character, ensuring 2 hex digits
    hexline2data = ("%02X" % (args["country"])).zfill(4) + \
                   ("%02X" % (args["state"])).zfill(4) + \
                   ("%02X" % (args["idcode"])).zfill(8) + \
                   ("%02X" % (args["x"])).zfill(4) + \
                   ("%02X" % (args["y"])).zfill(4) + \
                   ("%02X" % (args["z"])).zfill(4) + \
                   namestring + "00" # Nul termination character (\0 in C)
    # print(hexline2data)
    hexline2 = ("%02X" % int(len(hexline2data)/2)).zfill(2) + '000100' + hexline2data
    vhex.write(":" + hexline2 + checksum(hexline2) + "\n")
    # 2c. End of file
    vhex.write(":00000001FF\n")
    vhex.close()
    print(f'Hex written to file: {args["output"]}')

if __name__ == "__main__":
    main()