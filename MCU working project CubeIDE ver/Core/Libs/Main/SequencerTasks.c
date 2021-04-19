#include "SequencerTasks.h"

extern uint8_t ad_data[];

static void ResponceCmd(uint16_t cmd, uint8_t cmdResp)
{
	tBleStatus status;
	if (!sys.rf.isConnected)
		return;
	sys.rf.mcu_p_spcb[0] = cmdResp;
	sys.rf.mcu_p_spcb[1] = (uint8_t)((uint16_t)cmd >> 8);
	sys.rf.mcu_p_spcb[2] = cmd;
	status = Custom_STM_App_Update_Char(CUSTOM_STM_MCU_P_SPCB, sys.rf.mcu_p_spcb);
	if (status)
	{
		sys.rf.ErrStatuses[sys.rf.ErrStatusesLen] = status;
		sys.rf.ErrStatusesLen = (sys.rf.ErrStatusesLen + 1) % _RF_ER_STATUSES_MAX_LEN;
	}
}

static void ResponceData(uint16_t cmd, uint8_t cmdResp)
{
	tBleStatus status;
	if (!sys.rf.isConnected)
		return;
	sys.rf.mcu_p_data[0] = cmdResp;
	sys.rf.mcu_p_data[1] = (uint8_t)((uint16_t)cmd >> 8);
	sys.rf.mcu_p_data[2] = cmd;
	status = Custom_STM_App_Update_Char(CUSTOM_STM_MCU_P_DATA, sys.rf.mcu_p_data);
	if (status)
	{
		sys.rf.ErrStatuses[sys.rf.ErrStatusesLen] = status;
		sys.rf.ErrStatusesLen = (sys.rf.ErrStatusesLen + 1) % _RF_ER_STATUSES_MAX_LEN;
	}
}

static uint8_t isAcceptableCharValue(char value)
{
	if ((value >= 'a' && value <= 'z') || value == '.')
		return 1;
	if (value >= '0' && value <= '9')
		return 1;
	if (value >= 'A' && value <= 'Z')
		return 1;
	if (value == '_' || value == '-')
		return 1;
	return 0;
}



void ST_rf_ProcessFirstCharWrite (void) // p_mcu_cmd
{
	if (!sys.rf.isConnected)
		return;
	if (sys.rf.p_mcu_cmd_dat[0] != 0xCA && sys.rf.p_mcu_cmd_dat[3] != 0xFE)
		return;
	uint16_t cmd = (sys.rf.p_mcu_cmd_dat[1] << 8) + sys.rf.p_mcu_cmd_dat[2];
	uint16_t i, additionalData, helper;
	switch(cmd)
	{
	case 0x0001: // start impulse processing
		sys.rad.startComp();
		ResponceCmd(0x0001, 0x00);
		break;
	case 0x0002: // stop impulse processing
		sys.rad.stopComp();
		ResponceCmd(0x0002, 0x00);
		break;
	case 0x0003: // save spectrogram on uSD
		if (sys.fs._isInitialized)
		{
			sys.fs.saveRadInfo_csv("test.csv", sys.rad._spectrogram, 4096);
			ResponceCmd(0x0003, 0x00);
		}
		else
		{
			sys.fs.initialize("", 0);
			if (sys.fs._isInitialized)
			{
				sys.fs.saveRadInfo_csv("test.csv", sys.rad._spectrogram, 4096);
				ResponceCmd(0x0003, 0x00);
			}
			else
				ResponceCmd(0x0003, 0x01);
		}

		break;
	case 0x0004: // start sending CPS value
		sys.tim2.turnOn();
		ResponceCmd(0x0004, 0x00);
		break;
	case 0x0005: // stop sending CPS value
		sys.tim2.turnOff();
		ResponceCmd(0x0005, 0x00);
		break;
	case 0x0006: // fs initialization
		if (sys.uSD.isAvailable())
		{
			if (!sys.fs._isInitialized)
			{
				sys.fs.initialize("", 0);
				if (sys.fs._isInitialized)
					ResponceCmd(0x0006, 0x00);
				else
					ResponceCmd(0x0006, 0x03);
			}
			else
				ResponceCmd(0x0006, 0x02);
		}
		else
			ResponceCmd(0x0006, 0x01);
		break;
	case 0x0007:
		GPIOB->ODR ^= GPIO_ODR_OD1;
		ResponceCmd(0x0007, 0x00);
		break;
	case 0x0008: // get 20 values from spectrogram
		additionalData = (sys.rf.p_mcu_cmd_dat[4] << 8) + sys.rf.p_mcu_cmd_dat[5];

		if (additionalData > 4095)
		{
			ResponceData(0x0008, 0x01);
			break;
		}
		helper = additionalData + 20;
		if (helper > 4096)
			helper = 4096;

		sys.rf.mcu_p_data[3] = ((uint16_t)additionalData >> 8);
		sys.rf.mcu_p_data[4] = additionalData;
		sys.rf.mcu_p_data[5] = ((uint16_t)(helper - additionalData) >> 8);
		sys.rf.mcu_p_data[6] = ((uint16_t)(helper - additionalData));

		for (i = additionalData; i < helper; i++)
		{
			uint16_t j = 7 + (i - additionalData) * 4;
			sys.rf.mcu_p_data[j] = (sys.rad._spectrogram[i] >> 24);
			sys.rf.mcu_p_data[j + 1] = (sys.rad._spectrogram[i] >> 16);
			sys.rf.mcu_p_data[j + 2] = (sys.rad._spectrogram[i] >> 8);
			sys.rf.mcu_p_data[j + 3] = sys.rad._spectrogram[i];
		}
		ResponceData(0x0008, 0x00);
		break;
	case 0x0009: // clear spectrogram
		for (i = 0; i < 4096; i++)
			sys.rad._spectrogram[i] = 0;
		ResponceCmd(0x0009, 0x00);
		break;
	case 0x000A: // set time between cps writings
		helper = (sys.rf.p_mcu_cmd_dat[4] << 8) + sys.rf.p_mcu_cmd_dat[5];
		if (helper < 50)
		{
			helper = 50;
			ResponceCmd(0x000A, 0x01);
		}
		else
			ResponceCmd(0x000A, 0x00);
		sys.tim2.setTime(helper);
		break;
	case 0x000C: // set measurement time in minutes
		{
			uint32_t measurementTime = ((uint32_t)sys.rf.p_mcu_cmd_dat[4] << 24) + ((uint32_t)sys.rf.p_mcu_cmd_dat[5] << 16) +
					((uint32_t)sys.rf.p_mcu_cmd_dat[6] << 8) + sys.rf.p_mcu_cmd_dat[7];
			measurementTime *= 60;
			if (measurementTime == 0)
				sys.rad._saveTime = 0xFFFFFFFF;
			else
				sys.rad._saveTime = sys.tim17.globalSystemTimeSec + measurementTime;
			ResponceCmd(0x000C, 0x00);
		}
		break;
	case 0x000D: // set device number
		{
			char c1 = sys.rf.p_mcu_cmd_dat[4];
			char c2 = sys.rf.p_mcu_cmd_dat[5];
			if (!((c1 >= '0' && c1 <= '9') || (c1 >= 'a' && c1 <= 'z') || (c1 >= 'A' && c1 <= 'Z')))
			{
				ResponceCmd(0x000D, 0x01);
				return;
			}
			if (!((c2 >= '0' && c2 <= '9') || (c2 >= 'a' && c2 <= 'z') || (c2 >= 'A' && c2 <= 'Z')))
			{
				ResponceCmd(0x000D, 0x01);
				return;
			}
			ad_data[8] = sys.settings.deviceNumber[0] = c1;
			ad_data[9] = sys.settings.deviceNumber[1] = c2;
			if (!sys.fs._isInitialized)
				sys.fs.initialize("", 0);
			if (sys.fs._isInitialized)
			{
				sys.fs.saveSystemSettingsInfo(&sys.settings);
				ResponceCmd(0x000D, 0x00);
			}
			else
				ResponceCmd(0x000D, 0x02);

		}
		break;
	case 0x000E:
		{ // set dma buffer size
			uint16_t val = ((uint16_t)sys.rf.p_mcu_cmd_dat[4] << 8) + sys.rf.p_mcu_cmd_dat[5];
			if (val < 5 || val > 1000)
			{
				ResponceCmd(0x000E, 0x01);
				return;
			}
			if (!sys.fs._isInitialized)
				sys.fs.initialize("", 0);
			if (sys.fs._isInitialized)
			{
				sys.settings.countsToDma = val;
				sys.fs.saveSystemSettingsInfo(&sys.settings);
				ResponceCmd(0x000E, 0x00);
			}
			else
				ResponceCmd(0x000E, 0x02);
		}
		break;
	case 0x000F:
		{ // set min filtration value
			uint16_t val = ((uint16_t)sys.rf.p_mcu_cmd_dat[4] << 8) + sys.rf.p_mcu_cmd_dat[5];
			sys.settings.minValueFiltration = val;
			if (!sys.fs._isInitialized)
				sys.fs.initialize("", 0);
			if (sys.fs._isInitialized)
			{
				sys.fs.saveSystemSettingsInfo(&sys.settings);
				ResponceCmd(0x000F, 0x00);
			}
			else
				ResponceCmd(0x000F, 0x01);
		}
		break;
	case 0x0010:
		{ // set max filtration value
			uint16_t val = ((uint16_t)sys.rf.p_mcu_cmd_dat[4] << 8) + sys.rf.p_mcu_cmd_dat[5];
			sys.settings.maxValueFiltration = val;
			if (!sys.fs._isInitialized)
				sys.fs.initialize("", 0);
			if (sys.fs._isInitialized)
			{
				sys.fs.saveSystemSettingsInfo(&sys.settings);
				ResponceCmd(0x0010, 0x00);
			}
			else
				ResponceCmd(0x0010, 0x01);
		}
		break;
	case 0x0011:
		{ // get device status
			uint32_t flags = 0;
			if (COMP2->CSR & COMP_CSR_EN) // if impulse processing
				flags |= ((uint32_t)1 << 0);
			if (TIM2->CR1 & TIM_CR1_CEN) // if CPS sending
				flags |= ((uint32_t)1 << 1);
			if (sys.uSD.isAvailable()) // if uSD is available
				flags |= ((uint32_t)1 << 2);
			if (sys.fs._isInitialized) // if filesystem is initialized
				flags |= ((uint32_t)1 << 3);
			if (sys.rf.ErrStatusesLen) // if ble has error statuses
				flags |= ((uint32_t)1 << 4);
			if (sys.rad._saveTime != 0xFFFFFFFF) // if timer is set
				flags |= ((uint32_t)1 << 5);
			if (COMP2->CSR & COMP_CSR_VALUE) // comparator value
				flags |= ((uint32_t)1 << 6);

			uint32_t remainingTime = (flags & (1 << 5))? (sys.rad._saveTime-sys.tim17.globalSystemTimeSec) / 60 : 0; // in minutes
			uint8_t *mask = sys.rf.mcu_p_spcb;
			mask[3] = ((uint32_t)flags >> 24);
			mask[4] = ((uint32_t)flags >> 16);
			mask[5] = ((uint32_t)flags >> 8);
			mask[6] = flags;
			mask[7] = ((uint32_t)remainingTime >> 24);
			mask[8] = ((uint32_t)remainingTime >> 16);
			mask[9] = ((uint32_t)remainingTime >> 8);
			mask[10] = remainingTime;

			mask[11] = ((uint16_t)sys.settings.countsToDma >> 8);
			mask[12] = sys.settings.countsToDma;

			mask[13] = ((uint16_t)sys.settings.minValueFiltration >> 8);
			mask[14] = sys.settings.minValueFiltration;

			mask[15] = ((uint16_t)sys.settings.maxValueFiltration >> 8);
			mask[16] = sys.settings.maxValueFiltration;
			ResponceCmd(0x0011, 0x00);
		}
		break;
	default:
		ResponceCmd(0xFFFF, 0xAA);

	}


	//GPIOB->ODR ^= GPIO_ODR_OD1;
}

void ST_rf_ProcessSecondCharWrite (void) // p_mcu_data
{
	if (!sys.rf.isConnected)
		return;
	if (sys.rf.p_mcu_data_dat[0] != 0xCA && sys.rf.p_mcu_data_dat[3] != 0xFE)
		return;
	uint16_t cmd = (sys.rf.p_mcu_data_dat[1] << 8) + sys.rf.p_mcu_data_dat[2];
	switch (cmd)
	{
	case 0x000B:
		{
			uint16_t len = sys.rf.p_mcu_data_dat[4];
			static char fileName[70];
			uint16_t i;
			if (len < 5 || len > 64)
			{
				ResponceCmd(0x000B, 0x01);
				return;
			}
			if (sys.rf.p_mcu_data_dat[5] != '#' && sys.rf.p_mcu_data_dat[6 + len] != '#')
			{
				ResponceCmd(0x000B, 0x02);
				return;
			}
			for (i = 0; i < len; i++)
			{
				fileName[i] = sys.rf.p_mcu_data_dat[6 + i];
				if (!isAcceptableCharValue(fileName[i]))
				{
					ResponceCmd(0x000B, 0x03);
					return;
				}
			}
			fileName[len] = 0;
			if (sys.fs._isInitialized)
			{
				sys.fs.saveRadInfo_csv(fileName, sys.rad._spectrogram, 4096);
				ResponceCmd(0x000B, 0x00);
			}
			else
			{
				sys.fs.initialize("", 0);
				if (sys.fs._isInitialized)
				{
					sys.fs.saveRadInfo_csv(fileName, sys.rad._spectrogram, 4096);
					ResponceCmd(0x000B, 0x00);
				}
				else
					ResponceCmd(0x000B, 0x04);
			}
		}
		break;

	default:
		ResponceCmd(0xFFFF, 0xAA);
	}

	//GPIOB->ODR ^= GPIO_ODR_OD1;
}

void ST_rf_UpdateCPS (void) // mcu_p_cps
{
	tBleStatus status;
	if (!sys.rf.isConnected)
		return;
	status = Custom_STM_App_Update_Char(CUSTOM_STM_MCU_P_CPS, sys.rf.mcu_p_CPS_dat);
	if (status)
	{
		sys.rf.ErrStatuses[sys.rf.ErrStatusesLen] = status;
		sys.rf.ErrStatusesLen = (sys.rf.ErrStatusesLen + 1) % _RF_ER_STATUSES_MAX_LEN;
	}
}

void ST_fs_Initialize (void)
{
	//GPIOB->ODR ^= GPIO_ODR_OD1;
	if (sys.uSD.isAvailable())
		sys.fs.initialize("", 0);
}

void ST_fs_SaveSpectrPostDelayed (void)
{

	if (sys.uSD.isAvailable())
	{
		if (!sys.fs._isInitialized)
			sys.fs.initialize("", 0);
		if (sys.fs._isInitialized)
			sys.fs.saveRadInfo_csv("post_delayed.csv", sys.rad._spectrogram, 4096);
	}
}












