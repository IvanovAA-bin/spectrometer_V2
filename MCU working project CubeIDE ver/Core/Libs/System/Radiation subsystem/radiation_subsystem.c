#include "radiation_subsystem.h"

static struct _rad *_RAD;

void __rad_startComp (void)
{
	COMP2->CSR |= COMP_CSR_EN;
}

void __rad_stopComp (void)
{
	COMP2->CSR &= ~COMP_CSR_EN;
}

uint16_t __rad_calculateMaxValue (uint16_t* mas, uint16_t len)
{
	uint16_t max_val = mas[0];
	uint8_t returnCounter = 0;
	for (uint16_t i = 1; i < len; i++)
	{
		if (mas[i] > max_val)
		{
			max_val = mas[i];
			returnCounter = 0;
		}
		else
			++returnCounter;
		if (returnCounter >= 3)
			break;
	}
	max_val &= 0x0FFF;
	return max_val;
}

void __rad_clearSpectrogramm (void)
{
	for (uint16_t i = 0; i < 4096; i++)
		_RAD->_spectrogram[i] = 0;
}

void _init_rad_subsystem(struct _rad *rad)
{
	_RAD = rad;
	__rad_clearSpectrogramm();
	_RAD->_cps = 0;
	rad->_saveTime = 0xFFFFFFFF;
	rad->startComp = __rad_startComp;
	rad->stopComp = __rad_stopComp;
	rad->calculateMaxValue = __rad_calculateMaxValue;
	rad->clearSpectrogramm = __rad_clearSpectrogramm;
}















