#include "TIM2_subsystem.h"

static struct _tim2 *_T2;

static volatile uint8_t _t2_dm = 0;

static void _t2_dDelay(void)
{
	++_t2_dm;
}


void __tim2_initialize (void)
{

	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	_t2_dDelay();

	TIM2->PSC = (uint16_t) 64000 - 1;
	TIM2->ARR = (uint16_t) 1000;
	TIM2->CR1 |= TIM_CR1_ARPE | TIM_CR1_OPM | TIM_CR1_URS;
	TIM2->CNT = 0;
	TIM2->EGR |= TIM_EGR_UG;
	_t2_dDelay();
	TIM2->SR &= ~TIM_SR_UIF;
	NVIC_SetPriority(TIM2_IRQn, 1);
	NVIC_EnableIRQ(TIM2_IRQn);
	TIM2->DIER |= TIM_DIER_UIE;
}

void __tim2_setTime (uint16_t millis)
{
	TIM2->ARR = millis;
}

void __tim2_turnOn (void)
{
	TIM2->CR1 |= TIM_CR1_CEN;
	_T2->isTurnedOn = 1;
}

void __tim2_turnOff (void)
{
	TIM2->CR1 &= ~TIM_CR1_CEN;
	_T2->isTurnedOn = 0;
	_t2_dDelay();
	TIM2->CNT = 0;
}


void _init_tim2_subsystem(struct _tim2* tim2)
{
	_T2 = tim2;
	tim2->initialize = __tim2_initialize;
	tim2->setTime = __tim2_setTime;
	tim2->turnOn = __tim2_turnOn;
	tim2->turnOff = __tim2_turnOff;
}

