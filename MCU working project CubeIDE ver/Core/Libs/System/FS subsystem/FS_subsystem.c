#include "FS_subsystem.h"
#include "ff.h"
#include "main.h"

#define CSV_DATA_FOLDER "/CsvData"

static FATFS _fs;
static struct _FS *_SFS;

/*static*/ uint16_t _fs_strLen(char *str)
{
	uint16_t len = 0;
	while(str[len++] != 0 && len != 0xFFFF);
	if (len == 0xFFFF)
		Error_Handler(); // @error_state !!!!
	return len - 1;
}

/*static*/ void _fs_attachStrings(char* str1, char* str2, char *result, uint16_t maxLen)
{
	uint16_t len1 = _fs_strLen(str1);
	uint16_t len2 = _fs_strLen(str2);
	uint16_t i, resLen = len1 + len2;
	if (len1 + len2 > maxLen)
		Error_Handler();
	
	for (i = 0; i < len1; i++)
		result[i] = str1[i];
	for (i = len1; i <= resLen; i++)
		result[i] = str2[i - len1];
}

/*static*/ void _fs_convertSpectValToStr(uint32_t data, uint32_t index, char *buf, uint16_t bufsize)
{
	uint8_t d_len = 0, i_len = 0;
	uint32_t c_data = data, c_index = index;
	while(c_index)
	{
		++i_len;
		c_index /= 10;
	}
	while(c_data)
	{
		++d_len;
		c_data /= 10;
	}
	if (!d_len)
		d_len = 1;
	if (!i_len)
		i_len = 1;
	
	if (i_len + c_data + 3 > bufsize)
		Error_Handler();
	c_data = d_len;
	c_index = i_len;
	while(c_index)
	{
		uint8_t ost = index % 10 + 48;
		buf[c_index - 1] = ost;
		--c_index;
		index /= 10;
	}
	buf[i_len] = ',';
	while(c_data)
	{
		uint8_t ost = data % 10 + 48;
		buf[i_len + c_data] = ost;
		--c_data;
		data /= 10;
	}
	buf[i_len + d_len + 1] = '\n';
	buf[i_len + d_len + 2] = 0;
}

/*static*/ void __FS_saveFmwInfoAndInitFilStruct (char *str, uint8_t firstLoad)
{
	if (!_SFS->_isInitialized || str == NULL)
		return;
	
	FILINFO fno;
	DIR dr;
	if (firstLoad)
	{
		f_findfirst(&dr, &fno, "/", "fmw_info.txt");
		if (fno.fname[0] == 0)
		{
			FIL file;
			f_open(&file, "fmw_info.txt", FA_WRITE | FA_CREATE_ALWAYS);
			uint16_t len = _fs_strLen(str);
			UINT bwr;
			f_write(&file, str, len, &bwr);
			f_close(&file);
		}
	}
	
	f_mkdir(CSV_DATA_FOLDER);
}

// STATIC FUNCTIONS END


void __FS_initialize (char *info, uint8_t firstLoad)
{
	if (_SFS->_isInitialized) // @@@
		return;
	FRESULT res;
	res = f_mount(&_fs, "", 1);
	if (res != FR_OK)
		_SFS->_isInitialized = 0;
	else
	{
		_SFS->_isInitialized = 1;
		if (firstLoad)
			__FS_saveFmwInfoAndInitFilStruct(info, firstLoad);
	}
}

void __FS_saveRadInfo_csv (char *file_name, uint32_t *mas, uint16_t size)
{
	if (!_SFS->_isInitialized)
		return;
	
	FIL file;
	FRESULT res;
	static char fullPath[88];
	static char namePath[64];
	_fs_attachStrings("/", file_name, namePath, 64);
	_fs_attachStrings(CSV_DATA_FOLDER, namePath, fullPath, 88);
	
	res = f_open(&file, fullPath, FA_WRITE | FA_CREATE_ALWAYS);
	if (res != FR_OK)
		return;
	char str_buf[64];
	for (uint32_t i = 0; i < 4096; i++)
	{
		_fs_convertSpectValToStr(mas[i], i, str_buf, 64);
		f_puts(str_buf, &file);
	}	
	f_close(&file);
}

static char _fs_getHexAsChar(uint8_t val)
{
	val = val & 0x0F;
	if (val > 9)
		return val + 55;
	else
		return val + 48;
}

static void _fs_prepareSysSetStr(char *str, uint8_t cmd, uint8_t dataLen)
{
	str[0] = '#';

	str[1] = _fs_getHexAsChar(cmd >> 4);
	str[2] = _fs_getHexAsChar(cmd & 0x0F);
	str[3] = _fs_getHexAsChar(dataLen >> 4);
	str[4] = _fs_getHexAsChar(dataLen & 0x0F);
	str[5] = '#';
	str[6 + dataLen] = '#';
	str[7 + dataLen] = 0;
}

static uint8_t _fs_convertUint32ToChar(uint32_t val, char *buff)
{
	uint8_t len;
	uint32_t copy = val;
	uint8_t res;
	while(copy)
	{
		len++;
		copy /= 10;
	}
	if (!len)
		len = 1;
	res = len;
	buff[len] = 0;
	while(len)
	{
		--len;
		uint8_t ost = val % 10;
		buff[len] = ost + 48;
		val /= 10;
	}
	return res;
}

void __FS_saveSystemSettingsInfo (struct SystemSettings* settings)
{
	if (!_SFS->_isInitialized)
		return;

	FIL file;
	FRESULT res;
	char str[256];

	res = f_open(&file, "cfgr.txt", FA_WRITE | FA_CREATE_ALWAYS);
	if (res != FR_OK)
		return;
	{ // saving device name
		str[6] = settings->deviceNumber[0];
		str[7] = settings->deviceNumber[1];
		_fs_prepareSysSetStr(str, 1, 2);
		_fs_attachStrings(str, " @device name numbers\n", str, 240);
		f_puts(str, &file);
	}
	{ // saving dma impulse counts
		char valBuffer[16];
		uint16_t i;
		uint8_t valueLen = _fs_convertUint32ToChar(settings->countsToDma, valBuffer);
		for (i = 0; i < valueLen; i++)
			str[6 + i] = valBuffer[i];
		_fs_prepareSysSetStr(str, 2, valueLen);
		_fs_attachStrings(str, " @ADC counts on one impulse\n", str, 240);
		f_puts(str, &file);
	}
	{ // saving min filtration value
		char valBuffer[16];
		uint16_t i;
		uint8_t valueLen = _fs_convertUint32ToChar(settings->minValueFiltration, valBuffer);
		for (i = 0; i < valueLen; i++)
			str[6 + i] = valBuffer[i];
		_fs_prepareSysSetStr(str, 3, valueLen);
		_fs_attachStrings(str, " @min filtration value\n", str, 240);
		f_puts(str, &file);
	}
	{ // saving max filtration value
		char valBuffer[16];
		uint16_t i;
		uint8_t valueLen = _fs_convertUint32ToChar(settings->maxValueFiltration, valBuffer);
		for (i = 0; i < valueLen; i++)
			str[6 + i] = valBuffer[i];
		_fs_prepareSysSetStr(str, 4, valueLen);
		_fs_attachStrings(str, " @max filtration value\n", str, 240);
		f_puts(str, &file);
	}
	f_close(&file);
}

static uint8_t _fs_fromCharToUint8_t (char c)
{
	if (c >= '0' && c <= '9')
		return (c - 48) & 0x0F;
	if (c >= 'a' && c <= 'z')
		return (c - 97 + 10) & 0x0F;
	if (c >= 'A' && c <= 'Z')
		return (c - 65 + 10) & 0x0F;
	return 0;
}

static uint8_t _fs_processCfgrDataString(char *str, uint8_t *cmd, uint8_t *dataLen) // return value true - error, false - OK occured
{
	uint16_t strLen = _fs_strLen(str);
	if (strLen < 6)
		return 1;
	if (str[0] != '#' || str[5] != '#')
		return 1;
	*cmd = (_fs_fromCharToUint8_t(str[1]) << 4) + _fs_fromCharToUint8_t(str[2]);
	*dataLen = (_fs_fromCharToUint8_t(str[3]) << 4) + _fs_fromCharToUint8_t(str[4]);
	if (str[6 + (*dataLen)] != '#')
		return 1;
	return 0;
}

static uint32_t _fs_getUint32FromChar(uint32_t defaultValue, char *buff)
{
	uint32_t retValue = 0;
	uint8_t i = 0;
	while(buff[i] >= '0' && buff[i] <= '9' && i < 10)
	{
		uint8_t actualValue = buff[i] - 48;
		retValue = retValue * 10 + actualValue;
		++i;
	}
	if (i == 10)
		retValue = defaultValue;
	return retValue;
}

void __FS_downloadSystemSettingsInfo (struct SystemSettings* settings)
{
	if (!_SFS->_isInitialized)
		return;

	FILINFO fno;
	DIR dr;

	f_findfirst(&dr, &fno, "/", "cfgr.txt");
	if (fno.fname[0] == 0)
	{
		__FS_saveSystemSettingsInfo(settings);
	}
	else
	{
		FIL file;
		char buff[256], *retValue;
		uint8_t cmd, dataLen;
		FRESULT res = f_open(&file, "cfgr.txt", FA_READ | FA_OPEN_EXISTING);
		if (res != FR_OK)
			return;

		do
		{
			retValue = f_gets(buff, 250, &file);
			if (!retValue)
				break;
			if (!_fs_processCfgrDataString(buff, &cmd, &dataLen))
			{
				switch (cmd)
				{
				case 1: // setting device name
					{
						if (dataLen != 2)
							continue;
						char c1 = buff[6];
						char c2 = buff[7];
						if (!((c1 >= '0' && c1 <= '9') || (c1 >= 'a' && c1 <= 'z') || (c1 >= 'A' && c1 <= 'Z')))
							continue;
						if (!((c2 >= '0' && c2 <= '9') || (c2 >= 'a' && c2 <= 'z') || (c2 >= 'A' && c2 <= 'Z')))
							continue;
						settings->deviceNumber[0] = c1;
						settings->deviceNumber[1] = c2;
					}
					break;
				case 2: // setting dma impulse counts
					{
						uint16_t dma_counts = _fs_getUint32FromChar(128, &buff[6]);
						if (dma_counts < 5 || dma_counts > 1000)
							continue;
						settings->countsToDma = dma_counts;
					}
					break;
				case 3: // setting min filtration value
					{
						uint16_t val = _fs_getUint32FromChar(0, &buff[6]);
						settings->minValueFiltration = val;
					}
					break;
				case 4: // setting max filtration value
					{
						uint16_t val = _fs_getUint32FromChar(0, &buff[6]);
						settings->maxValueFiltration = val;
					}
					break;
				default:
					continue;
					break;
				}
			}
		} while(retValue);

		f_close(&file);
	}

}


void _init_fs_subsystem(struct _FS* fs)
{
	_SFS = fs;
	_SFS->_isInitialized = 0;
	_SFS->_initializationTime = 0xFFFFFFFF;
	fs->initialize = __FS_initialize;
	fs->saveRadInfo_csv = __FS_saveRadInfo_csv;
	fs->saveSystemSettingsInfo = __FS_saveSystemSettingsInfo;
	fs->downloadSystemSettingsInfo = __FS_downloadSystemSettingsInfo;
}

