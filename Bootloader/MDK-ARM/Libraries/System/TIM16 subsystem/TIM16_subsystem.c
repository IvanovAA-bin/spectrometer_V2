#include "TIM16_subsystem.h"
#include "TIM16_driver_functions.h"

void _init_tim16_subsystem(struct _tim16 *timer)
{
	timer->drv.initTimer = __tim16_initTimer;
	timer->drv.setPrescalerValue = __tim16_setPrescalerValue;
	timer->drv.setCounterValue = __tim16_setCounterValue;
	timer->drv.startTimer = __tim16_startTimer;
	timer->drv.isCounting = __tim16_isCounting;
	timer->drv.hasCounted_clearUIFflag = __tim16_hasCounted_clearUIFflag;
	timer->drv.stopTimer = __tim16_stopTimer;
}
