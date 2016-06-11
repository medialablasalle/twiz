#include "buttons.h"
#include "boards.h"
#include "low_res_timer.h"
#include "app_timer.h"
#include "app_button.h"
#include "ble_lbs.h"
#include "app_gpiote.h"

/**< Maximum number of users of the GPIOTE handler. */
#define APP_GPIOTE_MAX_USERS 1

extern ble_lbs_t m_lbs;

static void button_event_handler(uint8_t pin_no, uint8_t button_action)
{
    uint32_t err_code;

    switch (pin_no)
    {
        case BUTTON:
            err_code = ble_lbs_on_button_change(&m_lbs, !button_action); // inverted logic
            if (err_code != NRF_SUCCESS &&
                err_code != BLE_ERROR_INVALID_CONN_HANDLE &&
                err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        default:
            APP_ERROR_HANDLER(pin_no);
            break;
    }
}

/**@brief Function for initializing the GPIOTE handler module.
 */
void gpiote_init(void)
{
    APP_GPIOTE_INIT(APP_GPIOTE_MAX_USERS);
}

/**@brief Function for initializing the button handler module.
 */
void buttons_init(void)
{
    // Note: Array must be static because a pointer to it will be saved in the Button handler
    //       module.
    static app_button_cfg_t buttons[] =
    {
        {BUTTON, false, BUTTON_PULL, button_event_handler},
        // {WAKEUP_BUTTON_PIN, false, BUTTON_PULL, NULL}, // TODO
    };

    APP_BUTTON_INIT(buttons, sizeof(buttons) / sizeof(buttons[0]), BUTTON_DETECTION_DELAY, true);
}

