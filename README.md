Arduino Due USBHost Experiment
==============================
This repository aims to experiment the possibility using Arduino Due, 
a ubiquitous hardware, as a USB host to talk to USB devices. 

Arduino comes with Arduino IDE and Arduino IDE 1.5.8 has basic implementation of USBhost, 
though it can only talk to USB keyboard, USB mouse and act as ADK host. 
The goal of this experiment is to explore the way of utilize the USB port with minimal change of Arduino IDE 
and avoid using a separate framework such as LUFA. 

At this time no USB host library other than Arduino's official one exist. 
However the official library is a mixture of Circuits@Home's USB library, Peter Barrett's code and ATMEL's code. 
The difference between Circuits@Home's USB library and USBHost library is the low level code, 
and USBhost changed datatype to 32bit. Also Arduino only ported hidboot and ADK. 

From the experiment my conclusion is that there is no need to change anything in Arduino IDE or add any library to support new USB devices.
I only need to add one cpp file and a header file that contain a interface support class. 
So the whole project will only has 3 files and they can be compiled and uploaded via Arduino IDE directly.
