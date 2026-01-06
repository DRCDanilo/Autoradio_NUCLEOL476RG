/*
 * drv_uart.c
 *
 *  Created on: Nov 13, 2025
 *      Author: david
 */


#include "drv_uart.h"
//#include "shell.h"
//extern h_shell_t h_shell;


uint8_t drv_uart_receive(char * pData, uint16_t size){
	HAL_UART_Receive_IT(&huart2, (uint8_t*)pData,size);
	return 0;
}

uint8_t drv_uart_transmit(char * pData, uint16_t size){
	HAL_UART_Transmit(&huart2, (uint8_t*)pData,size,HAL_MAX_DELAY);
	return 0;
}
