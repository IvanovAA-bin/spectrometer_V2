#ifndef _TIM17_SUBSYSTEM_H_
#define _TIM17_SUBSYSTEM_H_

#include "stm32wb55xx.h"

struct _tim17
{
	volatile uint32_t globalSystemTimeSec;
	void (*initialize) (void);
	void (*turnOn) (void);
	void (*turnOff) (void);
};

//extern volatile uint32_t globalSystemTimeSec;

void _init_tim17_subsystem(struct _tim17 *tim17);


#endif

