# -*- coding: utf-8 -*-
"""
Spyder Editor

Demonstration of what the physical and coding layer should do
Also allowed generating "correct" input for
usage with the arduino.

Data is stored as a 1-D list
"""
from math import log, floor,ceil
number = 0b10000000_01000000_00100000_00010000_00001000_00000100_00000010_00000000
# Pre-defined strings.
acq_sequence = 0x55555555_55555555_55555555_55555555
start_sequence = 0b1110_1011_1001_0000
idle_sequence = 0b0101_0101
tail_sequence = 0b01_0101_0101

# Make python actually print the numbers as binary.
# And pad with zeros
acq_sequence_string = "0" + "{0:b}".format(acq_sequence)
start_sequence_string = "{0:b}".format(start_sequence)
idle_sequence_string = "0" + "{0:b}".format(idle_sequence)
tail_sequence_string = "0" + "{0:b}".format(tail_sequence)

def crcold(value, polynomial):
    '''Calculate CRC checksum'''
    # Firstly, we figure out how big the number still is.
    msb_poly = floor(log(polynomial)/log(2))
    msb_value = floor(log(value)/log(2))
    n_shift = msb_value - msb_poly
    # Subtract the generator polynomial
    value = value ^ (polynomial << n_shift)
    # If only remainder left, return remainder
    # Else call function recursively
    if 2**msb_poly > value:
        return value
    print(f"{value:b}")
    return crc(value, polynomial)

def crc(value, polynomial):
    value = f"{value:b}"
    valuelist = [int(digit) for digit in value]
    
    if polynomial == 0b11000101:
        print((value))
        while(len(valuelist) < 63):
            valuelist = [0] + valuelist
        for i in range(63 - 8):
            if valuelist[i] == 1:
                valuelist[i] = valuelist[i] ^ 1
                valuelist[i+1] = valuelist[i+1] ^ 1
                valuelist[i+5] = valuelist[i+5] ^ 1
                valuelist[i+7] = valuelist[i+7] ^ 1
            #print(valuelist)
        res = 0
        for index, digit in enumerate(reversed(valuelist[-7:])):
            res = res + (digit << index)
    elif polynomial == 0b1_0001_0000_0010_0001:
        while(len(valuelist) < 168):
            valuelist.append(0)
        print(valuelist)
        for i in range(152):
            if valuelist[i] == 1:
                valuelist[i] = valuelist[i] ^ 1
                valuelist[i+4] = valuelist[i+4] ^ 1
                valuelist[i+11] = valuelist[i+11] ^ 1
                valuelist[i+16] = valuelist[i+16] ^ 1
        print(valuelist)
        res = 0
        for index, digit in enumerate(reversed(valuelist[-16:])):
            res = res + (digit << index)
    else:
        raise ValueError
    #res = 0
    #for index, digit in enumerate(reversed(valuelist[-7:])):
    #    res = res + (digit << index)
    return res

def make_frame(frame_data):
    '''add checksum to frame data'''
    while len("{0:b}".format(frame_data)) < 168:
        frame_data = frame_data << 1
    
    checksum = crc(frame_data, 0b1_0001_0000_0010_0001)
    print("Checksum:")
    print("{0:b}".format(checksum))
    #checksum = 0b1111_1111_1111_1111 ^ checksum
    frame = frame_data + checksum
    print(crc((frame_data + checksum), 0b1_0001_0000_0010_0001))
    print()
    return frame

def frame_to_blocks(frame):
    frameBlocks = []
    frameLength = len(f"{frame:b}")
    nBlocks = ceil(frameLength / 56)
    blockFill = 56 * nBlocks - frameLength
    frame = frame << blockFill
    while (nBlocks > 0):
        thisBlock = (frame >> (56 * (nBlocks - 1))) % 2**56
        frameBlocks.append(thisBlock)
        nBlocks = nBlocks - 1
    return frameBlocks
    '''Cut up the frame into 7 octet blocks
    frameblocks = []
    while frame > 0:
        block_fill = 56 - len(f"{frame:b}")
        if block_fill > 0:
            # Add fill zeros to pad block to 56 bits
            frame = frame << block_fill
        frameblocks.append(frame % 2**56)
        frame = frame >> 56
    return frameblocks
'''
    
def make_codeblock(frame_block):
    '''Add checksum to 7 block'''
    # Add room for parity bits
    frame_block = frame_block << 7
    # Compute checksum
    checksum = crc(frame_block, 0b11000101)
    # Complement parity bits and add padding 0
    #print(checksum)
    #checksum = (checksum ^ 0b1111111)
    print(checksum)
    # Add to data
    codeblock = "{0:b}".format((frame_block + checksum) << 1)
    print(crc((frame_block + checksum), 0b11000101))
    print()
    while(len(codeblock) < 64):
        codeblock = "0" + codeblock
    return codeblock

def make_CLTU(codeblocks):
    '''Stack codeblocks into CLTU'''
    codeblock_string = ""
    for codeblock in codeblocks:
        codeblock_string = f"{codeblock_string}{codeblock}"
    CLTU = start_sequence_string + codeblock_string + tail_sequence_string
    return CLTU
    
def make_transmission(CLTU):
    transmission = acq_sequence_string + CLTU + idle_sequence_string + idle_sequence_string
    return transmission

# DATA FOR FRAME HEADER ##
data = 0
data += 1 << (8 * 8)
data += 2 << (7 * 8)
data += 3 << (6 * 8)
data += 4 << (5 * 8)
data += 5 << (4 * 8)
data += 6 << (3 * 8)
data += 7 << (2 * 8)
data += 8 << (1 * 8)
data += 9

FRAME_LENGTH = 72#floor(log(data)/log(2))

VERSION_NUMBER = 0b11
BYPASS_FLAG = 0b0
CONTROL_COMMAND_FLAG = 0b0
RESERVED_FIELD_A = 0b0
SPACECRAFT_ID = 0b1100110011
VIRTUAL_CHANNEL_ID = 0b001100
RESERVED_FIELD_B = 0b00
FRAME_SEQUENCE_NUMBER = 0b0000_0001

header = VERSION_NUMBER
header = (header << 1) + BYPASS_FLAG
header = (header << 1) + CONTROL_COMMAND_FLAG
header = (header << 2) + RESERVED_FIELD_A
header = (header << 10) + SPACECRAFT_ID
header = (header << 6) + VIRTUAL_CHANNEL_ID
header = (header << 2) + RESERVED_FIELD_B
header = (header << 8) + FRAME_LENGTH
header = (header << 8) + FRAME_SEQUENCE_NUMBER

frame_data = (header << 72) + data
frame = make_frame(frame_data)

frameblocks = frame_to_blocks(frame)
codeblocks = [make_codeblock(block) for block in frameblocks]
CLTU = make_CLTU(codeblocks)
transmission = make_transmission(CLTU)

print(transmission)