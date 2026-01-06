/*
 * drv_led.h
 *
 *  Created on: Nov 21, 2025
 *      Author: david
 */

#ifndef DRV_LED_H_
#define DRV_LED_H_

#define LED0 0xFE
#define LED1 0xFD
#define LED2 0xFB
#define LED3 0xF7
#define LED4 0xEF
#define LED5 0xDF
#define LED6 0xBF
#define LED7 0x7F

#define MCP23S17_OPCODE_WRITE 0x40

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


extern SPI_HandleTypeDef hspi3;

typedef struct drv_led_struct{
	uint8_t * registered_valueA;
	uint8_t * registered_valueB;

}drv_led_t;

void drv_MCP23S17_WriteRegister(uint8_t reg, uint8_t value);
void MCP23S17_Init(void);


#endif /* DRV_LED_H_ */
