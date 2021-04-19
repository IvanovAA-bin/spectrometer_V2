#include "System.h"

System sys;

void InitSystemStructure(void)
{
	_init_uSD_subsystem(&sys.uSD);
	_init_tim16_subsystem(&sys.tim16);
	_init_adc_subsystem(&sys.adc);
	_init_dma_dubsystem(&sys.dma);
	_init_fs_subsystem(&sys.fs);
	_init_tim17_subsystem(&sys.tim17);
	_init_rad_subsystem(&sys.rad);
	_init_rf_subsystem(&sys.rf);
	_init_tim2_subsystem(&sys.tim2);

	_init_system_settings(&sys.settings);
}
