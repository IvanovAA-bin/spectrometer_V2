#ifndef _USD_MODULE_H_
#define _USD_MODULE_H_

#include "stm32wb55xx.h"

struct _uSD_driver
{
	uint8_t (*isAvailable)(void);
	
};


struct _uSD
{
	uint8_t isInitialized;
	struct _uSD_driver drv;
};

void _init_uSD_subsystem(struct _uSD *uSD);


#endif
