Nunmouse
=========

This is a simple Teensy program to emulate a mouse using a modified Wii Nunchuck.


My first attempts, using i²c communication with the Nunchuck and mouse emulation for the computer were not satisfying.
The mouse moves were not natural and the i²c communication needed to be frequently reset.

So, I opened a Nunchuck, cut the wire, and connect some digital and analog inputs of the Teensy directly to the central point of the X/Y potentiometers and the buttons Red and White wires.

I put the Teensy (3.2) directly inside the Nunchuck and replace the cable by a standard USB one.


