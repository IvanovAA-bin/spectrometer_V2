#ifndef _USD_MODULE_H_
#define _USD_MODULE_H_

#include "stm32wb55xx.h"

struct _uSD
{
	uint8_t (*isAvailable)(void);
	void (*setInitialized)(uint8_t initValue);
	uint8_t (*isInitialized) (void);
};

void _init_uSD_subsystem(struct _uSD *uSD);


#endif
