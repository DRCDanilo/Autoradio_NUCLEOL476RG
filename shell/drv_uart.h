/*
 * drv_uart.h
 *
 *  Created on: Nov 27, 2025
 *      Author: danilo
 */

#ifndef DRV_UART_H_
#define DRV_UART_H_

#include <stdint.h>

uint8_t drv_uart_receive(char * pData, uint16_t size);
uint8_t drv_uart_transmit(char * pData, uint16_t size);

#endif /* DRV_UART_H_ */
