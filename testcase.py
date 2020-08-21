# -*- coding: utf-8 -*-
"""
Spyder Editor

Demonstration of what the physical and coding layer should do
Also allowed generating "correct" input for
usage with the arduino.

Data is stored as a 1-D list
"""

def acq_sequence():
    '''Generates 128-bit start sequence'''
    sequence = []
    for i in range(64):
        sequence.append(0)
        sequence.append(1)
    return sequence

def start_sequence():
    sequence = []
    sequence = sequence + [1, 1, 1, 0]
    sequence = sequence + [1, 0, 1, 1]
    sequence = sequence + [1, 0, 0, 1]
    sequence = sequence + [0, 0, 0, 0]
    return sequence

def idle_sequence():
    sequence = []
    for i in range(4):
        sequence.append(0)
        sequence.append(1)
    return sequence

def calculate_parity(octet):
    return 0

octet_1 = [0, 0, 0, 0, 0, 0, 0, 0]
octet_2 = [0, 0, 0, 0, 0, 0, 0, 0]
octet_3 = [0, 0, 0, 0, 0, 0, 0, 0]
octet_4 = [0, 0, 0, 0, 0, 0, 0, 0]
octet_5 = [0, 0, 0, 0, 0, 0, 0, 0]
octet_6 = [0, 0, 0, 0, 0, 0, 0, 0]
octet_7 = [0, 0, 0, 0, 0, 0, 0, 0]
