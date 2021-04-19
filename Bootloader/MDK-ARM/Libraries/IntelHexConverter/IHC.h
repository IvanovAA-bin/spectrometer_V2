#ifndef _IHC_H_
#define _IHC_H_

#include "main.h"
#define IHC_MEMORY_UNITS 8

enum IH_FormatType
{
	IH_DataType = 0, // string contains data record 
	IH_LastEntryType, // string indicates end-of-file record
	IH_ExtSegAddrType, // string contains extended segment address record
	IH_ExtLinAddrType, // string contains extended linear address record
	IH_StartLinAddrType, // string contains start linear address record
	
	IH_UnknownType
} ;

struct IH_MemoryUnit
{
	uint32_t Address;
	uint32_t Data;
} ;

struct IH_Data
{
	uint8_t IsIHexString; // Is intel hex string (bool)
	uint8_t IsCheckSumError; // CRC is ok or not. 0 = OK. !0 = FAIL
	enum IH_FormatType DataType; // data type
	uint8_t ByteDataLength; // data (in bytes) length
	uint8_t ByteData[IHC_MEMORY_UNITS * 8]; // data in bytes
	uint8_t MemUnitLength; //
	struct IH_MemoryUnit MemUnits[IHC_MEMORY_UNITS]; //
	uint32_t LowerAddress;
};


void IH_ProcessString(struct IH_Data *data, char *buf); // ":llaaaatt[dd...dd]cc\n"



#endif
