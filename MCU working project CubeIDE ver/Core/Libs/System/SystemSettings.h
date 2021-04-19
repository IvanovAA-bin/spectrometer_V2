#ifndef _SYSTEM_SETTINGS_H_
#define _SYSTEM_SETTINGS_H_

#include "main.h"

struct SystemSettings
{
	char deviceNumber[2];
	uint16_t countsToDma;
	uint16_t minValueFiltration;
	uint16_t maxValueFiltration;
};

void _init_system_settings(struct SystemSettings *settings);

#endif
