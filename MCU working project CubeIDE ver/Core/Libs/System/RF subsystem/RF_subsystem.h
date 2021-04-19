#ifndef _RF_SUBSYSTEM_H_
#define _RF_SUBSYSTEM_H_

#include "main.h"
#include "ble_types.h"
#include "app_conf.h"
#include "custom_stm.h"
#include "stm32_seq.h"
#define _RF_ER_STATUSES_MAX_LEN 128U


enum NotificationChar
{
	_MCU_P_CPS_NC = 0, // ..._notification channel
	_MCU_P_DATA_NC,
	_MCU_P_SPCB_NC
};

struct _rf
{
	uint8_t ErrStatusesLen;
	tBleStatus ErrStatuses[_RF_ER_STATUSES_MAX_LEN];

	uint8_t isConnected;

	uint8_t p_mcu_cmd_len;
	uint8_t p_mcu_cmd_dat[21];

	uint8_t p_mcu_data_len;
	uint8_t p_mcu_data_dat[245];

	uint8_t mcu_p_CPS_dat[17];

	uint8_t mcu_p_spcb[21];

	uint8_t mcu_p_data[245];


	void (*setConnected) (uint8_t connStatus);
	void (*processFirstCharWrite) (uint8_t* data, uint16_t dataLength);
	void (*processSecondCharWrite) (uint8_t* data, uint16_t dataLength);
	void (*processNotification) (enum NotificationChar charNotification, uint8_t isEnabled);
	void (*updateCpsChar) (uint32_t cpsValue);

	//void (*_seq_pfcw) (void);
	//void (*_seq_pscw) (void);
	//void (*_seq_updateCPS) (void);
};

void _init_rf_subsystem(struct _rf *RF);

#endif
