#include "main.h"
#include "../System.h"

//*************<required functons>****************
uint8_t USER_SPI_TransmitReceive(uint8_t *transmit, uint8_t *reseive, uint16_t count)
{
	uint16_t i = 1;
	while(!(SPI1->SR & SPI_SR_TXE)) {;}
	*((volatile uint8_t*)&SPI1->DR) = (uint8_t)(*transmit);
	for (; i<count; i++)
	{
		while(!(SPI1->SR & SPI_SR_TXE)) {;}
		*((volatile uint8_t*)&SPI1->DR) = (uint8_t)(*transmit);
		while(!(SPI1->SR & SPI_SR_RXNE)) {;}
		reseive[i - 1] = *((volatile uint8_t*)&SPI1->DR);
	}
	while(!(SPI1->SR & SPI_SR_RXNE)) {;}
	reseive[i - 1] = *((volatile uint8_t*)&SPI1->DR);
	//while(SPI1->SR & SPI_SR_BSY);
	while(SPI1->SR & (SPI_SR_FRLVL | SPI_SR_FTLVL));
	return 0;
}

void USER_SPI_Transmit(uint8_t *buf, uint16_t size) 
{
	uint16_t i = 0;
	uint8_t dummy_get;
	while(!(SPI1->SR & SPI_SR_TXE)) {;}
	*((volatile uint8_t*)&SPI1->DR) = (uint8_t)(buf[i]);
	for (i = 1; i<size; i++)
	{
		while(!(SPI1->SR & SPI_SR_TXE)) {;}
		*((volatile uint8_t*)&SPI1->DR) = (uint8_t)(buf[i]);
		while(!(SPI1->SR & SPI_SR_RXNE)) {;}
		dummy_get = *((volatile uint8_t*)&SPI1->DR);
	}
	while(!(SPI1->SR & SPI_SR_RXNE)) {;}
	dummy_get = *((volatile uint8_t*)&SPI1->DR);
	(void)dummy_get;
	//while(SPI1->SR & SPI_SR_BSY);
	while(SPI1->SR & (SPI_SR_FRLVL | SPI_SR_FTLVL));
	return;
}

void USER_SDCARD_Select() 
{ GPIOA->BSRR |= GPIO_BSRR_BR8; }

void USER_SDCARD_Unselect()
{ GPIOA->BSRR |= GPIO_BSRR_BS8; }
//*************</required functons>****************

//*************<additional functions>****************
uint8_t USER_SDCARD_isAvailable() // return value: !0 - available; 0 - isn't available
{
	return sys.uSD.isAvailable();
}

void USER_SDCARD_isInitialisedCallback() // Callback, which is called after succesfull uSD initialisation
{
	sys.uSD.setInitialized(1);
}

/*
* This function is called, when fatfs wants to know disk status
* return 0 when everything ok
* return 0x01 when drive is not initialized
* return 0x02 when there is no drive right now
*/
uint8_t USER_Return_uSD_Status()
{
	if (!sys.uSD.isAvailable())
		return 0x02;
	if (!sys.uSD.isInitialized())
		return 0x01;
	return 0x00;
}

//*************<user infinite cycle protection (ICP) functions>****************
void USER_ICP_ResetCondition() // Reset the condition (example: start timer)
{
	sys.tim16.setCounterValue(3000);
	sys.tim16.startTimer();
}
uint8_t USER_ICP_CheckCondition() // Check the condition. 1 = continue cycle. 0 = end cycle (example: timer value < SET_VALUE)
{
	return sys.tim16.isCounting();
}

uint8_t USER_ICP_ConditionValidation() // if condition happened, return 1 (means error), return 0 if everything ok (example: timer value >= SET_VALUE means error)
{
	sys.tim16.stopTimer();
	return sys.tim16.hasCounted_clearUIFflag();
}

//*************</user infinite cycle protection (ICP) functions>****************
//*************</additional functions>****************

