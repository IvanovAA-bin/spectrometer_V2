#ifndef _RADIATION_SUBSYSTEM_H_
#define _RADIATION_SUBSYSTEM_H_

#include "main.h"

struct _rad
{
	uint32_t _spectrogram[4096];
    uint32_t _cps;
    uint32_t _saveTime;
	
	void (*startComp) (void);
	void (*stopComp) (void);
	uint16_t (*calculateMaxValue) (uint16_t*, uint16_t);
	void (*clearSpectrogramm) (void);
};

void _init_rad_subsystem(struct _rad *rad);


#endif
