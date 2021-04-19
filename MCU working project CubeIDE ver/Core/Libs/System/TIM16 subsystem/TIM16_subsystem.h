#ifndef _TIM16_SUBSYSTEM_H_
#define _TIM16_SUBSYSTEM_H_

#include "stm32wb55xx.h"

struct _tim16
{
	void (*initTimer) (void);
	void (*setPrescalerValue)(uint16_t);
	void (*setCounterValue) (uint16_t);
	void (*startTimer)(void);
	uint8_t (*isCounting) (void);
	uint8_t (*hasCounted_clearUIFflag) (void);
	void (*stopTimer) (void);
};

void _init_tim16_subsystem(struct _tim16 *timer);

#endif
