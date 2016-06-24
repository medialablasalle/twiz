#include "ble_imu.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"

#define BLE_UUID_IMU_SERVICE    0x1901
#define BLE_UUID_IMU_DATA_CHAR  0x2B01


/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_imu       IMU Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_imu_t * p_imu, ble_evt_t * p_ble_evt)
{
    // TODO start imu timer ? (or use the advertizing one?)
    p_imu->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_imu       IMU Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_imu_t * p_imu, ble_evt_t * p_ble_evt)
{
    // TODO stop imu timer ? (or keep advertizing?)
    UNUSED_PARAMETER(p_ble_evt);
    p_imu->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief Function for handling the Write event.
 *
 * @param[in]   p_imu       IMU Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_imu_t * p_imu, ble_evt_t * p_ble_evt)
{
    if (p_imu->is_notification_supported)
    {
        ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

        if (
            (p_evt_write->handle == p_imu->imu_data_handles.cccd_handle)
            &&
            (p_evt_write->len == 2)
        )
        {
            // CCCD written, call application event handler
            if (p_imu->evt_handler != NULL)
            {
                ble_imu_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_IMU_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_IMU_EVT_NOTIFICATION_DISABLED;
                }

                p_imu->evt_handler(p_imu, &evt);
            }
        }
    }
}


void ble_imu_on_ble_evt(ble_imu_t * p_imu, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_imu, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_imu, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_imu, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for adding the IMU data characteristic.
 *
 * @param[in]   p_imu        IMU Service structure.
 * @param[in]   p_imu_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t imu_data_char_add(ble_imu_t * p_imu, const ble_imu_init_t * p_imu_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    ble_imu_data_t      initial_imu_data;
    uint8_t             encoded_report_ref[BLE_SRV_ENCODED_REPORT_REF_LEN];
    uint8_t             init_len;

    // Add IMU data characteristic
    if (p_imu->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));

        // The read operation on cccd should be possible without authentication.
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = p_imu_init->imu_data_char_attr_md.cccd_write_perm;
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    }

    memset(&char_md, 0, sizeof(char_md));

    #define CHAR_NAME "Twiz IMU"
    char_md.char_props.read         = 1;
    char_md.char_props.notify       = (p_imu->is_notification_supported) ? 1 : 0;
    char_md.p_char_user_desc        = (uint8_t*)(CHAR_NAME);
    char_md.char_user_desc_max_size = sizeof(CHAR_NAME);
    char_md.char_user_desc_size     = sizeof(CHAR_NAME);
    char_md.p_char_pf               = NULL;
    char_md.p_user_desc_md          = NULL;
    char_md.p_cccd_md               = (p_imu->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md               = NULL;

    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_IMU_DATA_CHAR); // TODO ?

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_imu_init->imu_data_char_attr_md.read_perm;
    attr_md.write_perm = p_imu_init->imu_data_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    initial_imu_data = p_imu_init->initial_imu_data;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(ble_imu_data_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(ble_imu_data_t);
    attr_char_value.p_value      = (uint8_t *)&initial_imu_data;

    err_code = sd_ble_gatts_characteristic_add(p_imu->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_imu->imu_data_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    if (p_imu_init->p_report_ref != NULL)
    {
        // Add Report Reference descriptor
        BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_REPORT_REF_DESCR);

        memset(&attr_md, 0, sizeof(attr_md));

        attr_md.read_perm = p_imu_init->imu_data_report_read_perm;
        BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);

        attr_md.vloc       = BLE_GATTS_VLOC_STACK;
        attr_md.rd_auth    = 0;
        attr_md.wr_auth    = 0;
        attr_md.vlen       = 0;

        init_len = ble_srv_report_ref_encode(encoded_report_ref, p_imu_init->p_report_ref);

        memset(&attr_char_value, 0, sizeof(attr_char_value));

        attr_char_value.p_uuid       = &ble_uuid;
        attr_char_value.p_attr_md    = &attr_md;
        attr_char_value.init_len     = init_len;
        attr_char_value.init_offs    = 0;
        attr_char_value.max_len      = attr_char_value.init_len;
        attr_char_value.p_value      = encoded_report_ref;

        err_code = sd_ble_gatts_descriptor_add(p_imu->imu_data_handles.value_handle,
                                               &attr_char_value,
                                               &p_imu->report_ref_handle);
        if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
    }
    else
    {
        p_imu->report_ref_handle = BLE_GATT_HANDLE_INVALID;
    }

    return NRF_SUCCESS;
}


uint32_t ble_imu_init(ble_imu_t * p_imu, const ble_imu_init_t * p_imu_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;
    ble_imu_data_t invalid_imu_data = {{0}};

    // Initialize service structure
    p_imu->evt_handler               = p_imu_init->evt_handler;
    p_imu->conn_handle               = BLE_CONN_HANDLE_INVALID;
    p_imu->is_notification_supported = p_imu_init->support_notification;
    p_imu->imu_data_last             = invalid_imu_data;

    // Add service
    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_IMU_SERVICE);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_imu->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add IMU data characteristic
    return imu_data_char_add(p_imu, p_imu_init);
}

inline bool is_connected(ble_imu_t * p_imu)
{
    // check if connected and notifying
    return ((p_imu->conn_handle != BLE_CONN_HANDLE_INVALID) && p_imu->is_notification_supported);
}

uint32_t ble_imu_data_update(ble_imu_t * p_imu, ble_imu_data_t * imu_data)
{
    uint32_t err_code = NRF_SUCCESS;
    uint16_t len = sizeof(ble_imu_data_t);

    // Save new IMU data
    memcpy(&(p_imu->imu_data_last), imu_data, len);

    // Update database
    err_code = sd_ble_gatts_value_set(p_imu->imu_data_handles.value_handle,
            0,
            &len,
            (uint8_t *)imu_data);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Send value if connected (and notifying)
    if (is_connected(p_imu))
    {
        ble_gatts_hvx_params_t hvx_params;
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle   = p_imu->imu_data_handles.value_handle;
        hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset   = 0;
        hvx_params.p_len    = &len;
        hvx_params.p_data   = (uint8_t *)imu_data;

        err_code = sd_ble_gatts_hvx(p_imu->conn_handle, &hvx_params);
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }

    return err_code;
}
