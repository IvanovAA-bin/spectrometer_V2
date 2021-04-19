#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "stm32wb55xx.h"
#include "uSD subsystem/uSD_subsystem.h"
#include "TIM16 subsystem/TIM16_subsystem.h"

typedef struct
{
	struct _uSD uSD;
	struct _tim16 tim16;
} System;

extern System sys;

void InitSystemStructure(void);
#endif
