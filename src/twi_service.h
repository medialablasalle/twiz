#ifndef BLE_TWI_SERVICE_H
#define BLE_TWI_SERVICE_H

#include "ble_imu.h"
#include "ble_lbs.h"
#include "ble_nus.h"

extern ble_imu_t imu_service;
extern ble_lbs_t m_lbs;
extern ble_nus_t m_nus;

void services_init(void);


#endif
