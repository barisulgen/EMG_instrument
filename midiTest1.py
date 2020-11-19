# -*- coding: utf-8 -*-
"""
@author: baris

2 channel MIDI i/o test (data from text file)
"""
#%%

import time
import rtmidi

midiout = rtmidi.MidiOut()
available_ports = midiout.get_ports()

print(available_ports)

#%%
if available_ports:
    midiout.open_port(1)
else:
    midiout.open_virtual_port("My virtual output")

#%%-------------------------------------------

# Opening file 
file = open('midiTest1.txt', 'r') 
  
# Using for loop 

    
with midiout:
    print("Using for loop") 
    for line in file:
        valSTR = line.strip()
        val = int(valSTR)
        print(val)
        if val >= 20:
            note_on_ch1 = [0x90, val, 112] # channel 1, middle C, velocity 112
            note_on_ch2 = [0x91, val, 112]
            note_off_ch1 = [0x80, val, 0]
            note_off_ch2 = [0x81, val, 0]
            midiout.send_message(note_on_ch1)
            midiout.send_message(note_on_ch2)
            time.sleep(0.2)
            midiout.send_message(note_off_ch1)
# Closing files 
file.close() 

#%%-------------------------------------------



time.sleep(0.2)
del midiout