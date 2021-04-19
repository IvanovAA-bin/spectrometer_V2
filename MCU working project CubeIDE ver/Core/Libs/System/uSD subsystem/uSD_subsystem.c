#include "uSD_subsystem.h"

static uint8_t __uSD_initialized;


uint8_t __uSD_isAvailable(void)
{
	return (GPIOA->IDR & GPIO_IDR_ID9)?0x00:0x01;
}

void __uSD_setInitialized(uint8_t initValue)
{
	__uSD_initialized = initValue;
}

uint8_t __uSD_isInitialized (void)
{
	return __uSD_initialized;
}

void _init_uSD_subsystem(struct _uSD *uSD)
{
	__uSD_initialized = 0;
	uSD->isAvailable = __uSD_isAvailable;
	uSD->setInitialized = __uSD_setInitialized;
	uSD->isInitialized = __uSD_isInitialized;
}
