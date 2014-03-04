###############################
# EV Datalogger Project
# Jon Sowman 2014
# University of Southampton
# All Rights Reserved
###############################

import struct
import time

# How many channels
channels = 10

# Open the output file and write the header
w = open('parsed.log', 'w+')
w.write("EV Logger Parsed Log\n")
w.write('Generated: ' + time.strftime("%c") + '\n')
w.write('Frequency: 1kHz\n')
w.write('ADC0, ADC1, ADC2, ADC3, ADC4, ADC5, ADC6, ACCELX, ACCELY, ACCELZ\n')
w.write('\n')

done = False

with open("sample.log", "r") as f:
    byte = f.read(2)
    while 1:
        for i in range(channels):
            # Swap the byte order here
            a = struct.pack('<h', *struct.unpack('>h', byte))
            w.write(str(int(a.encode("hex"), 16)))
            if i < (channels - 1):
                w.write(', ')
            byte = f.read(2)
            if not byte:
                done = True
        if done:
            break
        w.write('\n')
    w.close()
