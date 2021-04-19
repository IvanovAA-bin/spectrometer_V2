#include "TIM17_subsystem.h"


//volatile uint32_t globalSystemTimeSec = 0;


static volatile uint8_t ___dm = 0;

static void ___dDelay(void)
{
	++___dm;
}

void __tim17_initialize (void)
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;
	___dDelay();
	
	TIM17->PSC = (uint16_t) 64000 - 1;
	TIM17->ARR = (uint16_t) 1000;
	
	TIM17->CR1 |= TIM_CR1_URS | TIM_CR1_OPM;
	
	TIM17->CNT = 0;
	TIM17->EGR |= TIM_EGR_UG;
	___dDelay();
	TIM17->SR &= ~TIM_SR_UIF;
	NVIC_SetPriority(TIM1_TRG_COM_TIM17_IRQn, 1);
	NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);
	TIM17->DIER |= TIM_DIER_UIE;
}

void __tim17_turnOn (void)
{
	TIM17->CR1 |= TIM_CR1_CEN;
}

void __tim17_turnOff (void)
{
	TIM17->CR1 &= ~TIM_CR1_CEN;
}
	

void _init_tim17_subsystem(struct _tim17 *tim17)
{
	//globalSystemTimeSec = 0;
	tim17->globalSystemTimeSec = 0;
	tim17->initialize = __tim17_initialize;
	tim17->turnOn = __tim17_turnOn;
	tim17->turnOff = __tim17_turnOff;
}


