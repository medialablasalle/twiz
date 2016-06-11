#include <string.h>
#include "twi_service.h"
#include "ble_imu.h" //IMU service
#include "ble_lbs.h"

ble_imu_t imu_service;
ble_lbs_t m_lbs;

/**@brief Function for initializing services that will be used by the application.
 */
void services_init(void)
{
    uint32_t err_code;

    // Initialize IMU Service
    ble_imu_init_t imu_init;
    memset(&imu_init, 0, sizeof(imu_init));

    // Here the sec level for the IMU Service can be changed/increased. // TODO find defaults + remove ?
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&imu_init.imu_data_char_attr_md.cccd_write_perm); //A llow GATT client to modify CCCD config (to disable notifications ?)
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&imu_init.imu_data_char_attr_md.read_perm); // Allow reading on open links
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&imu_init.imu_data_char_attr_md.write_perm); // No writing by the client
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&imu_init.imu_data_report_read_perm); // Allow reading report char.

    imu_init.support_notification = true;

    err_code = ble_imu_init(&imu_service, &imu_init);
    APP_ERROR_CHECK(err_code);

    // Initialize LBS Service
    ble_lbs_init_t init;
    init.led_write_handler = (void*) led_write_handler;

    err_code = ble_lbs_init(&m_lbs, &init);
    APP_ERROR_CHECK(err_code);
}


