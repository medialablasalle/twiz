#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>
#include "ble_nus.h"

void uart_init(void);
int putchar(int c);
int getchar(void);
bool getchar_timeout(uint32_t timeout_ms, char *c);
void getline(int size, char *buf);

void nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length);

#endif
