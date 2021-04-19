#include "TIM16_subsystem.h"

static uint8_t ___dumm = 0;

static void ___dummyDelay(void)
{
	++___dumm;
}

static void ___tim16_updatePscArrCntRegisters (void)
{
	TIM16->CNT = 0;
	TIM16->EGR |= TIM_EGR_UG;
	___dummyDelay();
	TIM16->SR &= ~TIM_SR_UIF;
}

void __tim16_initTimer (void)
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
	___dummyDelay();
	TIM16->CR1 |= TIM_CR1_OPM;
	TIM16->PSC = (uint16_t) 64000 - 1;
	TIM16->ARR = (uint16_t) 32000;
	___tim16_updatePscArrCntRegisters();
}
void __tim16_setPrescalerValue (uint16_t val)
{
	TIM16->PSC = val - 1;
}

void __tim16_setCounterValue (uint16_t val)
{
	TIM16->CR1 &= ~TIM_CR1_CEN;
	TIM16->ARR = val - 1;
	___tim16_updatePscArrCntRegisters();
}

void __tim16_startTimer(void)
{
	TIM16->CR1 |= TIM_CR1_CEN;
}

uint8_t __tim16_isCounting (void)
{
	return (TIM16->CR1 & TIM_CR1_CEN)?0x01:0x00;
}

uint8_t __tim16_hasCounted_clearUIFflag (void)
{
	uint8_t retval = (TIM16->SR & TIM_SR_UIF)?0x01:0x00;
	TIM16->SR &= ~TIM_SR_UIF;
	return retval;
}

void __tim16_stopTimer (void)
{
	TIM16->CR1 &= ~TIM_CR1_CEN;
}


void _init_tim16_subsystem(struct _tim16 *timer)
{
	timer->initTimer = __tim16_initTimer;
	timer->setPrescalerValue = __tim16_setPrescalerValue;
	timer->setCounterValue = __tim16_setCounterValue;
	timer->startTimer = __tim16_startTimer;
	timer->isCounting = __tim16_isCounting;
	timer->hasCounted_clearUIFflag = __tim16_hasCounted_clearUIFflag;
	timer->stopTimer = __tim16_stopTimer;
}
