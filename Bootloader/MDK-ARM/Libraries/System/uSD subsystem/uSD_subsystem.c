#include "uSD_subsystem.h"
#include "uSD_driver_functions.h"

void _init_uSD_subsystem(struct _uSD *uSD)
{
	uSD->isInitialized = 0;
	
	
	uSD->drv.isAvailable = __uSD_isAvailable;
}
