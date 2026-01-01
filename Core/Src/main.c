/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "shell.h" //Include the shell file
#include <stdio.h> //Include to work with printf function
#include <stdlib.h> //Include to use atoi function
#include "drv_uart.h" //Include UART dirver
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define STACK_DEPTH 256

#define GPIO_EXPANDER_ADDRESS 0b01000000 //Address and LSB is '0' for SPI write operation
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
TaskHandle_t h_task_shell; //handler for the shell task
h_shell_t h_shell;
shell_func_pointer_t pfunc;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int chr) //Function to use the printf function to send with UART
{
	HAL_UART_Transmit(&huart2, (uint8_t*)&chr, 1, HAL_MAX_DELAY);
	return chr;
}

int fonction(h_shell_t * h_shell, int argc, char ** argv) //Function of the shell
{

	int size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "Je suis une fonction bidon\r\n");
	h_shell->drv.transmit(h_shell->print_buffer, size);

	size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "argc : %d\r\n", argc);
	h_shell->drv.transmit(h_shell->print_buffer, size);


	for(int i = 0; i < argc; i++)
	{
		size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "argc[%d] : %s \r\n", i, argv[i]);
		h_shell->drv.transmit(h_shell->print_buffer, size);
	}

	return 0;
}

int add_2_num(h_shell_t * h_shell, int argc, char ** argv)
{
	if(argc == 3)
	{
		int sum = atoi(argv[1]) + atoi(argv[2]);
		//printf("La somme est : %d \r\n", sum);
		int size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "La somme est : %d \r\n", sum);
		h_shell->drv.transmit(h_shell->print_buffer, size);
	}


	return 0;
}

void mcp23s17_init()
{
	write_spi(0x05, 0b10110000);

	write_spi(0x00, 0x00);
	write_spi(0x10, 0x00);


	write_spi(0x0A, 0xFF);
	write_spi(0x1A, 0xFF);
}

void write_spi(uint8_t reg, uint8_t value)
{

	uint8_t tx_data[3] = { GPIO_EXPANDER_ADDRESS, reg, value };
	HAL_GPIO_WritePin(VU_nCS_GPIO_Port, VU_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi3, tx_data, 3, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(VU_nCS_GPIO_Port, VU_nCS_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
}

int led(h_shell_t * h_shell, int argc, char ** argv)
{
	int size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "FUNCION LED\r\n");
	h_shell->drv.transmit(h_shell->print_buffer, size);


	while(1)
	{
		write_spi(0x0A, 0x7E);
		HAL_Delay(1000);
		write_spi(0x0A, 0xFF);
		HAL_Delay(1000);
	}

	return 0;
}





void task_shell(void * unused)
{
	shell_init(&h_shell);
	shell_add(&h_shell, 'f', fonction, "Une fonction inutile");
	shell_add(&h_shell, 'a', add_2_num, "Fonction pour additionner deux nom\r\n");
	shell_add(&h_shell, 'l', led, "Function pour allumer une LED");
	shell_run(&h_shell);
}

//Callback of USART interruption
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	//Here we should unlock the task
	if(huart->Instance == USART2)
	{
		//Function to unlock the task
		shell_uart_rx_callback();
	}

}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_SPI3_Init();
	/* USER CODE BEGIN 2 */
	h_shell.drv.receive = drv_uart_receive;
	h_shell.drv.transmit = drv_uart_transmit;
	mcp23s17_init();

	printf("\r\n===== TEST SHELL =====\r\n");

	if( xTaskCreate(task_shell, "Task Shell", STACK_DEPTH, NULL, 1, &h_task_shell) != pdPASS)
	{
		printf("Error creating the shell task \r\n");
		Error_Handler();
	}

	vTaskStartScheduler();

	/* USER CODE END 2 */

	/* Call init function for freertos objects (in cmsis_os2.c) */
	MX_FREERTOS_Init();

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 10;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
