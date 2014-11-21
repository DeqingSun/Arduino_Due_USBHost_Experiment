//Tested in Arduino 1.5.8
#include <Usb.h>
#include "DueHost.h"

USBHost usb;
EndpointAccess mEA(&usb);

void setup() {
  Serial.begin(115200);
}

unsigned long previousMillis = 0;
const long interval = 1000;
unsigned char ledPattern = 0;

void loop() {
  byte rcode;

  usb.Task();

  unsigned long currentMillis = millis();
  uint32_t current_state = usb.getUsbTaskState();

  if (current_state == USB_STATE_RUNNING && mEA.isReady()) {
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      uint8_t buf[3];
      buf[0] = (ledPattern >> 0) & 1;
      buf[1] = (ledPattern >> 1) & 1;
      buf[2] = (ledPattern >> 2) & 1;

      mEA.write(3, buf);

      Serial.print("TOGGLE LED ");
      Serial.println(ledPattern & 7);

      ledPattern++;
    }
  } else {
    previousMillis = currentMillis;
  }


}

