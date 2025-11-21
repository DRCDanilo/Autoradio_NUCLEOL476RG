/*
 * shell.h
 *
 *  Created on: 7 juin 2019
 *      Author: laurent
 */

#ifndef INC_LIB_SHELL_SHELL_H_
#define INC_LIB_SHELL_SHELL_H_

#include <stdint.h>

#define UART_DEVICE huart2

#define ARGC_MAX 8
#define BUFFER_SIZE 40
#define SHELL_FUNC_LIST_MAX_SIZE 64

//Struc for the functions
typedef struct{
	char c;
	int (* func)(int argc, char ** argv);
	char * description;
} shell_func_t;

//Structure for driver
typedef struct h_shell_structure
{
	int func_list_size;
	shell_func_t func_list[SHELL_FUNC_LIST_MAX_SIZE];
	char print_buffer[BUFFER_SIZE];
	char cmd_buffer[BUFFER_SIZE];

}h_shell_t;

void shell_init(h_shell_t * h_shell);
int shell_add(h_shell_t * h_shell, char c, int (* pfunc)(int argc, char ** argv), char * description);
int shell_run(h_shell_t * h_shell);



#endif /* INC_LIB_SHELL_SHELL_H_ */
