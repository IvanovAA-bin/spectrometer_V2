#ifndef _DMA_SUBSYSTEM_H_
#define _DMA_SUBSYSTEM_H_

#include "stm32wb55xx.h"

struct _dma
{
	void (*initialize) (uint16_t dataCounts);
	void (*returnDataInfo) (uint16_t **dataBuffer, uint16_t *dataCounts);
	
};

void _init_dma_dubsystem(struct _dma *dma);


#endif
