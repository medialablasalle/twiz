#include "twi_ble_evt.h"

#include <string.h>

#include "nrf_soc.h"

#include "twi_advertising.h"
#include "twi_conn.h"
#include "twi_gap.h"
#include "ble_hci.h"
#include "leds.h"
#include "boards.h"
#include "twi_service.h"
#include "ble_lbs.h"
#include "app_button.h"

extern bool is_advertising;

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt) // TODO prune useless cases? (sec, auth...)
{
    uint32_t                         err_code;
    static ble_gap_evt_auth_status_t m_auth_status;
    ble_gap_enc_info_t *             p_enc_info;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            is_advertising = false;
            err_code = app_button_enable();
            APP_ERROR_CHECK(err_code);
            led_off(LED_0);
            led_off(LED_1);
            led_off(LED_2);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            err_code = app_button_disable();
            APP_ERROR_CHECK(err_code);
            advertising_start();
            led_off(LED_0);
            led_off(LED_1);
            led_off(LED_2);
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle,
                                                   BLE_GAP_SEC_STATUS_SUCCESS,
                                                   &m_sec_params);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_AUTH_STATUS:
            m_auth_status = p_ble_evt->evt.gap_evt.params.auth_status;
            break;

        case BLE_GAP_EVT_SEC_INFO_REQUEST:
            p_enc_info = &m_auth_status.periph_keys.enc_info;
            if (p_enc_info->div == p_ble_evt->evt.gap_evt.params.sec_info_request.div)
            {
                err_code = sd_ble_gap_sec_info_reply(m_conn_handle, p_enc_info, NULL);
                APP_ERROR_CHECK(err_code);
            }
            else
            {
                // No keys found for this device
                err_code = sd_ble_gap_sec_info_reply(m_conn_handle, NULL, NULL);
                APP_ERROR_CHECK(err_code);
            }
            break;

        default:
            // No implementation needed.
            break;
    }
}

void ble_evt_dispatch(ble_evt_t * p_ble_evt) {
  on_ble_evt(p_ble_evt);
  ble_imu_on_ble_evt(&imu_service, p_ble_evt);
  ble_lbs_on_ble_evt(&m_lbs, p_ble_evt);
  ble_nus_on_ble_evt(&m_nus, p_ble_evt);
  ble_conn_params_on_ble_evt(p_ble_evt);
}
