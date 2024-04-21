/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "quadspi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
#include "flash_IS25LP064A.h"  // TODO should not be here, just for testing purposes
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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static uint8_t buffer_test[IS25LP064A_SECTOR_SIZE];
static uint32_t var = 0;
uint8_t read_back[IS25LP064A_SECTOR_SIZE];

char *writebuf = "Hello world from QSPI !";

uint8_t readbuf[100] = {0};

int __io_putchar(int ch) {
	HAL_UART_Transmit(&huart4, (uint8_t*)&ch, 1, HAL_MAX_DELAY);

	return ch;
}

int __io_getchar(void) {
	uint8_t ch = 0;

	HAL_UART_Receive(&huart4, &ch, 1, HAL_MAX_DELAY);

	return ch;
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
/* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

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
  MX_QUADSPI_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */

  // When flashing code over STLink, the code starts running before flashing is done,
  // and the STLink resets the board in the middle of configuration calls, leaving the
  // flash controller hanging, unstable state, if the board was reset in the middle of
  // a flash command, like reset chip (which possibly lasts 45 seconds)
  // 1 second delay stops that from happening
  //HAL_Delay (3000);
  fflush(stdout);
  printf("  HIIIIII no delay ``````  \r\n");

  TEST_QSPI_ExitQPIMODE();
  printf("  after exit qpi \r\n");

  if (CSP_QUADSPI_Init() != HAL_OK)
  {
	  printf("-----> quad spi init error  \r\n");
	  Error_Handler();
  }


  HAL_GPIO_WritePin (GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
  printf("   LED ON before erase   \r\n");
  HAL_Delay (1);

  if (CSP_QSPI_Erase_Chip() != HAL_OK)
  {
	  Error_Handler();
  }
  printf("   erase successful !!!!!!!!!!!!!    \r\n");

/*

  printf("Single write-read test \r\n buffer to write: %s   \r\n", writebuf);
  if (CSP_QSPI_EraseSector(0, IS25LP064A_SECTOR_SIZE-1) != HAL_OK) {
	printf("-----> erase sector error \r\n", var);
	while (1);
  }
  if (CSP_QSPI_Write(writebuf, 0, strlen (writebuf)) != HAL_OK)
  {
	  Error_Handler();
  }
  printf("   written!!!!!   \r\n");

  if (CSP_QSPI_Read(readbuf, 0, strlen (writebuf)) != HAL_OK)
  {
	  printf("-----> read  error  \r\n");
	  Error_Handler();
  }

  printf("read: ***%s***  pew pew \r\n", readbuf);
  printf("and mew\r\n\r\n");

  if (CSP_QSPI_EnableMemoryMappedMode() != HAL_OK) {
	printf("-----> enable memory mapped mode error \r\n");
	while (1);
  }
  memcpy(read_back, (uint8_t*) (0x90000000), strlen (writebuf));
  printf("read mapped mode: ***%s***  pew pew \r\n", read_back);

  if (CSP_QSPI_DisableMemoryMappedMode() != HAL_OK) {
	printf("-----> disable memory mapped mode error \r\n");
	while (1);
  }
*/


/*  if (TEST_QSPI_ExitQPIMODE() != HAL_OK)
  {
	  printf("-----> exit quad spi  error  \r\n");
	  Error_Handler();
  }
  printf("  and another exit qpi \r\n");
 */


    printf(" ~~~~~    starting write --- read test  \r\n");

	for (var = 0; var < IS25LP064A_SECTOR_SIZE; var++) {
		buffer_test[var] = (var & 0xff);
	}

	for (var = 0; var < IS25LP064A_SECTOR_COUNT; var++) {
		if (CSP_QSPI_EraseSector(var * IS25LP064A_SECTOR_SIZE, (var + 1) * IS25LP064A_SECTOR_SIZE - 1) != HAL_OK) {
			printf("-----> erase sector error  var = %d \r\n", var);

			while (1)
				;  //breakpoint - error detected
		}
		else {
			printf("erased sector %d  oooookay \r\n", var);
		}
		if (CSP_QSPI_Write(buffer_test, var * IS25LP064A_SECTOR_SIZE, IS25LP064A_SECTOR_SIZE) != HAL_OK) {
			printf("-----> write sector error  var = %d \r\n", var);
			while (1)
				;  //breakpoint - error detected
		}
		else {
			printf("written sector %d  oooookay \r\n", var);
		}
	}
	for (var = 0; var < IS25LP064A_SECTOR_COUNT; var++) {
		memset(read_back, 0, IS25LP064A_SECTOR_SIZE);
		if (CSP_QSPI_Read(read_back, var * IS25LP064A_SECTOR_SIZE, IS25LP064A_SECTOR_SIZE) != HAL_OK) {
			printf("-----> read sector error in non mapped mode var = %d \r\n", var);
			while (1);
		}
		if (memcmp(buffer_test, read_back, IS25LP064A_SECTOR_SIZE) != HAL_OK) {
			printf("-----> read sector wrong in non mapped mode  var = %d \r\n", var);
			while (1)
				;  //breakpoint - error detected - otherwise QSPI works properly
		}
		else {
			printf("read sector %d  oooookay \r\n", var);
		}
	}

	if (CSP_QSPI_EnableMemoryMappedMode() != HAL_OK) {
		printf("-----> enable memory mapped mode error \r\n");
		while (1)
			; //breakpoint - error detected
	}
	for (var = 0; var < IS25LP064A_SECTOR_COUNT-1; var++) {
		memset(read_back, 0, IS25LP064A_SECTOR_SIZE);
		memcpy(read_back, (uint8_t*) (0x90000000 + var * IS25LP064A_SECTOR_SIZE), IS25LP064A_SECTOR_SIZE);
		if (memcmp(buffer_test, read_back, IS25LP064A_SECTOR_SIZE) != HAL_OK) {
			printf("-----> read sector wrong in mapped  var = %d \r\n", var);
			while (1)
				;  //breakpoint - error detected - otherwise QSPI works properly
		}
		else {
			printf("read sector %d  oooookay \r\n", var);
		}
	}

	printf(" ~!~!~!~!~    SUCCESS write --- read test  \r\n");


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //HAL_GPIO_TogglePin (GPIOC, GPIO_PIN_7); GPIO_PIN_RESET
	  HAL_GPIO_WritePin (GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	  HAL_Delay (1000);   /* Insert delay */
	  HAL_GPIO_WritePin (GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	  HAL_Delay (200);   /* Insert delay */
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
