#ifndef _ADC_SUBSYSTEM_H_
#define _ADC_SUBSYSTEM_H_

#include "stm32wb55xx.h"

struct _adc
{
	void (*calibrateAndReinit) (void);
	void (*startBatConversion) (void);
	void (*startSigConversion) (void);
	uint8_t (*isBatDataAvailable) (void);
	uint8_t (*isSigDataAvailable) (void);
	uint16_t (*getSigData) (void);
	uint16_t (*getBatData) (void);
	void (*reCalibrate) (void);
};

void _init_adc_subsystem(struct _adc *adc);

#endif
