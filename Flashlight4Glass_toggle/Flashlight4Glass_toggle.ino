//Tested in Arduino 1.5.8
#include <Usb.h>
#include "testUSB.h"

USBHost usb;
EndpointAccess mEA(&usb);

void setup() {
  Serial.begin(115200);
}

unsigned long previousMillis = 0;       
const long interval = 1000;         

void loop() {
  byte rcode;

  usb.Task();

  unsigned long currentMillis = millis();
  uint32_t current_state = usb.getUsbTaskState();

  if (current_state == USB_STATE_RUNNING) {
    if (mEA.isReady()) {
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        uint8_t buf[8] = {0x80};
        mEA.write(1, buf);
        Serial.println("TOGGLE LED");
      }
    }
  }


}

