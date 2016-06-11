#ifndef BLE_TWI_SERVICE_H
#define BLE_TWI_SERVICE_H

#include "ble_imu.h"
#include "ble_lbs.h"

extern ble_imu_t imu_service;
extern ble_lbs_t m_lbs;

void services_init(void);


#endif
