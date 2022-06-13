#ifndef BEACON_ROUND_H
#define BEACON_ROUND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

// LEDs definitions for PCA10028
#define LEDS_NUMBER    1

#define LED_START      29
#define LED_0          29
#define LED_STOP       29

#define LEDS_ACTIVE_STATE 1

#define LEDS_LIST { LED_0 }

#define LEDS_INV_MASK  LEDS_MASK

#define BUTTONS_NUMBER 1

#define BUTTON_START   28
#define BUTTON_0       28
#define BUTTON_STOP    28
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST { BUTTON_0 }

#define HWFC           true

// Low frequency clock source to be used by the SoftDevice

#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}

#ifdef __cplusplus
}
#endif

#endif // BEACON_ROUND_H
