#include <stdint.h>
#include "low_res_timer.h"
#include "nordic_common.h"
#include "twi_advertising.h"
#include "leds.h"
#include "boards.h"
#include "twi_service.h"
#include "mpu9150.h"

extern ble_imu_t imu_service;

#define APP_TIMER_MAX_TIMERS            3                                           /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                           /**< Size of timer operation queues. */

app_timer_id_t                          m_imu_timer_id;                             /* The application timer ID that will update periodically the imu data */

#define IMU_UPDATE_INTERVAL_MS          40                                          /* The interval between two imu data updates (to set up the application timer) */

// IMU timer handler
static void imu_timer_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);

    bool connected = is_connected(&imu_service);

    // update imu data to send it
    if (connected) {
        // max number of time we can skip a notification (6sec):
        const int notif_timeout_cnt = 6000 / IMU_UPDATE_INTERVAL_MS;
        static int notif_cnt = 0;
        if (mpu9150_motion_detected(&notif_cnt, notif_timeout_cnt))
        {
            imu_data_t imu_data;
            ble_imu_data_update( &imu_service, get_imu_data(&imu_data) );
        }
    } else {
        // max number of time we can skip an advertizing (10 sec):
        const int adv_timeout_cnt = 10000 / IMU_UPDATE_INTERVAL_MS;
        static int adv_cnt = 0;
        if (mpu9150_motion_detected(&adv_cnt, adv_timeout_cnt))
        {
            advertising_init();
            advertising_start(); // TODO: check if this order works.
        } else {
            advertising_stop();
        }
    }

    // Visual debug : toggle LED
    static unsigned cpt = 0;
    int led = connected ? LED_B : LED_G;
    if ((++cpt % 25) == 0) // 25 * 40ms = 1 sec
        led_on(led);
    else
        led_off(led);
}

// Init low res timer
void low_res_timer_init(void)
{
    uint32_t err_code;

    // Initialize timer module, making it use the scheduler
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);

	// Register the timer that will update the imu data advertizing
	err_code = app_timer_create(&m_imu_timer_id, APP_TIMER_MODE_REPEATED, imu_timer_handler);
    APP_ERROR_CHECK(err_code);
}

// Start timers
void low_res_timer_start(void)
{
    uint32_t err_code;
    //Start the imu data update timer // TODO: use either IMU_UPDATE_INTERVAL_MS or APP_ADV_INTERVAL !!
    err_code = app_timer_start(m_imu_timer_id,
                               APP_TIMER_TICKS(IMU_UPDATE_INTERVAL_MS, APP_TIMER_PRESCALER),
                               NULL);
    APP_ERROR_CHECK(err_code);
}
