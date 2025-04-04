/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
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
#include <stdio.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define RADIO_MODE_STANDBY_RC        0x02
#define RADIO_MODE_TX                0x06
#define RADIO_COMMAND_TX_DONE        0x06

#define RADIO_MODE_BITFIELD          0x70
#define RADIO_STATUS_BITFIELD        0x0E
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SUBGHZ_HandleTypeDef hsubghz;

/* USER CODE BEGIN PV */
uint8_t RadioBufferAddr[2] = {0x80, 0x00};
uint8_t RadioTxData[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
uint8_t RadioRxData[5];
uint8_t RadioPacketType[1] = {0x01};
uint8_t RadioPacketParams[9] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00};
uint8_t RadioSyncr[3] = {0x07, 0x40, 0x01};
uint8_t RadioFrequency[4] = {0x34, 0x03, 0x00, 0x9F};	// 915MHz
uint8_t RadioPA[4] = {0x07, 0x00, 0x01, 0x01};
uint8_t RadioTxPA[2] = {0x0E, 0x07};
uint8_t LoRaMod[4] = {0x07, 0x04, 0x00, 0x01};	// Spreading factor 7, Bandwidth 125kHz
uint8_t RadioConfigIRQ[8] = {0x02, 0x03, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00};
uint8_t RadioGetIRQ[3] = {0x00, 0x00, 0x00};
uint8_t RadioClrIRQ[2] = {0x01, 0x02};
uint8_t RadioGetData[2] = {0x00, 0x00};

uint8_t RadioCmd[3] = {0x00, 0x00, 0x00};
uint8_t RadioResult = 0x00;
uint8_t RadioParam  = 0x00;
uint8_t RadioMode   = 0x00;
uint8_t RadioStatus = 0x00;
uint8_t IRQStatus = 0x00;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_SUBGHZ_Init(void);
void InitTx(void);
void InitRx(void);
void InfiniteTx(void);
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

  /* USER CODE BEGIN SysInit */

  /* Configure LED2 & LED3 */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_SUBGHZ_Init();
  /* USER CODE BEGIN 2 */

  /*## 1 - Wakeup the SUBGHZ Radio ###########################################*/

  /* Retrieve Status from SUBGHZ Radio */
  if (HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &RadioResult, 1) != HAL_OK)
    Error_Handler();
  else
  {
    /* Format Mode and Status receive from SUBGHZ Radio */
    RadioMode   = ((RadioResult & RADIO_MODE_BITFIELD) >> 4); 
    
    /* Check if SUBGHZ Radio is in RADIO_MODE_STANDBY_RC mode */
    if(RadioMode != RADIO_MODE_STANDBY_RC)
      Error_Handler();
  }

  InitTx();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  InfiniteTx();

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3|RCC_CLOCKTYPE_HCLK
                              |RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SUBGHZ Initialization Function
  * @param None
  * @retval None
  */
static void MX_SUBGHZ_Init(void)
{

  /* USER CODE BEGIN SUBGHZ_Init 0 */

  /* USER CODE END SUBGHZ_Init 0 */

  /* USER CODE BEGIN SUBGHZ_Init 1 */

  /* USER CODE END SUBGHZ_Init 1 */
  hsubghz.Init.BaudratePrescaler = SUBGHZSPI_BAUDRATEPRESCALER_8;
  if (HAL_SUBGHZ_Init(&hsubghz) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SUBGHZ_Init 2 */

  /* USER CODE END SUBGHZ_Init 2 */

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
  while(1)
  {
    printf("Error");
    HAL_Delay(500);
  }
  /* USER CODE END Error_Handler_Debug */
}

void InitTx(void)
{
  //Step 1
  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_BUFFERBASEADDRESS, RadioBufferAddr, 2) != HAL_OK)
	  Error_Handler();
  //Step 2
  if (HAL_SUBGHZ_WriteBuffer(&hsubghz, 0x80, RadioTxData, 5) != HAL_OK)
	  Error_Handler();
  //Step 3
  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_PACKETTYPE, RadioPacketType, 1) != HAL_OK)
	  Error_Handler();
  //Step 3.5 - Verify packet type
  if (HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_PACKETTYPE, RadioGetData, 2) != HAL_OK)
	  Error_Handler();
  //Step 4
  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_PACKETPARAMS, RadioPacketParams, 9) != HAL_OK)
	  Error_Handler();
  //Step 5
  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, SUBGHZ_RADIO_WRITE_REGISTER, RadioSyncr, 3) != HAL_OK)
	  Error_Handler();
  //Step 6
  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_RFFREQUENCY, RadioFrequency, 4) != HAL_OK)
	  Error_Handler();
  //Step 7
  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_PACONFIG, RadioPA, 4) != HAL_OK)
	  Error_Handler();
  //Step 8
  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_TXPARAMS, RadioTxPA, 2) != HAL_OK)
	  Error_Handler();
  //Step 9
  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_MODULATIONPARAMS, LoRaMod, 4) != HAL_OK)
	  Error_Handler();
  //Step 10
  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_CFG_DIOIRQ, RadioConfigIRQ, 8) != HAL_OK)
	  Error_Handler();
  //Step 11
  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_TX, RadioCmd, 3) != HAL_OK)
	  // After the transmission is finished, the sub-GHZ radio enters automatically the Standby mode
	  Error_Handler();
  //Step 11.5 - Check Status
  RadioResult = 0x00;
  if (HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &RadioResult, 1) != HAL_OK)
	Error_Handler();

  RadioMode   = ((RadioResult & RADIO_MODE_BITFIELD) >> 4);
  RadioStatus = ((RadioResult & RADIO_STATUS_BITFIELD) >> 1);

  if (RadioMode == RADIO_MODE_TX)
	{
	  /* Wait end of transfer. SUBGHZ Radio go in Standby Mode */
	  do
	  {
		RadioResult = 0x00;
		if (HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &RadioResult, 1) != HAL_OK)
		  Error_Handler();

		RadioMode   = ((RadioResult & RADIO_MODE_BITFIELD) >> 4);
		RadioStatus = ((RadioResult & RADIO_STATUS_BITFIELD) >> 1);
	  }
	  while (RadioMode != RADIO_MODE_STANDBY_RC);
	}
	else
	{
	  Error_Handler();
	}

  //Step 12
  IRQStatus = 0x00;
  if (HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_IRQSTATUS, RadioGetIRQ, 3) != HAL_OK)
	  Error_Handler();

  IRQStatus = RadioGetIRQ[1];

  if (IRQStatus)
	  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_CLR_IRQSTATUS, RadioClrIRQ, 2) != HAL_OK)
		  Error_Handler();
}

void InitRx(void)
{
	//Step 1
	if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_BUFFERBASEADDRESS, RadioBufferAddr, 2) != HAL_OK)
	  Error_Handler();
	//Step 2
	if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_PACKETTYPE, RadioPacketType, 1) != HAL_OK)
	  Error_Handler();
	//Step 2.5 - Verify packet type
	if (HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_PACKETTYPE, RadioGetData, 2) != HAL_OK)
	  Error_Handler();
	//Step 3
	if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_PACKETPARAMS, RadioPacketParams, 9) != HAL_OK)
	  Error_Handler();
	//Step 4
	if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, SUBGHZ_RADIO_WRITE_REGISTER, RadioSyncr, 3) != HAL_OK)
	  Error_Handler();
	//Step 5
	if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_RFFREQUENCY, RadioFrequency, 4) != HAL_OK)
	  Error_Handler();
	//Step 6
	if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_MODULATIONPARAMS, LoRaMod, 4) != HAL_OK)
		Error_Handler();
	//Step 7
	if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_CFG_DIOIRQ, RadioConfigIRQ, 8) != HAL_OK)
		Error_Handler();
	//Step 8
	if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_RX, RadioCmd, 3) != HAL_OK)
		Error_Handler();
	do
	{
		//Step 8.5
		RadioResult = 0x00;
		if (HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &RadioResult, 1) != HAL_OK)
			Error_Handler();
		RadioMode   = ((RadioResult & RADIO_MODE_BITFIELD) >> 4);
		RadioStatus = ((RadioResult & RADIO_STATUS_BITFIELD) >> 1);
		//Step 9
		if (HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_IRQSTATUS, RadioGetIRQ, 3) != HAL_OK)
				Error_Handler();

		IRQStatus = RadioGetIRQ[1];
	}
	while (!RadioGetIRQ[0] || !RadioGetIRQ[1] || !RadioGetIRQ[2]);

	// Get stats of the receive
	if (HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_RXBUFFERSTATUS, RadioCmd, 3) != HAL_OK)
		Error_Handler();

	// Get receive
	if (HAL_SUBGHZ_ReadBuffer(&hsubghz, 0x00, RadioRxData, 5) != HAL_OK)
		Error_Handler();

	//Step 11
	if (IRQStatus)
		if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_CLR_IRQSTATUS, RadioClrIRQ, 2) != HAL_OK)
			Error_Handler();
}

void InfiniteTx(void)
{
  // Infinite sending
  if (HAL_SUBGHZ_WriteBuffer(&hsubghz, 0x80, RadioTxData, 5) != HAL_OK)
      Error_Handler();
  if (HAL_SUBGHZ_ReadBuffer(&hsubghz, 0x80, RadioRxData, 5) != HAL_OK)
	  Error_Handler();
  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_TX, RadioCmd, 3) != HAL_OK)
  // After the transmission is finished, the sub-GHZ radio enters automatically the Standby mode
	  Error_Handler();

  RadioResult = 0x00;
	if (HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &RadioResult, 1) != HAL_OK)
	  Error_Handler();
	RadioMode   = ((RadioResult & RADIO_MODE_BITFIELD) >> 4);
	RadioStatus = ((RadioResult & RADIO_STATUS_BITFIELD) >> 1);

	if (RadioMode == RADIO_MODE_TX)
	  {
		/* Wait end of transfer. SUBGHZ Radio go in Standby Mode */
		do
		{
		  RadioResult = 0x00;
		  if (HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &RadioResult, 1) != HAL_OK)
			Error_Handler();

		  RadioMode   = ((RadioResult & RADIO_MODE_BITFIELD) >> 4);
		  RadioStatus = ((RadioResult & RADIO_STATUS_BITFIELD) >> 1);
		}
		while (RadioMode != RADIO_MODE_STANDBY_RC);
	  }
	  else if (RadioMode != RADIO_MODE_STANDBY_RC)
		Error_Handler();

  IRQStatus = 0x00;
	if (HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_IRQSTATUS, RadioGetIRQ, 3) != HAL_OK)
	  Error_Handler();
	IRQStatus = RadioGetIRQ[1];

	if (IRQStatus)
	  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_CLR_IRQSTATUS, RadioClrIRQ, 2) != HAL_OK)
		  Error_Handler();
	  memset(RadioGetIRQ, 0, sizeof(RadioGetIRQ));
	  if (HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_IRQSTATUS, RadioGetIRQ, 3) != HAL_OK)
		  Error_Handler();

	memset(RadioRxData, 0, sizeof(RadioRxData));
	if (HAL_SUBGHZ_WriteBuffer(&hsubghz, 0x80, RadioRxData, 5) != HAL_OK)
	      Error_Handler();
	if (HAL_SUBGHZ_ReadBuffer(&hsubghz, 0x80, RadioRxData, 5) != HAL_OK)
		  Error_Handler();
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
