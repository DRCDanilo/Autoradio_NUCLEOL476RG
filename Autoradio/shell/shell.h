/*
 * shell.h
 *
 *  Created on: Nov 13, 2025
 *      Author: david
 */

#ifndef SHELL_H_
#define SHELL_H_


#include <stdint.h>
#include "usart.h"
#include "cmsis_os.h"
#include "drv_uart.h"

#define UART_DEVICE huart2

#define ARGC_MAX 8
#define BUFFER_SIZE 40
#define SHELL_FUNC_LIST_MAX_SIZE 64

typedef uint8_t (* drv_shell_transmit_t)(char * pData, uint16_t size);
typedef uint8_t (* drv_shell_receive_t)(char * pData, uint16_t size);


typedef struct drv_shell_struct
{
	drv_shell_transmit_t drv_shell_transmit;
	drv_shell_receive_t drv_shell_receive;
} drv_shell_t;

struct h_shell_struct;

typedef struct{
	char c;
	int (* func)(struct h_shell_struct * h_shell,int argc, char ** argv);
	char * description;
} shell_func_t;

typedef struct h_shell_struct{

	UART_HandleTypeDef * huart;
	SemaphoreHandle_t sem_uart_rx;


	int shell_func_list_size;
	shell_func_t shell_func_list[SHELL_FUNC_LIST_MAX_SIZE];
	char print_buffer[BUFFER_SIZE];
	char cmd_buffer[BUFFER_SIZE];
	drv_shell_t drv_shell;
}h_shell_t;


void shell_uart_rx_callback(h_shell_t * h_shell);
void shell_init(h_shell_t * h_shell);
int shell_add(h_shell_t * h_shell, char c, int (* pfunc)(h_shell_t * h_shell,int argc, char ** argv), char * description);
int shell_run(h_shell_t * h_shell);


#endif /* SHELL_H_ */
