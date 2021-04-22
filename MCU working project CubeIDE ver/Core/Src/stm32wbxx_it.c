/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32wbxx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32wbxx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_common.h"
#include "System.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern COMP_HandleTypeDef hcomp2;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
	  GPIOB->ODR ^= GPIO_ODR_OD1;
	  for (uint32_t i = 0; i < 1000000; i++);
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32WBxx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32wbxx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */
	uint16_t *dma_buffer;
	uint16_t dma_buf_len;

	sys.dma.returnDataInfo(&dma_buffer, &dma_buf_len);
	uint16_t max_val = sys.rad.calculateMaxValue(dma_buffer, dma_buf_len);
	//sys.rad.increaseChannelData(max_val);

	if (max_val > sys.settings.minValueFiltration)
	{
		if (sys.tim2.isTurnedOn)
			++sys.rad._cps;
		if (max_val < sys.settings.maxValueFiltration)
			++sys.rad._spectrogram[max_val];
	}


	GPIOB->BSRR |= GPIO_BSRR_BS1; // LED


	DMA1->IFCR |= DMA_IFCR_CGIF1 | DMA_IFCR_CHTIF1;
	ADC1->ISR |= ADC_ISR_EOS | ADC_ISR_EOSMP;
  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles COMP1 and COMP2 interrupts through EXTI lines 20 and 21.
  */
void COMP_IRQHandler(void)
{
  /* USER CODE BEGIN COMP_IRQn 0 */
	if (!(ADC1->CR & ADC_CR_ADSTART) && !(DMA1->IFCR & (DMA_IFCR_CGIF1 | DMA_IFCR_CHTIF1)))
	{
		ADC1->CR |= ADC_CR_ADSTART;
		GPIOB->BSRR |= GPIO_BSRR_BR1; // LED
	}

  /* USER CODE END COMP_IRQn 0 */
  HAL_COMP_IRQHandler(&hcomp2);
  /* USER CODE BEGIN COMP_IRQn 1 */

  /* USER CODE END COMP_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */
	if (!sys.uSD.isAvailable()) // if uSD has been detached
	{
		sys.uSD.setInitialized(0);
		sys.fs._isInitialized = 0;
		sys.fs._initializationTime = 0xFFFFFFFF;
	}
	else
	{
		sys.fs._initializationTime = sys.tim17.globalSystemTimeSec + 3;
	}
  /* USER CODE END EXTI9_5_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */

  /* USER CODE END EXTI9_5_IRQn 1 */
}

/**
  * @brief This function handles HSEM global interrupt.
  */
void HSEM_IRQHandler(void)
{
  /* USER CODE BEGIN HSEM_IRQn 0 */

  /* USER CODE END HSEM_IRQn 0 */
  HAL_HSEM_IRQHandler();
  /* USER CODE BEGIN HSEM_IRQn 1 */

  /* USER CODE END HSEM_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void RTC_WKUP_IRQHandler(void)
{
	HW_TS_RTC_Wakeup_Handler();
}
void IPCC_C1_RX_IRQHandler(void)
{
	HW_IPCC_Rx_Handler();
}
void IPCC_C1_TX_IRQHandler(void)
{
	HW_IPCC_Tx_Handler();
}

void TIM1_TRG_COM_TIM17_IRQHandler(void) // ONE SECOND TIMER
{

	if (sys.tim17.globalSystemTimeSec >= sys.fs._initializationTime)
	{
		sys.fs._initializationTime = 0xFFFFFFFF;
		UTIL_SEQ_SetTask(1 << USER_TASK_INITIALIZE_FS, CFG_SCH_PRIO_0);
	}
	if (sys.tim17.globalSystemTimeSec >= sys.rad._saveTime)
	{
		sys.rad._saveTime = 0xFFFFFFFF;
		UTIL_SEQ_SetTask(1 << USER_TASK_SAVE_SPECTR_POST_DELAYED, CFG_SCH_PRIO_0);
	}

	++sys.tim17.globalSystemTimeSec;
	TIM17->SR &= ~TIM_SR_UIF;
	TIM17->CR1 |= TIM_CR1_CEN;
}

void TIM2_IRQHandler(void) // CPS UPDATER TIMER
{
	if (sys.rf.isConnected)
		sys.rf.updateCpsChar(sys.rad._cps);
	sys.rad._cps = 0;
	TIM2->SR &= ~TIM_SR_UIF;
	TIM2->CR1 |= TIM_CR1_CEN;
}

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
