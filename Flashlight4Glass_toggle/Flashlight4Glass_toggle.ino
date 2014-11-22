//Tested in Arduino 1.5.8
#include <Usb.h>
#include "testUSB.h"
//Need to hack VCC to 3.3V on Due
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

USBHost usb;
EndpointAccess mEA(&usb);

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);              // start the library
  lcd.setCursor(0, 0);
  lcd.print("LCD_Init"); // print a simple message
}

unsigned long previousMillis = 0;
const long interval = 1000;
uint32_t last_state = 0;
bool flashlightOn = false;

void loop() {
  byte rcode;

  usb.Task();

  unsigned long currentMillis = millis();
  uint32_t current_state = usb.getUsbTaskState();

  if (current_state == USB_STATE_RUNNING) {
    if (last_state != current_state) {
      lcd.clear();
      lcd.print("Found Flashlight");
    }
    if (mEA.isReady()) {
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        uint8_t buf[2] = {'F', 'F'};
        lcd.setCursor(0, 1);
        if (flashlightOn) {
          lcd.print("OFF");
        } else {
          buf[1] = 'O';
          lcd.print("ON ");
        }
        mEA.write(2, buf);
        Serial.println("TOGGLE LED");
        flashlightOn = !flashlightOn;
      }
    } else {
      if (last_state != current_state) {
        lcd.clear();
        lcd.print("Not Flashlight");
      }
    }
  } else if (current_state == USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE) {
    if (last_state != current_state) {
      lcd.clear();
      lcd.print("Wait USB Device");
      flashlightOn = false;
    }
  }
  last_state = current_state;
}

