#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "stm32wb55xx.h"
#include "uSD subsystem/uSD_subsystem.h"
#include "TIM16 subsystem/TIM16_subsystem.h"
#include "ADC subsystem/ADC_subsystem.h"
#include "DMA subsystem/DMA_subsystem.h"
#include "FS subsystem/FS_subsystem.h"
#include "TIM17 subsystem/TIM17_subsystem.h"
#include "Radiation subsystem/radiation_subsystem.h"
#include "RF subsystem/RF_subsystem.h"
#include "TIM2 subsystem/TIM2_subsystem.h"
#include "SystemSettings.h"


typedef struct
{
	struct _FS fs;
	struct _uSD uSD;
	struct _tim2 tim2;
	struct _tim16 tim16;
	struct _tim17 tim17;
	struct _adc adc;
	struct _dma dma;
	struct _rad rad;
	struct _rf rf;

	struct SystemSettings settings;
} System;
extern System sys;




void InitSystemStructure(void);
#endif
