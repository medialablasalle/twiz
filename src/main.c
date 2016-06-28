#include "uart.h"
#include "imu.h"
#include "leds.h"
#include "low_res_timer.h"
#include "high_res_timer.h"
#include "twi_gap.h"
#include "twi_conn.h"
#include "twi_advertising.h"
#include "twi_ble_stack.h"
#include "twi_sys_evt.h"
#include "twi_scheduler.h"
#include "twi_calibration_store.h"
#include "ak8975a.h"
#include "twi_service.h"
#include "buttons.h"
#include "nrf_soc.h"

/**@brief Function for application main entry.
 */
int main(void)
{
    // Initialize
    leds_init();
    ble_stack_init();
    low_res_timer_init();
    high_res_timer_init();
    uart_init();
    imu_init();
    APP_ERROR_CHECK(pstorage_init());
    calibration_store_init();

    // Setup BLE stack
    gap_params_init();
    services_init();
    advertising_init();
    conn_params_init();
    sec_params_init();

    // Try load calibration data from flash
    imu_load_calibration_data();

    led_on(LED_G);
    // Wait for 1 second for a 'c' on the serial port or a button press.
    // If we get a 'c', then start calibration procedure
    printf("Press button or 'c' key to start calibration procedure\r\n");
    static char c;
    for (int i=0; i<1000; i++) {
        bool button_was_pressed = nrf_gpio_pin_read(BUTTON);
        if (getchar_timeout(1, &c) || button_was_pressed)
            if(c == 'c' || button_was_pressed) {
                led_off(LED_G);
                led_on(LED_R);
                printf("Starting calibration procedure\r\n");
                printf("Please close minicom and start python calibration GUI\r\n");
                imu_calibrate(button_was_pressed);
                led_off(LED_R);
                break;
            }
    }
    led_off(LED_G);

    // Start execution
    low_res_timer_start();
    advertising_start();

    gpiote_init();
    buttons_init();
    scheduler_init();

    // Enter main loop
    for (;;)
    {
        app_sched_execute();
        imu_update();
        sd_app_evt_wait(); // save power while waiting for new event
    }
}

/**
 * @}
 */
