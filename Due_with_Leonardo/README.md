Blink an LED the hard way
==============================
![Blink LED via USB](USB_Arduino.jpg?raw=true)

Here is an example of using Arduino Due's usb host port. The USB device is also an Arduino so repeat this experiment will be easier. The Arduino Leonardo is running LUFA's Generic HID demo. If you don't want to set up toolchain for LUFA. There is a pre-compiled hex file for Leonardo. Please notice this demo will take over USB and you have to reset your Leonardo to enter bootloader again. The Due side code can be compiled and uploaded with Arduino 1.5.8 directly.

![USB connection](USB_Connection.png?raw=true)

Protocol of this USB connection is quite simple. First byte to Leonardo's endpoint controls first LED (RXLED on PB0). Second byte controls second LED (TXLED on PD5). Third byte controls third LED (L LED on PC7, Micro Pro doesn't have it).

When Leonardo is plugged into Due's host port, Due check's its PID and VID. If they matches, Due will write packets to Leonardo every second to toggle LEDs on it.



