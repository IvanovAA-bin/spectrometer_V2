#ifndef _TIM2_SUBSYSTEM_H_
#define _TIM2_SUBSYSTEM_H_

#include "main.h"
#include "stm32wb55xx.h"

struct _tim2
{
	uint8_t isTurnedOn;
	void (*initialize)(void);
	void (*setTime)(uint16_t millis);
	void (*turnOn)(void);
	void (*turnOff)(void);
};

void _init_tim2_subsystem(struct _tim2* tim2);

#endif
