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
#include "dma.h"
#include "i2c.h"
#include "sai.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include <stdio.h>
#include "shell.h"
#include "drv_uart.h"
#include "drv_led.h"
#include "sgtl5000.h"



#define CODEC_ADDR 0x14



int __io_putchar(int chr){
	HAL_UART_Transmit(&huart2, (uint8_t*)&chr, 1, HAL_MAX_DELAY);
	return chr;
}


h_shell_t h_shell =
{
		.huart = &huart2,
		.drv_shell = {
				.drv_shell_transmit = drv_uart_transmit,
				.drv_shell_receive = drv_uart_receive
		},
};

h_sgtl5000_t h_sgtl5000 = {
	.hi2c = &hi2c2,
	.hsai_tx = &hsai_BlockA2,
	.hsai_rx = &hsai_BlockB2,
	.dev_address = CODEC_ADDR
};


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	  /* Prevent unused argument(s) compilation warning */
	  if(huart->Instance == USART2){
		  shell_uart_rx_callback(&h_shell);
		  //xSemaphoreTake(h_shell.sem_uart_rx, portMAX_DELAY);
	  }

	  /* NOTE: This function should not be modified, when the callback is needed,
	           the HAL_UART_TxHalfCpltCallback can be implemented in the user file.
	   */
}

void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    extern h_sgtl5000_t h_sgtl5000;
    // Remplit la première moitié du buffer
    sgtl5000_triangle(&h_sgtl5000, 440.0f,0);    // 440 Hz
}

void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
    extern h_sgtl5000_t h_sgtl5000;
    // Remplit la seconde moitié du buffer
    sgtl5000_triangle(&h_sgtl5000, 440.0f,1);
}





int fonction(h_shell_t * h_shell,int argc, char ** argv)
{
	printf("Je suis une fonction bidon\r\n");
	printf("argc = %d\r\n", argc);
	for (int i = 0 ; i<argc ; i++){
		printf("argv[%d] = %s\r\n", i, argv[i]);
	}

	return 0;
}

int addition(h_shell_t * h_shell,int argc, char ** argv){
	int sum = 0;
	for (int i = 0 ; i<argc ; i++){
		sum = sum + atoi(argv[i]);
	}
	printf("sum = %d\r\n" , sum);
	return 0;
}

int afficheStats(h_shell_t * h_shell,int argc, char ** argv){
	char * buf = 0;
	vTaskGetRunTimeStats(buf);
	return 0;
}


void task_shell(void* unused){
	MCP23S17_Init();
	//drv_MCP23S17_WriteRegister(0x00,0xFF);
	shell_init(&h_shell);
	shell_add(&h_shell,'f', fonction, "Une fonction inutile");
	shell_add(&h_shell,'a', addition, "Une addition");
	shell_add(&h_shell,'p', afficheStats, "Un affichage des stats");
	shell_run(&h_shell);
  }

void task_led(void * unused){
	for(;;){

		//uint8_t data[2];
		//HAL_I2C_Mem_Read(&hi2c2,CODEC_ADDR,0,I2C_MEMADD_SIZE_16BIT,data,2,100);
		//printf("la valeur est 0x%02X\r\n",data);
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		vTaskDelay(250);

	}
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName){
	Error_Handler();
}

void configureTimerForRunTimeStats(void){
	HAL_TIM_Base_Start(&htim2);
}

unsigned long getRunTimeCounterValue(void){
	static unsigned long value = 0;
	static unsigned long counter_prev = 0;
	unsigned long counter = __HAL_TIM_GET_COUNTER(&htim2);
	if (counter < counter_prev){
		value += 65535;
	}
	counter_prev = counter;
	return value + counter;
}
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_SPI3_Init();
  MX_I2C2_Init();
  MX_SAI2_Init();
  /* USER CODE BEGIN 2 */


	if(xTaskCreate(task_led,"LED",128,NULL,2,NULL) != pdPASS){
		printf("Error creating task LED \r\n");
		Error_Handler();
	}

	if (xTaskCreate(task_shell,"SHELL",512,NULL,1,NULL) != pdPASS){
		  printf("Error creating task shell\r\n");
		  Error_Handler();
	}

	sgtl5000_init(&h_sgtl5000);
	sgtl5000_start(&h_sgtl5000);

	vTaskStartScheduler();
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
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

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
  PeriphClkInit.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 13;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV17;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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

#ifdef  USE_FULL_ASSERT
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
