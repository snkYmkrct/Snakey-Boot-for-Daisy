/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    quadspi.c
  * @brief   This file provides code for the configuration
  *          of the QUADSPI instances.
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
#include "quadspi.h"

/* USER CODE BEGIN 0 */
#include "flash_IS25LP064A.h"

static uint8_t QSPI_WriteEnable(void);
static uint8_t QSPI_AutoPollingMemReady(void);
static uint8_t QSPI_Configuration(void);
static uint8_t QSPI_ResetChip(void);

static uint8_t qspi_enabled = 0;

/* USER CODE END 0 */

QSPI_HandleTypeDef hqspi;

/* QUADSPI init function */
void MX_QUADSPI_Init(void)
{

  /* USER CODE BEGIN QUADSPI_Init 0 */

  /* USER CODE END QUADSPI_Init 0 */

  /* USER CODE BEGIN QUADSPI_Init 1 */

  /* USER CODE END QUADSPI_Init 1 */
  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 1;
  hqspi.Init.FifoThreshold = 1;
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;
  hqspi.Init.FlashSize = 22;
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  hqspi.Init.FlashID = QSPI_FLASH_ID_1;
  hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN QUADSPI_Init 2 */

  /* USER CODE END QUADSPI_Init 2 */

}

void HAL_QSPI_MspInit(QSPI_HandleTypeDef* qspiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(qspiHandle->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspInit 0 */

  /* USER CODE END QUADSPI_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_QSPI;
    PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_PLL;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* QUADSPI clock enable */
    __HAL_RCC_QSPI_CLK_ENABLE();

    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    /**QUADSPI GPIO Configuration
    PG6     ------> QUADSPI_BK1_NCS
    PF7     ------> QUADSPI_BK1_IO2
    PF6     ------> QUADSPI_BK1_IO3
    PF10     ------> QUADSPI_CLK
    PF9     ------> QUADSPI_BK1_IO1
    PF8     ------> QUADSPI_BK1_IO0
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /* USER CODE BEGIN QUADSPI_MspInit 1 */

  /* USER CODE END QUADSPI_MspInit 1 */
  }
}

void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef* qspiHandle)
{

  if(qspiHandle->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspDeInit 0 */

  /* USER CODE END QUADSPI_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_QSPI_CLK_DISABLE();

    /**QUADSPI GPIO Configuration
    PG6     ------> QUADSPI_BK1_NCS
    PF7     ------> QUADSPI_BK1_IO2
    PF6     ------> QUADSPI_BK1_IO3
    PF10     ------> QUADSPI_CLK
    PF9     ------> QUADSPI_BK1_IO1
    PF8     ------> QUADSPI_BK1_IO0
    */
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_10|GPIO_PIN_9
                          |GPIO_PIN_8);

  /* USER CODE BEGIN QUADSPI_MspDeInit 1 */

  /* USER CODE END QUADSPI_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */


/* QUADSPI init function */
uint8_t CSP_QUADSPI_Init(void) {

	hqspi.Instance = QUADSPI;
    if (HAL_QSPI_DeInit(&hqspi) != HAL_OK) {
        return HAL_ERROR;
    }

    MX_QUADSPI_Init();

    if (QSPI_ResetChip() != HAL_OK) {
        return HAL_ERROR;
    }

    HAL_Delay(1);

    if (QSPI_AutoPollingMemReady() != HAL_OK) {
        return HAL_ERROR;
    }

    if (QSPI_WriteEnable() != HAL_OK) {

        return HAL_ERROR;
    }

    if (QSPI_Configuration() != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

uint8_t QSPI_ResetChip() {
    QSPI_CommandTypeDef sCommand;

    /* Erasing Sequence -------------------------------------------------- */
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    sCommand.Instruction = RESET_ENABLE_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.Address = 0;
    sCommand.DataMode = QSPI_DATA_NONE;
    sCommand.DummyCycles = 0;

    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
        != HAL_OK) {
        return HAL_ERROR;
    }

    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    sCommand.Instruction = RESET_MEMORY_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.Address = 0;
    sCommand.DataMode = QSPI_DATA_NONE;
    sCommand.DummyCycles = 0;

    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
        != HAL_OK) {
        return HAL_ERROR;
    }
    return HAL_OK;
}

uint8_t QSPI_AutoPollingMemReady(void) {

    QSPI_CommandTypeDef sCommand;
    QSPI_AutoPollingTypeDef sConfig;

    /* Configure automatic polling mode to wait for memory ready ------ */
    if (qspi_enabled){
    	sCommand.InstructionMode = QSPI_INSTRUCTION_4_LINES;
        sCommand.DataMode = QSPI_DATA_4_LINES;
    }
    else {
    	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
        sCommand.DataMode = QSPI_DATA_1_LINE;
    }
    sCommand.Instruction = READ_STATUS_REG_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DummyCycles = 0;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    sConfig.Match = 0x00;
    sConfig.Mask = IS25LP064A_SR_WIP;
    sConfig.MatchMode = QSPI_MATCH_MODE_AND;
    sConfig.StatusBytesSize = 1;
    sConfig.Interval = 0x10;
    sConfig.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_QSPI_AutoPolling(&hqspi, &sCommand, &sConfig,
                             HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

static uint8_t QSPI_WriteEnable(void) {
    QSPI_CommandTypeDef sCommand;
    QSPI_AutoPollingTypeDef sConfig;

    /* Enable write operations ------------------------------------------ */
    if (qspi_enabled){
    	sCommand.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    }
    else {
    	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    }
    sCommand.Instruction = WRITE_ENABLE_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_NONE;
    sCommand.DummyCycles = 0;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
        != HAL_OK) {
        return HAL_ERROR;
    }

    /* Configure automatic polling mode to wait for write enabling ---- */
    sConfig.Match = IS25LP064A_SR_WREN;
    sConfig.Mask = IS25LP064A_SR_WREN;
    sConfig.MatchMode = QSPI_MATCH_MODE_AND;
    sConfig.StatusBytesSize = 1;
    sConfig.Interval = 0x10;
    sConfig.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    sCommand.Instruction = READ_STATUS_REG_CMD;
    if (qspi_enabled){
    	sCommand.DataMode = QSPI_DATA_4_LINES;
    }
    else {
    	sCommand.DataMode = QSPI_DATA_1_LINE;
    }
    if (HAL_QSPI_AutoPolling(&hqspi, &sCommand, &sConfig,
                             HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

/*Enable quad mode and set dummy cycles count*/
uint8_t QSPI_Configuration(void) {

    QSPI_CommandTypeDef sCommand;
    QSPI_AutoPollingTypeDef sConfig;
    uint8_t reg;

    /*----- Setting the dummy cycles ----*/
    if (QSPI_WriteEnable() != HAL_OK) {

        return HAL_ERROR;
    }

    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    sCommand.Instruction = WRITE_READ_PARAM_REG_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.DataMode = QSPI_DATA_1_LINE;
    sCommand.DummyCycles = 0;
    sCommand.NbData = 1;

    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
        != HAL_OK) {
        return HAL_ERROR;
    }

    /* Setting in Read Register P4P3 bits as 10, so full reg = 11110000 */
    reg = 0xF0;
    if (HAL_QSPI_Transmit(&hqspi, &reg,
                          HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        Error_Handler();
        return HAL_ERROR;
    }

    if (QSPI_AutoPollingMemReady() != HAL_OK) {
        return HAL_ERROR;
    }


    /*----- Setting the QSPI mode ----*/

    /* enable quad mode */
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = WRITE_STATUS_REG_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_1_LINE;
    sCommand.DummyCycles = 0;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    sCommand.NbData = 1;

    if (QSPI_WriteEnable() != HAL_OK) {
        return HAL_ERROR;
    }

    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
        != HAL_OK) {
        return HAL_ERROR;
    }

    reg = IS25LP064A_SR_QE;
    if (HAL_QSPI_Transmit(&hqspi, &reg,
                          HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        Error_Handler();
        return HAL_ERROR;
    }
    /* Configure automatic polling mode to wait for quad enable complete */
    sConfig.Match = IS25LP064A_SR_QE;
    sConfig.Mask = IS25LP064A_SR_QE;
    sConfig.MatchMode = QSPI_MATCH_MODE_AND;
    sConfig.StatusBytesSize = 1;
    sConfig.Interval = 0x10;
    sConfig.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    sCommand.Instruction = READ_STATUS_REG_CMD;
    sCommand.DataMode = QSPI_DATA_1_LINE;

    if (HAL_QSPI_AutoPolling(&hqspi, &sCommand, &sConfig,
                             HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return HAL_ERROR;
    }

    if (QSPI_AutoPollingMemReady() != HAL_OK) {
        return HAL_ERROR;
    }

    /* enter qpi mode */
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = ENTER_QUAD_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_NONE;
    sCommand.DummyCycles = 0;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    sCommand.NbData = 0;

    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
        != HAL_OK) {
        return HAL_ERROR;
    }
    qspi_enabled = 1;  /* qpi mode ON */

    return HAL_OK;
}

uint8_t CSP_QSPI_Erase_Chip(void) {
    QSPI_CommandTypeDef sCommand;

    if (QSPI_WriteEnable() != HAL_OK) {
        return HAL_ERROR;
    }

    sCommand.Instruction = CHIP_ERASE_CMD;
    sCommand.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.Address = 0;
    sCommand.DataMode = QSPI_DATA_NONE;
    sCommand.DummyCycles = 0;


    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
        != HAL_OK) {
        return HAL_ERROR;
    }

    if (QSPI_AutoPollingMemReady() != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

uint8_t CSP_QSPI_EraseSector(uint32_t EraseStartAddress, uint32_t EraseEndAddress) {

    QSPI_CommandTypeDef sCommand;

    /* Erasing Sequence -------------------------------------------------- */
    sCommand.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    sCommand.Instruction = SECTOR_ERASE_QPI_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_4_LINES;

    sCommand.DataMode = QSPI_DATA_NONE;
    sCommand.DummyCycles = 0;

    EraseStartAddress = EraseStartAddress
                        - EraseStartAddress % IS25LP064A_SECTOR_SIZE;

    while (EraseEndAddress >= EraseStartAddress) {
        sCommand.Address = (EraseStartAddress & 0x7FFFFF);

        if (QSPI_WriteEnable() != HAL_OK) {
            return HAL_ERROR;
        }

        if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
            != HAL_OK) {
            return HAL_ERROR;
        }
        EraseStartAddress += IS25LP064A_SECTOR_SIZE;

        if (QSPI_AutoPollingMemReady() != HAL_OK) {
            return HAL_ERROR;
        }
    }
    return HAL_OK;
}


uint8_t
CSP_QSPI_EnableMemoryMappedMode(void) {

    QSPI_CommandTypeDef sCommand;
    QSPI_MemoryMappedTypeDef sMemMappedCfg;

    /* Enable Memory-Mapped mode-------------------------------------------------- */

    sCommand.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    sCommand.Instruction = QUAD_INOUT_FAST_READ_CMD;
    sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
    sCommand.AddressMode = QSPI_ADDRESS_4_LINES;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.DataMode = QSPI_DATA_4_LINES;
    sCommand.NbData = 0;
    sCommand.Address = 0;

    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    //sCommand.AlternateByteMode  = QSPI_ALTERNATE_BYTES_4_LINES;
    //sCommand.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
    //sCommand.AlternateBytes = 0x000000A0;

    sCommand.DummyCycles = IS25LP064A_DUMMY_CYCLES_READ_QUAD;
    //sCommand.DummyCycles = 6;

    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    //sCommand.SIOOMode = QSPI_SIOO_INST_ONLY_FIRST_CMD;


    sMemMappedCfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;

    if (HAL_QSPI_MemoryMapped(&hqspi, &sCommand, &sMemMappedCfg) != HAL_OK) {
        return HAL_ERROR;
    }
    return HAL_OK;
}

uint8_t
CSP_QSPI_WriteMemory(uint8_t* buffer, uint32_t address, uint32_t buffer_size) {

    QSPI_CommandTypeDef sCommand;
    uint32_t end_addr, current_size, current_addr;

    /* Calculation of the size between the write address and the end of the page */
    current_addr = 0;


    //
    while (current_addr <= address) {
        current_addr += MEMORY_PAGE_SIZE;
    }
    current_size = current_addr - address;

    /* Check if the size of the data is less than the remaining place in the page */
    if (current_size > buffer_size) {
        current_size = buffer_size;
    }

    /* Initialize the adress variables */
    current_addr = address;
    end_addr = address + buffer_size;

    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressSize = QSPI_ADDRESS_32_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    sCommand.Instruction = QUAD_IN_FAST_PROG_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
    sCommand.DataMode = QSPI_DATA_4_LINES;
    sCommand.NbData = buffer_size;
    sCommand.Address = address;
    sCommand.DummyCycles = 0;

    /* Perform the write page by page */
    do {
        sCommand.Address = current_addr;
        sCommand.NbData = current_size;

        if (current_size == 0) {
            return HAL_OK;
        }

        /* Enable write operations */
        if (QSPI_WriteEnable() != HAL_OK) {
            return HAL_ERROR;
        }

        /* Configure the command */
        if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
            != HAL_OK) {

            return HAL_ERROR;
        }

        /* Transmission of the data */
        if (HAL_QSPI_Transmit(&hqspi, buffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {

            return HAL_ERROR;
        }

        /* Configure automatic polling mode to wait for end of program */
        if (QSPI_AutoPollingMemReady() != HAL_OK) {
            return HAL_ERROR;
        }

        /* Update the address and size variables for next page programming */
        current_addr += current_size;
        buffer += current_size;
        current_size =
            ((current_addr + MEMORY_PAGE_SIZE) > end_addr) ?
            (end_addr - current_addr) : MEMORY_PAGE_SIZE;
    } while (current_addr <= end_addr);

    return HAL_OK;

}

uint8_t
CSP_QSPI_ReadMemory(uint8_t* buffer, uint32_t address, uint32_t buffer_size) {

    QSPI_CommandTypeDef sCommand;
    uint32_t end_addr, current_size, current_addr;

    /* Calculation of the size between the write address and the end of the page */
    current_addr = 0;


    //
    while (current_addr <= address) {
        current_addr += MEMORY_PAGE_SIZE;
    }
    current_size = current_addr - address;

    /* Check if the size of the data is less than the remaining place in the page */
    if (current_size > buffer_size) {
        current_size = buffer_size;
    }

    /* Initialize the adress variables */
    current_addr = address;
    end_addr = address + buffer_size;

    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressSize = QSPI_ADDRESS_32_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    sCommand.Instruction = QUAD_IN_FAST_PROG_CMD;
    sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
    sCommand.DataMode = QSPI_DATA_4_LINES;
    sCommand.NbData = buffer_size;
    sCommand.Address = address;
    sCommand.DummyCycles = 0;

    /* Perform the write page by page */
    do {
        sCommand.Address = current_addr;
        sCommand.NbData = current_size;

        if (current_size == 0) {
            return HAL_OK;
        }

        /* Enable write operations */
        if (QSPI_WriteEnable() != HAL_OK) {
            return HAL_ERROR;
        }

        /* Configure the command */
        if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
            != HAL_OK) {

            return HAL_ERROR;
        }

        /* Transmission of the data */
        if (HAL_QSPI_Transmit(&hqspi, buffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {

            return HAL_ERROR;
        }

        /* Configure automatic polling mode to wait for end of program */
        if (QSPI_AutoPollingMemReady() != HAL_OK) {
            return HAL_ERROR;
        }

        /* Update the address and size variables for next page programming */
        current_addr += current_size;
        buffer += current_size;
        current_size =
            ((current_addr + MEMORY_PAGE_SIZE) > end_addr) ?
            (end_addr - current_addr) : MEMORY_PAGE_SIZE;
    } while (current_addr <= end_addr);

    return HAL_OK;

}

/* USER CODE END 1 */
