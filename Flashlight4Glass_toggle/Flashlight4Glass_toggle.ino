//Tested in Arduino 1.5.8
#include <Usb.h>
#include "testUSB.h"

#define TARGET_VID 
#define TARGET_PID 

USBHost usb;
EndpointAccess mEA(&usb);

void setup() {
  Serial.begin(115200);
}

unsigned char last_state = 0xFF;
boolean board_plugged = false;
uint32_t target_device_addr = 0;
boolean target_device_found = false;

void loop() {
  byte rcode;

  usb.Task();

 /* unsigned char current_state = usb.getUsbTaskState();

  if (current_state != last_state) {

    //print_USB_state(current_state);

    switch (current_state) {


      case USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE:
        if (board_plugged == true) {
          target_device_found = false;
          board_plugged = false;
          usb.ReleaseDevice(1);
        }
        break;


      case USB_STATE_RUNNING:
        if (board_plugged == false) {
          Serial.println("!!!!BOARD PLUGGED");
          usb.ForEachUsbDevice(&CheckTargetDevice);
        }

        board_plugged = true;
        break;

    }

    last_state = current_state;
  }*/

}

