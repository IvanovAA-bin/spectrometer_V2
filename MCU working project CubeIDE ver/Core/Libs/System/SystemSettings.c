#include "SystemSettings.h"

void _init_system_settings(struct SystemSettings *settings)
{
	settings->countsToDma = 128;
	settings->deviceNumber[0] = 'X';
	settings->deviceNumber[1] = 'X';
	settings->minValueFiltration = 0;
	settings->maxValueFiltration = 0xFFFF;
}
