#ifndef WT51822_S4AT_H
#define WT51822_S4AT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

// LEDs definitions for WT51822_S4AT
#define LEDS_NUMBER    0

#define LEDS_ACTIVE_STATE 0

#define LEDS_LIST {}

#define LEDS_INV_MASK  LEDS_MASK

#define BUTTONS_NUMBER 0

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST {}

#define RX_PIN_NUMBER  1
#define TX_PIN_NUMBER  2
#define CTS_PIN_NUMBER 3
#define RTS_PIN_NUMBER 4
#define HWFC           false

#define SPIS_MISO_PIN  28    // SPI MISO signal.
#define SPIS_CSN_PIN   12    // SPI CSN signal.
#define SPIS_MOSI_PIN  25    // SPI MOSI signal.
#define SPIS_SCK_PIN   29    // SPI SCK signal.

#define SPIM0_SCK_PIN       3     /**< SPI clock GPIO pin number. */
#define SPIM0_MOSI_PIN      4     /**< SPI Master Out Slave In GPIO pin number. */
#define SPIM0_MISO_PIN      5     /**< SPI Master In Slave Out GPIO pin number. */
#define SPIM0_SS_PIN        6     /**< SPI Slave Select GPIO pin number. */

#define SPIM1_SCK_PIN       15     /**< SPI clock GPIO pin number. */
#define SPIM1_MOSI_PIN      12     /**< SPI Master Out Slave In GPIO pin number. */
#define SPIM1_MISO_PIN      14     /**< SPI Master In Slave Out GPIO pin number. */
#define SPIM1_SS_PIN        13     /**< SPI Slave Select GPIO pin number. */

// serialization APPLICATION board
#define SER_CONN_CHIP_RESET_PIN     12    // Pin used to reset connectivity chip

#define SER_APP_RX_PIN              25    // UART RX pin number.
#define SER_APP_TX_PIN              28    // UART TX pin number.
#define SER_APP_CTS_PIN             0     // UART Clear To Send pin number.
#define SER_APP_RTS_PIN             29    // UART Request To Send pin number.

#define SER_APP_SPIM0_SCK_PIN       7     // SPI clock GPIO pin number.
#define SER_APP_SPIM0_MOSI_PIN      0     // SPI Master Out Slave In GPIO pin number
#define SER_APP_SPIM0_MISO_PIN      30    // SPI Master In Slave Out GPIO pin number
#define SER_APP_SPIM0_SS_PIN        25    // SPI Slave Select GPIO pin number
#define SER_APP_SPIM0_RDY_PIN       29    // SPI READY GPIO pin number
#define SER_APP_SPIM0_REQ_PIN       28    // SPI REQUEST GPIO pin number

// serialization CONNECTIVITY board
#define SER_CON_RX_PIN              28    // UART RX pin number.
#define SER_CON_TX_PIN              25    // UART TX pin number.
#define SER_CON_CTS_PIN             29    // UART Clear To Send pin number. Not used if HWFC is set to false.
#define SER_CON_RTS_PIN             0    // UART Request To Send pin number. Not used if HWFC is set to false.


#define SER_CON_SPIS_SCK_PIN        7     // SPI SCK signal.
#define SER_CON_SPIS_MOSI_PIN       0     // SPI MOSI signal.
#define SER_CON_SPIS_MISO_PIN       30    // SPI MISO signal.
#define SER_CON_SPIS_CSN_PIN        25    // SPI CSN signal.
#define SER_CON_SPIS_RDY_PIN        29    // SPI READY GPIO pin number.
#define SER_CON_SPIS_REQ_PIN        28    // SPI REQUEST GPIO pin number.
// Low frequency clock source to be used by the SoftDevice

#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_RC,            \
                                 .rc_ctiv       = 16,                                \
                                 .rc_temp_ctiv  = 2,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_250_PPM}

#ifdef __cplusplus
}
#endif

#endif // WT51822_S4AT_H
