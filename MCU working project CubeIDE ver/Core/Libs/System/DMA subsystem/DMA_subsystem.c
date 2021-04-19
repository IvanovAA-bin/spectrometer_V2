#include "DMA_subsystem.h"

static uint16_t _dma_data_buffer[1024];
static uint16_t _dma_data_counts;


void __dma_initialize (uint16_t dataCounts)
{
	DMA1_Channel1->CPAR = (uint32_t)&(ADC1->DR);
	DMA1_Channel1->CMAR = (uint32_t)_dma_data_buffer;
	DMA1_Channel1->CNDTR = dataCounts;
	_dma_data_counts = dataCounts;
	DMA1_Channel1->CCR |= DMA_CCR_TCIE;
	DMA1_Channel1->CCR |= DMA_CCR_EN;
}


void __dma_returnDataInfo (uint16_t **dataBuffer, uint16_t *dataCounts)
{
	*dataBuffer = _dma_data_buffer;
	*dataCounts = _dma_data_counts;
}


void _init_dma_dubsystem(struct _dma *dma)
{
	_dma_data_counts = 0;
	dma->initialize = __dma_initialize;
	dma->returnDataInfo = __dma_returnDataInfo;
}

