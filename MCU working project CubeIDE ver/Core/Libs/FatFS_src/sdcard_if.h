#ifndef __SDCARD_IF_H__
#define __SDCARD_IF_H__


/*
* Пользователю необходимо переписать некоторые функции,
* находящиеся в sdcard.c
* См сишный файл
*
* Также нужно поставть библиотеку CMSIS снизу под свой МК
*/
//#define CMSIS_LIB "stm32f10x.h"


#include "stm32wb55xx.h"
// call before initializing any SPI devices
//void SDCARD_Unselect(void);

// all procedures return 0 on success, < 0 on failure

int SDCARD_Init(void);
int SDCARD_GetBlocksNumber(uint32_t* num);
int SDCARD_ReadSingleBlock(uint32_t blockNum, uint8_t* buff); // sizeof(buff) == 512!
int SDCARD_WriteSingleBlock(uint32_t blockNum, const uint8_t* buff); // sizeof(buff) == 512!

// Read Multiple Blocks
int SDCARD_ReadBegin(uint32_t blockNum);
int SDCARD_ReadData(uint8_t* buff); // sizeof(buff) == 512!
int SDCARD_ReadEnd(void);

// Write Multiple Blocks
int SDCARD_WriteBegin(uint32_t blockNum);
int SDCARD_WriteData(const uint8_t* buff); // sizeof(buff) == 512!
int SDCARD_WriteEnd(void);

// TODO: read lock flag? CMD13, SEND_STATUS

#endif // __SDCARD_H__
