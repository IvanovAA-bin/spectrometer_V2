#include "RF_subsystem.h"

struct _rf *RFS;


void __rf_setConnected (uint8_t connStatus)
{
	RFS->isConnected = connStatus;
	//GPIOB->ODR ^= GPIO_ODR_OD1;
	// PROCESS DISCONNECTION !!!!!!!!!
}


void __rf_processFirstCharWrite (uint8_t* data, uint16_t dataLength) // finished
{
	uint8_t i;
	if (dataLength > 20)
		dataLength = 20;
	for (i = 0; i < dataLength; i++)
		RFS->p_mcu_cmd_dat[i] = data[i];
	RFS->p_mcu_cmd_len = dataLength;
	UTIL_SEQ_SetTask(1 << USER_TASK_PROCESS_FCW, CFG_SCH_PRIO_0);
}

void __rf_processSecondCharWrite (uint8_t* data, uint16_t dataLength) // finished
{
	uint8_t i;
	if (dataLength > 240)
		dataLength = 240;
	for (i = 0; i < dataLength; i++)
		RFS->p_mcu_data_dat[i] = data[i];
	RFS->p_mcu_data_len = dataLength;
	UTIL_SEQ_SetTask(1 << USER_TASK_PROCESS_SCW, CFG_SCH_PRIO_0);
}

void __rf_processNotification (enum NotificationChar charNotification, uint8_t isEnabled)
{
	//GPIOB->ODR ^= GPIO_ODR_OD1;
	switch(charNotification)
	{
	case _MCU_P_CPS_NC:
		break;

	case _MCU_P_DATA_NC:
		break;

	case _MCU_P_SPCB_NC:
		break;
	}
}

void __rf_updateCpsChar (uint32_t cpsValue) // finished
{
	RFS->mcu_p_CPS_dat[0] = (uint8_t)((uint32_t) cpsValue >> 24);
	RFS->mcu_p_CPS_dat[1] = (uint8_t)((uint32_t) cpsValue >> 16);
	RFS->mcu_p_CPS_dat[2] = (uint8_t)((uint32_t) cpsValue >> 8);
	RFS->mcu_p_CPS_dat[3] = (uint8_t)cpsValue;
	UTIL_SEQ_SetTask(1 << USER_TASK_UPDATE_CPS_CHAR, CFG_SCH_PRIO_0);
}


void _init_rf_subsystem(struct _rf *RF) // finished
{
	RFS = RF;
	RFS->isConnected = 0;
	RFS->p_mcu_cmd_len = 0;
	RFS->p_mcu_data_len = 0;
	RFS->ErrStatusesLen = 0;
	RFS->setConnected = __rf_setConnected;
	RFS->processFirstCharWrite = __rf_processFirstCharWrite;
	RFS->processSecondCharWrite = __rf_processSecondCharWrite;
	RFS->processNotification = __rf_processNotification;
	RFS->updateCpsChar = __rf_updateCpsChar;
}




