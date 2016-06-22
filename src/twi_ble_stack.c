#include "twi_ble_stack.h"

#include "twi_ble_evt.h"
#include "twi_sys_evt.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "twi_error.h"

void ble_stack_init(void) {
  // Initialize the SoftDevice handler module.
  SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_RC_250_PPM_8000MS_CALIBRATION, false);
  /* The NRF_CLOCK_LFCLKSRC_RC_250_PPM_TEMP_4000MS_CALIBRATION would be ideal
     but it's not available in this SDK. As this recalibration is useful only
     if the temperature changes and it takes 17ms to do, recalibrating every
     8000MS doesn't seem too risky. More explanations here :
     devzone.nordicsemi.com/question/953/what-low-frequency-clock-sources-can-i-use
  */
  // Register with the SoftDevice handler module for BLE events.
  ERR_CHECK(softdevice_ble_evt_handler_set(ble_evt_dispatch));
  // Register with the SoftDevice handler module for BLE events.
  ERR_CHECK(softdevice_sys_evt_handler_set(sys_evt_dispatch));
}
