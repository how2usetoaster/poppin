// POPPIN RP2040

#include "poppin.h"
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <stdio.h>
#include "Adafruit_TinyUSB.h"

Adafruit_USBD_HID CONTROLLER;

REPORT input_report;

uint8_t const HID_REPORT[] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x04,                    // USAGE (Joystick)
    0xa1, 0x01,                    // COLLECTION (Application)
    // Buttons
    0x05, 0x09,                    //   USAGE_PAGE (Button)
    0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
    0x29, 0x0b,                    //   USAGE_MAXIMUM (Button 11)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x0b,                    //   REPORT_COUNT (11)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    // Padding
    0x95, 0x05,                    //   REPORT_COUNT (5)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    // Lamps
    0x09, 0x00,                    //   USAGE (Undefined)
    0x05, 0x0a,                    //   USAGE_PAGE (Ordinals)
    0x19, 0x01,                    //   USAGE_MINIMUM (Instance 1)
    0x29, 0x09,                    //   USAGE_MAXIMUM (Instance 9)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x09,                    //   REPORT_COUNT (9)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0xc0                           // END_COLLECTION
};

/*
uint8_t const HID_REPORT[] = {
  TUD_HID_REPORT_DESC_GENERIC_INOUT(64)
};
*/

void scanKeys() {
  while (true) {
    // for (int i=0;i<9;i++) {
    //   input_report.setButton(wrap(i-1,0,9), false);
    //   input_report.setButton(i, true);
    //   sendReport();
    //   delay(100);
    // }
    // // Simple cycle

    for (int i=0;i<9;i++) {
      input_report.setButton(i, !digitalRead(BTN_INPUT[i]));
      // input_report.setButton(i, analogRead(26)%2);
    }
  }
}

bool controllerReady() {
  if (!TinyUSBDevice.mounted()) return false;
  if (!CONTROLLER.ready()) return false;
  return true;
}

void sendReport() {
  CONTROLLER.sendReport16(0, input_report.b);
}

void setup() {
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  for (int i=0;i<11;i++) {
    pinMode(BTN_INPUT[i],INPUT_PULLUP);
    pinMode(LMP_OUTPUT[i],OUTPUT);
  }
  pinMode(25,OUTPUT);
  multicore_launch_core1(scanKeys);
  
  CONTROLLER.enableOutEndpoint(true);
  CONTROLLER.setPollInterval(2);
  CONTROLLER.setReportDescriptor(HID_REPORT, sizeof(HID_REPORT));
  CONTROLLER.setStringDescriptor("poppin controller");
  CONTROLLER.setReportCallback(get_report_callback, set_report_callback);
  CONTROLLER.begin();
}

void loop() { // Scan for button press
  #ifdef TINYUSB_NEED_POLLING_TASK
  TinyUSBDevice.task();
  #endif

  while (!controllerReady()) {
    delay(10);
  }

  static uint32_t ms = 0;
  if (millis() - ms > POLLING) {
    ms = millis();
    sendReport();
  }
}

uint16_t get_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
  // not used in this example
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;
  //sendReport();
  return 0;
}

void set_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
  // This example doesn't use multiple report and report ID
  (void) report_id;
  (void) report_type;
  
  bool lamps[9] = {
    lampOn(buffer[0]),
    lampOn(buffer[1]),
    lampOn(buffer[2]),
    lampOn(buffer[3]),
    lampOn(buffer[4]),
    lampOn(buffer[5]),
    lampOn(buffer[6]),
    lampOn(buffer[7]),
    lampOn(buffer[8]),
  };
  for (int i=0;i<9;i++) {
    digitalWrite(LAMP[i],lamps[i]);
  }
  digitalWrite(25,lamps[0]);

  // echo back anything we received from host
}

// int wrap(int kX, int const kLowerBound, int const kUpperBound)
// {
//     int range_size = kUpperBound - kLowerBound + 1;

//     if (kX < kLowerBound)
//         kX += range_size * ((kLowerBound - kX) / range_size + 1);

//     return kLowerBound + (kX - kLowerBound) % range_size;
// }

bool lampOn(uint8_t data) {
  if (data > 0x7F) {
    return true;
  } else {
    return false;
  }
}