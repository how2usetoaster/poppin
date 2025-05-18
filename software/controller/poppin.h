// POPPIN RP2040
// v1b and v2

#ifndef POPPIN_RP2040
#define POPPIN_RP2040

/*
    BUTTON AND LAMP DEFINITIONS
*/

static int POLLING = 2;

typedef struct {
  uint16_t b;
  void setButton(int btn, bool value) {
    uint16_t current = 0;
    int i = 0;
    for (i;i<16;i++) {
      uint16_t bit = 0;
      if (i==btn) {
        bit = (int)value << i;
      } else {
        bit = (b & (1 << i));
      }
      current |= bit;
    }
    b = current;
  }
} REPORT;

// On v1b and v2 these are conveniently mapped as even and odd 0-21
static int BTN_INPUT[11] = {
  0,2,4,6,8,10,12,14,16, // Buttons 1-9
  18,20 // Test and Service
};

static int LMP_OUTPUT[11] = {
  1,3,5,7,9,11,13,15,17, // Buttons 1-9
  19,21 // Test and Service
};

static int BUTTON[9] = {BTN_INPUT[0],BTN_INPUT[1],BTN_INPUT[2],BTN_INPUT[3],BTN_INPUT[4],BTN_INPUT[5],BTN_INPUT[6],BTN_INPUT[7],BTN_INPUT[8]}; 

static int LAMP[9] = {LMP_OUTPUT[0],LMP_OUTPUT[1],LMP_OUTPUT[2],LMP_OUTPUT[3],LMP_OUTPUT[4],LMP_OUTPUT[5],LMP_OUTPUT[6],LMP_OUTPUT[7],LMP_OUTPUT[8]};

static int TEST_BUTTON = BTN_INPUT[9];
static int SERVICE_BUTTON = BTN_INPUT[10];

static int TEST_LED = LMP_OUTPUT[9];
static int SERVICE_LED = LMP_OUTPUT[10];

/*
    lamp stuff idk
*/

#endif