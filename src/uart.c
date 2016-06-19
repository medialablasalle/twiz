#include <stdint.h>
#include <stdbool.h>

#include "uart.h"
#include "boards.h"
#include "nrf_delay.h"
#include "simple_uart.h"

#include "ble_nus.h"
#include "twi_service.h"

void uart_init()
{
    simple_uart_config(UART_RTS_PIN, UART_TX_PIN, UART_CTS_PIN, UART_RX_PIN, false);

    NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Enabled << UART_INTENSET_RXDRDY_Pos;

    NVIC_SetPriority(UART0_IRQn, APP_IRQ_PRIORITY_LOW);
    NVIC_EnableIRQ(UART0_IRQn);
}

int putchar(int c)
{
    simple_uart_put(c);
    return 0;
}

int getchar() {
    return simple_uart_get();
}

// Return NRF_SUCCESS on success, -1 on timeout
bool getchar_timeout(uint32_t timeout_ms, char *c)
{
    return simple_uart_get_with_timeout(timeout_ms, (uint8_t *)c);
}


// Read an line from serial port until \r or until size-1 bytes are read. Returns the line,
// NULL terminated, without the \r. \n are skipped.
void getline(int size, char* buf)
{
    char c;
    while (size > 1) {
        c = getchar();
        if (c == '\n')
            continue;
        if (c == '\r') {
            *buf = 0;
            return;
        }
        *buf = c;
        size--;
        buf++;
    }
    *buf = 0;
}

/**@brief    Function for handling the data from the Nordic UART Service.
 *
 * @details  This function will process the data received from the Nordic UART BLE Service and send
 *           it to the UART module.
 */
void nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length)
{

    /**@snippet [Handling the data received over BLE] */

    for (int i = 0; i < length; i++)
    {
        simple_uart_put(p_data[i]);
    }
    simple_uart_put('\n');
}

/**@brief   Function for handling UART interrupts.
 *
 * @details This function will receive a single character from the UART and append it to a string.
 *          The string will be be sent over BLE when the last character received was a 'new line'
 *          i.e '\n' (hex 0x0D) or if the string has reached a length of @ref NUS_MAX_DATA_LENGTH.
 */
void UART0_IRQHandler(void)
{
    static uint8_t data_array[BLE_NUS_MAX_DATA_LEN];
    static uint8_t index = 0;
    uint32_t err_code;

    /**@snippet [Handling the data received over UART] */

    data_array[index] = simple_uart_get();
    index++;

    if ((data_array[index - 1] == '\n') || (index >= (BLE_NUS_MAX_DATA_LEN - 1)))
    {
        err_code = ble_nus_send_string(&m_nus, data_array, index + 1);
        if (err_code != NRF_ERROR_INVALID_STATE)
        {
            APP_ERROR_CHECK(err_code);
        }

        index = 0;
    }
}

