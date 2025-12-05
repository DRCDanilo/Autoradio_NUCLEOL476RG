/*
 * drv_uart.c
 *
 *  Created on: Nov 27, 2025
 *      Author: danil
 */
#include "drv_uart.h"

#include "usart.h"

#include "cmsis_os.h" //To include all OS FreeRTOS functions
extern SemaphoreHandle_t sem_usart;
extern UART_HandleTypeDef huart2;

uint8_t drv_uart_receive(char * pData, uint16_t size)
{
	//char c;

	//HAL_UART_Receive(&UART_DEVICE, (uint8_t*)(&c), 1, HAL_MAX_DELAY);//Code to work in regular mode and inside of a task

	//To receive data from interruption
	//HAL_UART_Receive_IT(&UART_DEVICE, (uint8_t*)(pData) , size);
	HAL_UART_Receive_IT(&huart2, (uint8_t*)(pData) , size);
	//Block the task: take an empty semaphore
	xSemaphoreTake(sem_usart, portMAX_DELAY);

	//return c;
	return 0;
}

uint8_t drv_uart_transmit(char * pData, uint16_t size)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)pData, size, HAL_MAX_DELAY);
	//return size;
	return 0;
}
