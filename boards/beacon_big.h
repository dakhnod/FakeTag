#ifndef BEACON_BIG_H
#define BEACON_BIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

// LEDs definitions for PCA10028
#define LEDS_NUMBER    3

#define LED_START      12
#define LED_0          12
#define LED_1          15
#define LED_2          16
#define LED_STOP       16

#define LEDS_ACTIVE_STATE 1

#define LEDS_LIST { LED_0, LED_1, LED_2 }

#define LEDS_INV_MASK  LEDS_MASK

#define BUTTONS_NUMBER 2

#define BUTTON_START   8
#define BUTTON_0       8
#define BUTTON_1       18
#define BUTTON_STOP    18
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST { BUTTON_0, BUTTON_1 }

#define HWFC           true

// Low frequency clock source to be used by the SoftDevice

#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_RC,            \
                                 .rc_ctiv       = 16,                                \
                                 .rc_temp_ctiv  = 2,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_250_PPM}

#ifdef __cplusplus
}
#endif

#endif // BEACON_BIG_H
