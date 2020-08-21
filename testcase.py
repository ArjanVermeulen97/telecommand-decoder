# -*- coding: utf-8 -*-
"""
Spyder Editor

Demonstration of what the physical and coding layer should do
Also allowed generating "correct" input for
usage with the arduino.

Data is stored as a 1-D list
"""

# Pre-defined strings.
acq_sequence = 0b01010101_01010101_01010101_01010101_01010101_01010101_01010101_01010101
start_sequence = 0b1110_1011_1001_0000
idle_sequence = 0b0101_0101
tail_sequence = 0b01_0101_0101

# Make python actually print the numbers as binary.
acq_sequence_string = "{0:b}".format(acq_sequence)
start_sequence_string = "{0:b}".format(start_sequence)
idle_sequence_string = "{0:b}".format(idle_sequence)
tail_sequence_string = "{0:b}".format(tail_sequence)

def rot_shift(x, n):
    '''Performs n-bit rotational shift of x.
    i.e. rot_shift(0b0001, 1) = 0b1000
    rot_shift(0b0100, 1) = 0b0010
    
    Note: x has to be 8-bit and n > 0.
    '''
    return (x << n) | (x >> (8 - n))

def calculate_bch(data):
    '''Calculates parity digits according to modified BCH:
    g(x) = x7 + x6 + x2 + x0
    And appends a 0 to the end of it.
    '''
    data_0 = data               # x^0
    data_2 = rot_shift(data, 2) # x^2
    data_6 = rot_shift(data, 6) # x^6
    data_7 = rot_shift(data, 7) #x ^7
    # Perform XOR-addition
    data = data_0 ^ data_2 ^ data_6 ^ data_7 
    # Add trailing 0
    data = data << 1
    return data

# Input data
octet_1 = 0b1000_0000
octet_2 = 0b0100_0000
octet_3 = 0b0010_0000
octet_4 = 0b0001_0000
octet_5 = 0b0000_1000
octet_6 = 0b0000_0100
octet_7 = 0b0000_0010

total_code = 0
total_code = total_code + (octet_7)
total_code = total_code + (octet_6 << 8)
total_code = total_code + (octet_5 << 16)
total_code = total_code + (octet_4 << 24)
total_code = total_code + (octet_3 << 32)
total_code = total_code + (octet_2 << 40)
total_code = total_code + (octet_1 << 48)
total_code_string = "{0:b}".format(total_code)
print("Total code:")
print(total_code_string)
print("Perform BCH encoding")
total_code_bch = calculate_bch(total_code)
total_code_bch_string = "{0:b}".format(total_code_bch)
print("BCH-encoded code:")
print(total_code_bch_string)
print("Add start sequence and tail sequence")
cltu = start_sequence_string + total_code_bch_string + idle_sequence_string
print(f"CLTU: {cltu}")
print("For total transmission, add acquisition block and final idle.")
print("Total transmission:")
transmission = acq_sequence_string + cltu + idle_sequence_string
print(transmission)

## TO DO: write decoding algorithm.