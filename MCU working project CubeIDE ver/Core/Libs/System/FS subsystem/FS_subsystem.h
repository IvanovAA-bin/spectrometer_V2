#ifndef _FS_SUBSYSTEM_H_
#define _FS_SUBSYSTEM_H_

#include "stm32wb55xx.h"
#include "../SystemSettings.h"

struct _FS
{
	uint8_t _isInitialized;
	volatile uint32_t _initializationTime;
	
	void (*initialize) (char *info, uint8_t firstLoad);
	void (*saveRadInfo_csv) (char *file_name, uint32_t *mas, uint16_t size);
	
	void (*downloadSystemSettingsInfo) (struct SystemSettings* settings);
	void (*saveSystemSettingsInfo) (struct SystemSettings* settings);

};

void _init_fs_subsystem(struct _FS* fs);


#endif
