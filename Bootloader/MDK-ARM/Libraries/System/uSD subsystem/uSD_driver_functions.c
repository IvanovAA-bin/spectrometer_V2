#include "uSD_driver_functions.h"

uint8_t __uSD_isAvailable(void)
{
	return (GPIOA->IDR & GPIO_IDR_ID9)?0x00:0x01;
}
