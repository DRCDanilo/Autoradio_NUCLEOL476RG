/*
 * drv_led.c
 *
 *  Created on: Nov 21, 2025
 *      Author: david
 */

#include "gpio.h"
#include "drv_led.h"

void drv_MCP23S17_WriteRegister(uint8_t reg, uint8_t value)
{
    uint8_t data[3] = {MCP23S17_OPCODE_WRITE, reg, value};

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET); // CS LOW
    HAL_SPI_Transmit(&hspi3, data, 3, 100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET); // CS HIGH
}

void MCP23S17_Init(void)
{
    // Mettre PORTA en sortie
    drv_MCP23S17_WriteRegister(0x00, 0xFF);
    // Mettre PORTB en sortie
    drv_MCP23S17_WriteRegister(0x01, 0xFF);
}


