#include <stdbool.h>
#include <stdint.h>
#include "ble_advdata.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "bsp.h"
#include "keys.h"
#include "battery.h"
#include "nrf_drv_gpiote.h"

#define CENTRAL_LINK_COUNT              0                                 /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           0                                 /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define IS_SRVC_CHANGED_CHARACT_PRESENT 0                                 /**< Include or not the service_changed characteristic. if not enabled, the server's database cannot be changed for the lifetime of the device*/

#define APP_CFG_NON_CONN_ADV_TIMEOUT    0                                 /**< Time for which the device must be advertising in non-connectable mode (in seconds). 0 disables timeout. */
#define NON_CONNECTABLE_ADV_INTERVAL    MSEC_TO_UNITS(5000, UNIT_0_625_MS) /**< T
    err_code = bsp_init(BSP_INIT_LED, APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), NULL);he advertising interval for non-connectable advertisement (100 ms). This value can vary between 100ms to 10.24s). */

#define DEAD_BEEF                       0xDEADBEEF                        /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define APP_TIMER_PRESCALER             8                                 /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                 /**< Size of timer operation queues. */

#define KEY_CYCLE_PERIOD APP_TIMER_TICKS(3600 * 1000, APP_TIMER_PRESCALER)
#define LED_BLINK_PERIOD APP_TIMER_TICKS(100, APP_TIMER_PRESCALER)
#define LED_BLINK_COUNT 3
#define DEBOUNCE_PERIOD APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)

#define STATUS_FLAG_BATTERY_MASK           0b11000000
#define STATUS_FLAG_COUNTER_MASK           0b00111111
#define STATUS_FLAG_MEDIUM_BATTERY         0b01000000
#define STATUS_FLAG_LOW_BATTERY            0b10000000
#define STATUS_FLAG_CRITICALLY_LOW_BATTERY 0b11000000
#define STATUS_FLAG_MAINTAINED             0b00000100

#define BATTERY_COUNTER_THRESHOLD 10

static ble_gap_adv_params_t m_adv_params;                                 /**< Parameters to be passed to the stack when starting advertising. */

uint8_t status_flag = 0;
uint16_t current_key_index = 0;
uint8_t limit_led_blink = true;

APP_TIMER_DEF(key_cycle_timer);
APP_TIMER_DEF(led_blink_timer);
#ifdef BUTTON_PIN
APP_TIMER_DEF(debounce_timer);
uint8_t ignore_button = false;
#endif
#ifdef BUTTON_PIN
#endif

static uint8_t offline_finding_adv_template[] = {
	0x1e, /* Length (30) */
	0xff, /* Manufacturer Specific Data (type 0xff) */
	0x4c, 0x00, /* Company ID (Apple) */
	0x12, 0x19, /* Offline Finding type and length */
	0x00, /* State */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, /* First two bits */
	0x00, /* Hint (0x00) */
};

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

void fill_adv_template_from_key(uint8_t *key) {
	/* copy last 22 bytes */
	memcpy(&offline_finding_adv_template[7], &key[6], 22);
	/* append two bits of public key */
	offline_finding_adv_template[29] = key[0] >> 6;
}

void set_addr_from_key(uint8_t *key) {
	/* copy first 6 bytes */
	/* BLESSED seems to reorder address bytes, so we copy them in reverse order */

    ble_gap_addr_t address = {
        .addr_type = BLE_GAP_ADDR_TYPE_RANDOM_STATIC
    };

	address.addr[5] = key[0] | 0b11000000;
	address.addr[4] = key[1];
	address.addr[3] = key[2];
	address.addr[2] = key[3];
	address.addr[1] = key[4];
	address.addr[0] = key[5];

    NRF_LOG_DEBUG(
        "address is: %X:%X:%X:%X:%X:%X\n", 
        address.addr[5],
        address.addr[4],
        address.addr[3],
        address.addr[2],
        address.addr[1],
        address.addr[0]
        );

    ret_code_t ret_code = sd_ble_gap_address_set(BLE_GAP_ADDR_CYCLE_MODE_NONE, &address);
    APP_ERROR_CHECK(ret_code);
}

/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void advertising_init(void)
{
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
    m_adv_params.p_peer_addr = NULL;                             // Undirected advertisement.
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval    = NON_CONNECTABLE_ADV_INTERVAL;
    m_adv_params.timeout     = APP_CFG_NON_CONN_ADV_TIMEOUT;
}

static void set_key(uint8_t *key){
    set_addr_from_key(key);
    fill_adv_template_from_key(key);
    offline_finding_adv_template[6] = status_flag;
    ret_code_t err_code = sd_ble_gap_adv_data_set(
        offline_finding_adv_template,
        sizeof(offline_finding_adv_template),
        NULL,
        0
    );
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    ret_code_t err_code;

    err_code = sd_ble_gap_adv_start(&m_adv_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    uint32_t err_code;

    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                    PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);

    //Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);

    // Enable BLE stack.
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for doing power management.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

void set_key_by_index(uint16_t key_index){
    set_key(advertisement_keys[key_index]);
}

void update_battery_level(){
    static uint16_t battery_counter = BATTERY_COUNTER_THRESHOLD;
    if((++battery_counter) < BATTERY_COUNTER_THRESHOLD){
        return;
    }
    battery_counter = 0;
    uint8_t battery_level = get_current_level();
    NRF_LOG_DEBUG("battery level: %d\n", battery_level);
    status_flag &= (~STATUS_FLAG_BATTERY_MASK);
    if(battery_level > 80){
        // do nothing
    }else if(battery_level > 50){
        status_flag |= STATUS_FLAG_MEDIUM_BATTERY;
    }else if(battery_level > 30){
        status_flag |= STATUS_FLAG_LOW_BATTERY;
    }else{
        status_flag |= STATUS_FLAG_CRITICALLY_LOW_BATTERY;
    }
}

void key_cycle_timer_handler(void *context){
    update_battery_level();

    current_key_index++;
    current_key_index %= advertisement_key_count;
    set_key_by_index(current_key_index);
    NRF_LOG_DEBUG("current key index: %d\n", current_key_index);

}

void led_timer_handler(void *context){
    uint8_t blink_count = *((uint8_t*) context);
    if(blink_count & 1){
        bsp_board_leds_on();
        NRF_LOG_DEBUG("leds on\n");
    }else{
        bsp_board_leds_off();
        NRF_LOG_DEBUG("leds off\n");
    }
    (*((uint8_t*) context))--;
    if(blink_count <= 0){
        app_timer_stop(led_blink_timer);
    }
}

#ifdef BUTTON_PIN
void debounce_timer_handler(void *context){
    ignore_button = false;
    NRF_LOG_DEBUG("debounce timer\n");
}
#endif

void timers_init(){
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);

    app_timer_create(
        &key_cycle_timer,
        APP_TIMER_MODE_REPEATED,
        key_cycle_timer_handler
    );
    app_timer_create(
        &led_blink_timer,
        APP_TIMER_MODE_REPEATED,
        led_timer_handler
    );
    
    #ifdef BUTTON_PIN
    app_timer_create(
        &debounce_timer,
        APP_TIMER_MODE_SINGLE_SHOT,
        debounce_timer_handler
    );
    #endif
}

void blink_leds(uint8_t blink_count){
    static uint8_t blink_count_;
    blink_count_ = blink_count * 2;
    app_timer_start(
        led_blink_timer,
        LED_BLINK_PERIOD,
        &blink_count_
    );
}

void timers_start(){
    app_timer_start(
        key_cycle_timer,
        KEY_CYCLE_PERIOD,
        NULL
    );
    blink_leds(4);
}

void leds_init(){
    bsp_board_leds_init();
}

void button_handler(long unsigned int pin, nrf_gpiote_polarity_t polarity){
    if(ignore_button){
        return;
    }
    ignore_button = true;
    ret_code_t err_code;
    err_code = app_timer_start(
        debounce_timer,
        DEBOUNCE_PERIOD,
        NULL
    );
    APP_ERROR_CHECK(err_code);

    static uint8_t button_counter = 0;
    button_counter++;
    status_flag &= (~STATUS_FLAG_COUNTER_MASK);
    status_flag |= (button_counter & STATUS_FLAG_COUNTER_MASK);
    NRF_LOG_DEBUG("setting button counter to %d\n", (button_counter & STATUS_FLAG_COUNTER_MASK));
    set_key_by_index(current_key_index);
    blink_leds(25);
}

void buttons_init(){
    #ifdef BUTTON_PIN
    NRF_LOG_DEBUG("enabling button support\n");
    ret_code_t err_code;
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_in_config_t in_config = {
        .sense = BUTTON_PULL == NRF_GPIO_PIN_PULLUP ? NRF_GPIOTE_POLARITY_HITOLO : NRF_GPIOTE_POLARITY_LOTOHI,
        .pull = BUTTON_PULL,
        .hi_accuracy = false,
        .is_watcher = false
    };
    err_code = nrf_drv_gpiote_in_init(BUTTON_PIN, &in_config, button_handler);
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_in_event_enable(BUTTON_PIN, true);
    #endif

}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    ret_code_t err_code;
    // Initialize.
    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEBUG("key count: %d\n", advertisement_key_count);

    leds_init();
    buttons_init();

    timers_init();
    timers_start();

    ble_stack_init();
    advertising_init();

    update_battery_level();

    set_key_by_index(0);
    // Start execution.
    NRF_LOG_INFO("BLE Beacon started\r\n");
    advertising_start();

    // Enter main loop.
    for (;; )
    {
        if (NRF_LOG_PROCESS() == false)
        {
            power_manage();
        }
    }
}


/**
 * @}
 */
