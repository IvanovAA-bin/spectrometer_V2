#ifndef _TIM16_DRV_FUNCTIONS_H_
#define _TIM16_DRV_FUNCTIONS_H_

#include "stm32wb55xx.h"

void __tim16_initTimer (void);
void __tim16_setPrescalerValue (uint16_t val);
void __tim16_setCounterValue (uint16_t val);
void __tim16_startTimer(void);
uint8_t __tim16_isCounting (void);
uint8_t __tim16_hasCounted_clearUIFflag (void);
void __tim16_stopTimer (void);

#endif
