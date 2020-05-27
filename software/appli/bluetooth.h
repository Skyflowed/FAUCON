/*
 * bluetooth.h
 *
 *  Created on: 3 avr. 2020
 *      Author: Nirgal
 */

#ifndef APPLI_BLUETOOTH_H_
#define APPLI_BLUETOOTH_H_

#include "config.h"
#include "stm32f4_uart.h"

void BLUETOOTH_init(void);

#define BLUETOOTH_printf(...)	UART_printf(BLUETOOTH_UART_ID, __VA_ARGS__)

#endif /* APPLI_BLUETOOTH_H_ */
