#include "IHC.h"

static uint16_t ihc_getstrlen(char *buf)
{
	uint16_t len = 0;
	while(buf[len] != 0 && len < 1024)
		len++;
	return len;
}

static uint8_t AsciiToHex(char c)
{
	if(c <= '9' && c >= '0' )
		return c - 0x30;
	else
		return  c - 0x41 + 10;
}

static uint8_t ihc_getbyte(char *buf, uint16_t index)
{
	return (AsciiToHex(buf[index]) << 4) | (AsciiToHex(buf[index+1])&0x0F);
}

static void ihc_resetdata(struct IH_Data *data)
{
	uint8_t i;
	data->DataType = IH_UnknownType;
	data->IsIHexString = 0;
	data->IsCheckSumError = 0;
	data->MemUnitLength = 0;
	data->ByteDataLength = 0;
	data->LowerAddress = 0;
	for (i = 0; i < IHC_MEMORY_UNITS; i++)
	{
		data->MemUnits[i].Address = 0;
		data->MemUnits[i].Data = 0xFFFFFFFF;
	}
	for (i = 0; i < IHC_MEMORY_UNITS * 8; i++)
	{
		data->ByteData[i] = 0xFF;
	}
}

static uint8_t ihc_ischecksumerror(char *buf, uint16_t len)
{
	uint8_t stringCheckSum = ihc_getbyte(buf, len - 3);
	uint16_t i;
	uint8_t calculatedCheckSum = 0;
	for (i = 1; i < len - 3; i += 2)
		calculatedCheckSum += ihc_getbyte(buf, i);
	calculatedCheckSum = ~calculatedCheckSum;
	++calculatedCheckSum;
	if (stringCheckSum == calculatedCheckSum)
		return 0;
	else
		return 1;
}

void IH_ProcessString(struct IH_Data *data, char *buf)
{
	uint16_t len = ihc_getstrlen(buf);
	uint8_t dataType;
	uint8_t i;
	ihc_resetdata(data);
	if (buf[0] != ':')
	{
		data->IsIHexString = 0;
		return;
	}
	if (len > 128)
	{
		data->IsIHexString = 1;
		data->IsCheckSumError = 1;
		return;
	}
	
	data->IsIHexString = 1;
	if (ihc_ischecksumerror(buf, len))
	{
		data->IsCheckSumError = 1;
		return;
	}
	data->ByteDataLength = ihc_getbyte(buf, 1);
	data->LowerAddress = (ihc_getbyte(buf, 3) << 8) + ihc_getbyte(buf, 5);
	dataType = ihc_getbyte(buf, 7);
	switch (dataType)
	{
		case 0:
			data->DataType = IH_DataType;
		  for (i = 0; i < IHC_MEMORY_UNITS * 8; i++)
				data->ByteData[i] = 0xFF;
			for (i = 0; i < data->ByteDataLength; i++)
				data->ByteData[i] = ihc_getbyte(buf, 9 +i*2);
			data->MemUnitLength = data->ByteDataLength / 4;
			for (i = 0; i < data->MemUnitLength; i++)
			{
				uint8_t sti = 9 + i * 8;
				data->MemUnits[i].Data = ihc_getbyte(buf, sti) + (ihc_getbyte(buf, sti + 2) << 8) + (ihc_getbyte(buf, sti + 4) << 16) + (ihc_getbyte(buf, sti + 6) << 24);
				data->MemUnits[i].Address = data->LowerAddress + i * 4;
			}
			break;
		case 1:
			data->DataType = IH_LastEntryType;
			break;
		case 2: //?????????????????????????????????????
			data->DataType = IH_ExtSegAddrType;
			data->LowerAddress = (ihc_getbyte(buf, 9) << 8) + ihc_getbyte(buf, 11);
			break;
		case 4:
			data->DataType = IH_ExtLinAddrType;
			data->LowerAddress = (ihc_getbyte(buf, 9) << 8) + ihc_getbyte(buf, 11);
			break;
		case 5:
			data->DataType = IH_StartLinAddrType;
			break;
		default:
			data->DataType = IH_UnknownType;
			break;
	}
}
