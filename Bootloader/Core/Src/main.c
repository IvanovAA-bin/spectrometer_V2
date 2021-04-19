/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "System.h"
#include "ff.h"
#include "IHC.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FLASH_MEM_ADDR 0x08000000U
#define START_PAGE 5U
#define PAGES_TO_ERASE 14U
#define PAGE_SIZE 4096U

#define LED_ON GPIOB->BSRR |= GPIO_BSRR_BR1
#define LED_OFF GPIOB->BSRR |= GPIO_BSRR_BS1
#define LED_REVERSE GPIOB->ODR ^= GPIO_ODR_OD1

#define DELAY_NO_APP_DETECT_ON  100
#define DELAY_NO_APP_DETECT_OFF 100

#define DELAY_USD_ERROR_ON  100
#define DELAY_USD_ERROR_OFF 300

#define DELAY_FLASH_ERASE_ERROR_ON  300
#define DELAY_FLASH_ERASE_ERROR_OFF 100
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t eraseFlash(void)
{
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef EraseInitStruct;
	HAL_FLASHEx_AllowOperation();
	
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Page = START_PAGE;
	EraseInitStruct.NbPages = PAGES_TO_ERASE;
	uint32_t PAGEError = FLASH_PAGE_NB;
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	HAL_FLASHEx_Erase(&EraseInitStruct,&PAGEError);
	//uint32_t i;
	//HAL_FLASH_Unlock();
	//for (i = START_PAGE; i < STOP_PAGE; i++)
		//FLASH_PageErase(FLASH_MEM_ADDR + i * PAGE_SIZE);
	HAL_FLASH_Lock();
	return PAGEError;
}

uint8_t isDataProgrammed(uint32_t req_data, uint32_t got_data)
{
	if (req_data == got_data)
		return 1;
	return 0;
}

void jumpToApplication(void)
{
	uint32_t app_jump_address;

	typedef void(*pFunction)(void);	//объявляем пользовательский тип
	pFunction Jump_To_Application; //и создаём переменную этого типа

	 __disable_irq();//запрещаем прерывания
	SCB->VTOR = (FLASH_MEM_ADDR + START_PAGE * PAGE_SIZE); // Перемещаем таблицу прерываний
	app_jump_address = *( uint32_t*) ((FLASH_MEM_ADDR + START_PAGE * PAGE_SIZE) + 4); //извлекаем адрес перехода из вектора Reset
	Jump_To_Application = (pFunction)app_jump_address;  //приводим его к пользовательскому типу
	__set_MSP(*(__IO uint32_t*) (FLASH_MEM_ADDR + START_PAGE * PAGE_SIZE));  //устанавливаем SP приложения                                           
	Jump_To_Application();		                        //запускаем приложение	
}

uint8_t isAppAvailable(void)
{
	uint32_t table_address = (FLASH_MEM_ADDR + START_PAGE * PAGE_SIZE);
	uint32_t table_addr_data = *((uint32_t*)table_address);
	if (table_addr_data == 0xFFFFFFFF)
		return 0;
	return 1;
}

void eternalFlashing(uint32_t on_time, uint32_t off_time)
{
	while(1)
	{
		LED_ON;
		HAL_Delay(on_time);
		LED_OFF;
		HAL_Delay(off_time);
	}
}

void eternalFlashing_FMW_DOWNLOAD_ERROR(void)
{
	while(1)
	{
		for (uint32_t i = 100; i <= 400; i+=50)
		{
			LED_ON;
			HAL_Delay(i);
			LED_OFF;
			HAL_Delay(500 - i);
		}
	}
}

void turnOffSPI(void)
{
	SPI1->CR1 &= ~SPI_CR1_SPE;
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	InitSystemStructure();
	//FLASH->SR |= FLASH_SR_PGSERR | FLASH_SR_PGAERR;
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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
	
	HAL_Delay(50); // Initialization
	sys.tim16.drv.initTimer();
	SPI1->CR1 |= SPI_CR1_SPE;
	HAL_Delay(50);
	
	if (sys.uSD.drv.isAvailable()) // If uSD is attached
	{
		FATFS fs;
		FRESULT res;
		
		res = f_mount(&fs, "", 1); // filesystem mounting
		if (res != FR_OK) // if fs is not mounted
			eternalFlashing(DELAY_USD_ERROR_ON, DELAY_USD_ERROR_OFF); // eternal flashing
		
		{
			FILINFO fno;
			DIR dr;
			f_findfirst(&dr, &fno, "/", "bootloader_info.txt");
			if (fno.fname[0] == 0)
			{
				FIL file;
				f_open(&file, "bootloader_info.txt", FA_WRITE | FA_CREATE_ALWAYS);
				char writedata[] = "Project: ZSpectrum\nBootloader ver.: 0.0.1\nInfo: test_bootloader";
				uint32_t bwr;
				f_write(&file, writedata, sizeof(writedata), &bwr);
				f_close(&file);
			}
		}
		
		FILINFO filinfo;
		DIR dir;
		res = f_findfirst(&dir, &filinfo, "/", "*.hex"); // detecting firmware
		
		if (filinfo.fname[0] == 0) // if firmware update file is not detected
		{
			//f_mount(NULL, "", 1); // unmounting fs
			if (isAppAvailable()) // if application is allocated in memory
			{
				turnOffSPI();
				jumpToApplication(); // jump to application
			}
			else // if no app detected
				eternalFlashing(DELAY_NO_APP_DETECT_ON, DELAY_NO_APP_DETECT_OFF); // eternal led flashing
		}
		else // if firmware update file is detected
		{
			struct IH_Data codeData;
			struct IH_Data compData;
			compData.ByteDataLength = 0;
			compData.LowerAddress = 0;
			TCHAR buff[256];
			uint32_t ADDR = 0;
			uint32_t eraseError;
			FIL file;
			res = f_open(&file, filinfo.fname, FA_READ | FA_OPEN_EXISTING); // open firmware file
			if (res != FR_OK) // if file was not opened 
				eternalFlashing(DELAY_USD_ERROR_ON, DELAY_USD_ERROR_OFF); // go to eternal flashing
			eraseError = eraseFlash(); // erasing memory for new fmw 
			if (eraseError != 0xFFFFFFFF) // if mem was not erased correctly
				eternalFlashing(DELAY_FLASH_ERASE_ERROR_ON, DELAY_FLASH_ERASE_ERROR_OFF); // go to eternal cycle
			
			HAL_FLASH_Unlock(); // unlocking flash
			do
			{
				f_gets(buff, 256, &file); // getting string from fmw file
				IH_ProcessString(&codeData, buff); // analyzing string
				if (!codeData.IsIHexString) // if str is not hex string, go to next string
					continue;
				if (codeData.IsCheckSumError) // if checksum error detected
				{
					HAL_FLASH_Lock(); // locking flash
					eternalFlashing_FMW_DOWNLOAD_ERROR(); // got to eternal cycle
				}
				if (codeData.DataType == IH_ExtLinAddrType || codeData.DataType == IH_ExtSegAddrType) // if data type == addr type
					ADDR = codeData.LowerAddress << 16; // collecting it
				if (codeData.DataType == IH_DataType) // if sting is data type
				{
					uint8_t i; // VER 2 BOOTLOADER
					for (i = compData.ByteDataLength; i < compData.ByteDataLength + codeData.ByteDataLength; i++)
						compData.ByteData[i] = codeData.ByteData[i - compData.ByteDataLength];
					compData.LowerAddress = codeData.LowerAddress - compData.ByteDataLength;
					compData.ByteDataLength = compData.ByteDataLength + codeData.ByteDataLength;
					
					uint8_t full64len = compData.ByteDataLength / 8;
					
					for (i = 0; i < full64len; i++)
					{
						uint32_t addr = (ADDR & 0xFFFF0000) + (compData.LowerAddress & 0x0000FFFF) + i * 8;
						uint32_t dat1 = compData.ByteData[i*8]            + ((uint32_t)compData.ByteData[i*8 + 1] << 8)
						   + ((uint32_t)compData.ByteData[i*8 + 2] << 16) + ((uint32_t)compData.ByteData[i*8 + 3] << 24);
						uint32_t dat2 = compData.ByteData[i*8 + 4]        + ((uint32_t)compData.ByteData[i*8 + 5] << 8)
   						 + ((uint32_t)compData.ByteData[i*8 + 6] << 16) + ((uint32_t)compData.ByteData[i*8 + 7] << 24);
						uint64_t data = (uint64_t)dat1 + ((uint64_t)dat2 << 32);
						__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR); 
  					HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, data);
						
						uint64_t checkProgrammed = *(uint64_t*)addr; 
						if (data != checkProgrammed) 
						{
							HAL_FLASH_Lock(); 
							eternalFlashing_FMW_DOWNLOAD_ERROR();
						} 
					}
					
					uint8_t sLen = full64len * 8;
					for (i = sLen; i < compData.ByteDataLength; i++)
						compData.ByteData[i - sLen] = compData.ByteData[i];
					compData.ByteDataLength = compData.ByteDataLength - sLen;
					
				}
			} while(codeData.DataType != IH_LastEntryType && buff[0] != 0); // do that until empty string or data type == last entry type
			
			if (compData.ByteDataLength > 0)
			{
				uint8_t allignLen = (compData.ByteDataLength / 8 + 1) * 8;
				uint8_t i;
				for (i = compData.ByteDataLength; i < allignLen; i++)
					compData.ByteData[i] = 0xFF;
				
				uint8_t full64len = allignLen / 8;
					
					for (i = 0; i < full64len; i++)
					{
						uint32_t addr = (ADDR & 0xFFFF0000) + (compData.LowerAddress & 0x0000FFFF) + i * 8;
						uint32_t dat1 = compData.ByteData[i*8]            + ((uint32_t)compData.ByteData[i*8 + 1] << 8)
						   + ((uint32_t)compData.ByteData[i*8 + 2] << 16) + ((uint32_t)compData.ByteData[i*8 + 3] << 24);
						uint32_t dat2 = compData.ByteData[i*8 + 4]        + ((uint32_t)compData.ByteData[i*8 + 5] << 8)
   						 + ((uint32_t)compData.ByteData[i*8 + 6] << 16) + ((uint32_t)compData.ByteData[i*8 + 7] << 24);
						uint64_t data = (uint64_t)dat1 + ((uint64_t)dat2 << 32);
						__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR); // clearing flag
  					HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, data);
						
						uint64_t checkProgrammed = *(uint64_t*)addr; 
						if (data != checkProgrammed) // if word wasn't programmed
						{
							HAL_FLASH_Lock(); // locking flash
							eternalFlashing_FMW_DOWNLOAD_ERROR(); // go to eternal cycle
						} 
					}
			}
			
			HAL_FLASH_Lock(); // locking flash
			f_close(&file); // closing fmw file
			f_unlink(filinfo.fname); // deleting fmw file
			//f_mount(NULL, "", 1); // unmounting fs
			if (isAppAvailable()) // if application is allocated in memory
			{
				turnOffSPI();
				jumpToApplication(); // jump to application
			}
			else // if no app detected
				eternalFlashing(DELAY_NO_APP_DETECT_ON, DELAY_NO_APP_DETECT_OFF);
				//eternalFlashing_NO_APP_DETECTED(); // eternal led flashing
		}
	}
	else // if uSD is detached
	{
		if (isAppAvailable()) // if application is allocated in memory
		{
			turnOffSPI();
			jumpToApplication(); // jump to application
		}
		else // if no app detected
			eternalFlashing(DELAY_NO_APP_DETECT_ON, DELAY_NO_APP_DETECT_OFF);
			//eternalFlashing_NO_APP_DETECTED(); // eternal led flashing
	}
	
	
  /* USER CODE END 2 */

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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK4|RCC_CLOCKTYPE_HCLK2
                              |RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK2Divider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.AHBCLK4Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SMPS;
  PeriphClkInitStruct.SmpsClockSelection = RCC_SMPSCLKSOURCE_HSI;
  PeriphClkInitStruct.SmpsDivSelection = RCC_SMPSCLKDIV_RANGE1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN Smps */

  /* USER CODE END Smps */
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
