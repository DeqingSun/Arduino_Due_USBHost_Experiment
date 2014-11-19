#!/usr/bin/python

#Filename : helloworld.py
import os

print 'Hello World'

arduino_avr_path = '/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/'

arduino_port = '/dev/tty.usbmodem141341'

arduino_upload_file = 'default/Lufa_leonardo.hex'

os.system(arduino_avr_path+'bin/avrdude -C'+arduino_avr_path+'etc/avrdude.conf -patmega32u4 -cavr109 -P'+arduino_port+' -b57600 -D -Uflash:w:'+arduino_upload_file+':i');
