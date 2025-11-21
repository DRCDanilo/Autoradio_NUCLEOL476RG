/*
 * shell.c
 *
 *  Created on: 7 juin 2019
 *      Author: Laurent Fiack
 */

#include "shell.h"

#include <stdio.h>

#include "usart.h"
#include "gpio.h"

#include "cmsis_os.h" //To include all OS FreeRTOS functions

SemaphoreHandle_t sem_usart = NULL; //Semaphore for the usart

//typedef struct{
//	char c;
//	int (* func)(int argc, char ** argv);
//	char * description;
//} shell_func_t;

static int shell_func_list_size = 0;
static shell_func_t shell_func_list[SHELL_FUNC_LIST_MAX_SIZE];

static char print_buffer[BUFFER_SIZE];

static char uart_read() {
	char c;

	//HAL_UART_Receive(&UART_DEVICE, (uint8_t*)(&c), 1, HAL_MAX_DELAY);//Code to work in regular mode and inside of a task

	//To receive data from interruption
	HAL_UART_Receive_IT(&UART_DEVICE, (uint8_t*)(&c) , 1);
	//Block the task: take an empty semaphore
	xSemaphoreTake(sem_usart, portMAX_DELAY);

	return c;
}

//Function to unlock the task : give the semaphore
void shell_uart_rx_callback(void)
{
	BaseType_t higher_priority_task_woken; //Create argument to give the semaphore
	xSemaphoreGiveFromISR(sem_usart, &higher_priority_task_woken);
	portYIELD_FROM_ISR(higher_priority_task_woken); //Call the scheduler
}


static int uart_write(char * s, uint16_t size) {
	HAL_UART_Transmit(&UART_DEVICE, (uint8_t*)s, size, HAL_MAX_DELAY);
	return size;
}

static int sh_help(h_shell_t * h_shell, int argc, char ** argv) {
	int i;
	for(i = 0 ; i < h_shell->func_list_size ; i++) {
		int size;
		size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "%c: %s\r\n", h_shell->func_list[i].c, h_shell->func_list[i].description);
		uart_write(h_shell->print_buffer, size);
	}

	return 0;
}

void shell_init(h_shell_t * h_shell) {
	int size = 0;
	//Copy the "" text into the print_buffer, with a length lower or equal than BUFFER_SIZE. size has the number of characters in the string created
	size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "\r\n\r\n===== Monsieur Shell v0.2 =====\r\n");
	uart_write(h_shell->print_buffer, size);

	shell_add(&h_shell, 'h', sh_help, "Help");
	sem_usart = xSemaphoreCreateBinary();
}

int shell_add(h_shell_t * h_shell, char c, int (* pfunc)(int argc, char ** argv), char * description) {
	if (h_shell->func_list_size < SHELL_FUNC_LIST_MAX_SIZE) {
		h_shell->func_list[h_shell->func_list_size].c = c;
		h_shell->func_list[h_shell->func_list_size].func = pfunc;
		h_shell->func_list[h_shell->func_list_size].description = description;
		h_shell->func_list_size++;
		return 0;
	}

	return -1;
}

static int shell_exec(h_shell_t * h_shell, char * buf) {
	int i;

	char c = buf[0];

	int argc;
	char * argv[ARGC_MAX];
	char *p;

	for(i = 0 ; i < h_shell->func_list_size ; i++) {
		if (h_shell->func_list[i].c == c) {
			argc = 1;
			argv[0] = buf;

			for(p = buf ; *p != '\0' && argc < ARGC_MAX ; p++){
				if(*p == ' ') {
					*p = '\0';
					argv[argc++] = p+1;
				}
			}

			return h_shell->func_list[i].func(argc, argv);
		}
	}

	int size;
	size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "%c: no such command\r\n", c);
	uart_write(h_shell->print_buffer, size);
	return -1;
}

static char backspace[] = "\b \b";
static char prompt[] = "> ";

int shell_run(h_shell_t * h_shell) {
	int reading = 0;
	int pos = 0;


	//static char cmd_buffer[BUFFER_SIZE]; //Commented because is in the driver structure

	while (1) {
		uart_write(prompt, 2);
		reading = 1;

		while(reading) {
			char c = uart_read();
			int size;

			switch (c) {
			//process RETURN key
			case '\r':
				//case '\n':
				size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "\r\n");
				uart_write(h_shell->print_buffer, size);
				h_shell->cmd_buffer[pos++] = 0;     //add \0 char at end of string
				size = snprintf (h_shell->print_buffer, BUFFER_SIZE, ":%s\r\n", h_shell->cmd_buffer);
				uart_write(h_shell->print_buffer, size);
				reading = 0;        //exit read loop
				pos = 0;            //reset buffer
				break;
				//backspace
			case '\b':
				if (pos > 0) {      //is there a char to delete?
					pos--;          //remove it in buffer

					uart_write(backspace, 3);	// delete the char on the terminal
				}
				break;
				//other characters
			default:
				//only store characters if buffer has space
				if (pos < BUFFER_SIZE) {
					uart_write(&c, 1);
					h_shell->cmd_buffer[pos++] = c; //store
				}
			}
		}
		shell_exec(h_shell, h_shell->cmd_buffer);
	}
	return 0;
}
