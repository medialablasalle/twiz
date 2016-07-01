#ifndef BLE_IMU_H__
#define BLE_IMU_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "imu.h"

typedef imu_data_t ble_imu_data_t;
/* OLD Custom Data Structure:
typedef struct imu_data { int16_t imu[4]; } ble_imu_data_t; // DEPRECATED - TODO: REMOVE
*/

/**@brief IMU Service event type. */
typedef enum
{
    BLE_IMU_EVT_NOTIFICATION_ENABLED,                             /**< IMU data notification enabled event. */
    BLE_IMU_EVT_NOTIFICATION_DISABLED                             /**< IMU data notification disabled event. */
} ble_imu_evt_type_t;

/**@brief IMU Service event. */
typedef struct
{
    ble_imu_evt_type_t evt_type;                                  /**< Type of event. */
} ble_imu_evt_t;

// Forward declaration of the ble_imu_t type.
typedef struct ble_imu_s ble_imu_t;

/**@brief IMU Service event handler type. */
typedef void (*ble_imu_evt_handler_t) (ble_imu_t * p_imu, ble_imu_evt_t * p_evt);

/**@brief IMU Service init structure. This contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_imu_evt_handler_t         evt_handler;                    /**< Event handler to be called for handling events in the IMU Service. */
    bool                          support_notification;           /**< TRUE if notification is supported. */
    ble_srv_report_ref_t *        p_report_ref;                   /**< If not NULL, a Report Reference descriptor with the specified value will be added to the IMU data characteristic */
    ble_imu_data_t                initial_imu_data;               /**< Initial value */
    ble_srv_cccd_security_mode_t  imu_data_char_attr_md;          /**< Initial security level for IMU data attribute */
    ble_gap_conn_sec_mode_t       imu_data_report_read_perm;      /**< Initial security level for IMU report read attribute */
} ble_imu_init_t;

/**@brief IMU Service structure. This contains various status information for the service. */
typedef struct ble_imu_s
{
    ble_imu_evt_handler_t         evt_handler;                    /**< Event handler to be called for handling events in the IMU Service. */
    uint16_t                      service_handle;                 /**< Handle of IMU Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t      imu_data_handles;               /**< Handles related to the characteristic. */
    uint16_t                      report_ref_handle;              /**< Handle of the Report Reference descriptor. */
    ble_imu_data_t                imu_data_last;                  /**< Last value passed to the IMU Service. */
    uint16_t                      conn_handle;                    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    bool                          is_notification_supported;      /**< TRUE if notification is supported. */
} ble_imu_t;

/**@brief Function for initializing the IMU Service.
 *
 * @param[out]  p_imu       IMU Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_imu_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_imu_init(ble_imu_t * p_imu, const ble_imu_init_t * p_imu_init);

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the IMU Service.
 *
 * @note For the requirements in the specification to be fulfilled, ble_imu_data_update()
 *       must be called upon reconnection if the IMU data has changed while the service
 *       has been disconnected from a bonded client.
 *
 * @param[in]   p_imu      IMU Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_imu_on_ble_evt(ble_imu_t * p_imu, ble_evt_t * p_ble_evt);

/**@brief Function for updating the IMU data.
 *
 * @details The application calls this function after having performed an IMU data sample. If
 *          notification has been enabled, the IMU data characteristic is sent to the client.
 *
 * @note For the requirements in the specification to be fulfilled, this function must be
 *       called upon reconnection if the IMU data has changed while the service has been
 *       disconnected from a bonded client.
 *
 * @param[in]   p_imu          IMU Service structure.
 * @param[in]   imu_data       New IMU data.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_imu_data_update(ble_imu_t * p_imu, ble_imu_data_t * imu_data);


/**@brief Function to know if there is a connection to the IMU service
 *
 * @details The application calls this function after having performed an IMU data sample. If
 *          notification has been enabled, the IMU data characteristic is sent to the client.
 *
 * @note For the requirements in the specification to be fulfilled, this function must be
 *       called upon reconnection if the IMU data has changed while the service has been
 *       disconnected from a bonded client.
 *
 * @param[in]   p_imu          IMU Service structure.
 * @param[in]   imu_data       New IMU data.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
bool is_connected(ble_imu_t * p_imu);

#endif // BLE_IMU_H__

/** @} */
