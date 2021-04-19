#include "System.h"

System sys;

void InitSystemStructure(void)
{
	_init_uSD_subsystem(&sys.uSD);
	_init_tim16_subsystem(&sys.tim16);
}
