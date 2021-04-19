/* vim: set ai et ts=4 sw=4: */

#include "sdcard_if.h"
#include "main.h"
/*
* Пользовательские низкоуровневые функции работы с SPI
* Их необходимо переопределить
* Например, в main.c (или в любом .c файле) можно написать:
* void USER_SDCARD_Select()
* {
* GPIOA->BSRR |= GPIO_BSRR_BR4;
* }
*/

//*************<required functons>****************

__weak uint8_t USER_SPI_TransmitReceive(uint8_t *transmit, uint8_t *reseive, uint16_t count) // 1 = error. 0 = ok
{
	return 1;
}

__weak void USER_SPI_Transmit(uint8_t *buf, uint16_t size) 
{
	
}

__weak void USER_SDCARD_Select() 
{
	
}

__weak void USER_SDCARD_Unselect()
{
	
}
//*************</required functons>****************
//*************<additional functions>****************
__weak uint8_t USER_SDCARD_isAvailable() // return value: !0 - available; 0 - isn't available
{
	return 1;
}


__weak void USER_SDCARD_isInitialisedCallback() // Callback, which is called after succesfull uSD initialisation
{
	
}




/*
* This function is called, when fatfs wants to know disk status
* return 0 when everything ok
* return 0x01 when drive is not initialized
* return 0x02 when there is no drive right now
*/
__weak uint8_t USER_Return_uSD_Status()
{
	return 0;
}

//*************<user infinite cycle protection (ICP) functions>****************
/*
* it is used to protect from infinite cycles by:
*
* USER_ICP_ResetCondition();
* while(... && USER_ICP_CheckCondition())
* {
* 	...
* }
* if (USER_ICP_ConditionValidation()) 
*		ERROR CONDITION
*
*
* 
*/
__weak void USER_ICP_ResetCondition() // Reset the condition (example: start timer)
{
	
}
__weak uint8_t USER_ICP_CheckCondition() // Check the condition. 1 = continue cycle. 0 = end cycle (example: timer value < SET_VALUE)
{
	return 1;
}

__weak uint8_t USER_ICP_ConditionValidation() // if condition happened, return 1 (means error), return 0 if everything ok (example: timer value >= SET_VALUE means error)
{
	return 0;
}
//*************</user infinite cycle protection (ICP) functions>****************
//*************</additional functions>****************

/* Конец пользовательских функций */

/*
R1: 0abcdefg
     ||||||`- 1th bit (g): card is in idle state
     |||||`-- 2th bit (f): erase sequence cleared
     ||||`--- 3th bit (e): illigal command detected
     |||`---- 4th bit (d): crc check error
     ||`----- 5th bit (c): error in the sequence of erase commands
     |`------ 6th bit (b): misaligned addres used in command
     `------- 7th bit (a): command argument outside allowed range
             (8th bit is always zero)
*/
/*static*/ uint8_t SDCARD_ReadR1() { // REPLACE WITH CHECKING MECHANIZM
    uint8_t r1;
    // make sure FF is transmitted during receive
    uint8_t tx = 0xFF;
		USER_ICP_ResetCondition();
    for(;USER_ICP_CheckCondition();) {
        if (USER_SPI_TransmitReceive(&tx, &r1, sizeof(r1)))
					return 0xFF;
        if((r1 & 0x80) == 0) // 8th bit alwyas zero, r1 recevied
            break;
    }
		if (USER_ICP_ConditionValidation())
			return 0xFF;
    return r1;
}

// data token for CMD9, CMD17, CMD18 and CMD24 are the same
#define DATA_TOKEN_CMD9  0xFE
#define DATA_TOKEN_CMD17 0xFE
#define DATA_TOKEN_CMD18 0xFE
#define DATA_TOKEN_CMD24 0xFE
#define DATA_TOKEN_CMD25 0xFC

/*static*/ int SDCARD_WaitDataToken(uint8_t token) { // REPLACE WITH CHECKING MECHANIZM
    uint8_t fb;
    // make sure FF is transmitted during receive
    uint8_t tx = 0xFF;
		USER_ICP_ResetCondition();
    for(;USER_ICP_CheckCondition();) {
        if (USER_SPI_TransmitReceive(&tx, &fb, sizeof(fb)))
					return -1;
        if(fb == token)
            break;

        if(fb != 0xFF)
            return -1;
    }
		if (USER_ICP_ConditionValidation())
			return -30;
    return 0;
}

/*static*/ int SDCARD_ReadBytes(uint8_t* buff, uint16_t buff_size) { // REPLACE WITH CHECKING MECHANIZM
    // make sure FF is transmitted during receive
    uint8_t tx = 0xFF;
    while(buff_size > 0) {
        if (USER_SPI_TransmitReceive(&tx, buff, 1))
					return -1;
        buff++;
        buff_size--;
    }

    return 0;
}

/*static*/ int SDCARD_WaitNotBusy() { // REPLACE WITH CHECKING MECHANIZM
    uint8_t busy;
		USER_ICP_ResetCondition();
    do {
        if(SDCARD_ReadBytes(&busy, sizeof(busy)) < 0) {
            return -1;
        }
    } while(busy != 0xFF && USER_ICP_CheckCondition() == 1);
		if (USER_ICP_ConditionValidation())
			return -30;
    return 0;
}
 
int SDCARD_Init() {
    /*
    Step 1.
    Set DI and CS high and apply 74 or more clock pulses to SCLK. Without this
    step under certain circumstances SD-card will not work. For instance, when
    multiple SPI devices are sharing the same bus (i.e. MISO, MOSI, CS).
    */
		//==========================
			if (!USER_SDCARD_isAvailable())
				return -29;
		//==========================
			
    USER_SDCARD_Unselect();

    uint8_t high = 0xFF;
    for(int i = 0; i < 10; i++) { // 80 clock pulses
        USER_SPI_Transmit(&high, sizeof(high));
    }

    USER_SDCARD_Select();
		
		

    /*
    Step 2.
    
    Send CMD0 (GO_IDLE_STATE): Reset the SD card.
    */
    if(SDCARD_WaitNotBusy() < 0) { // keep this!
        USER_SDCARD_Unselect();
        return -1;
    }

    {
        /*static const*/ uint8_t cmd[] =
            { 0x40 | 0x00 /* CMD0 */, 0x00, 0x00, 0x00, 0x00 /* ARG = 0 */, (0x4A << 1) | 1 /* CRC7 + end bit */ };
        USER_SPI_Transmit((uint8_t*)cmd, sizeof(cmd));
    }

    if(SDCARD_ReadR1() != 0x01) {
        USER_SDCARD_Unselect();
        return -1;
    }

    /*
    Step 3.
    After the card enters idle state with a CMD0, send a CMD8 with argument of
    0x000001AA and correct CRC prior to initialization process. If the CMD8 is
    rejected with illigal command error (0x05), the card is SDC version 1 or
    MMC version 3. If accepted, R7 response (R1(0x01) + 32-bit return value)
    will be returned. The lower 12 bits in the return value 0x1AA means that
    the card is SDC version 2 and it can work at voltage range of 2.7 to 3.6
    volts. If not the case, the card should be rejected.
    */
    if(SDCARD_WaitNotBusy() < 0) { // keep this!
        USER_SDCARD_Unselect();
        return -1;
    }

    {
        /*static const*/ uint8_t cmd[] =
            { 0x40 | 0x08 /* CMD8 */, 0x00, 0x00, 0x01, 0xAA /* ARG */, (0x43 << 1) | 1 /* CRC7 + end bit */ };
        USER_SPI_Transmit((uint8_t*)cmd, sizeof(cmd));
    }

    if(SDCARD_ReadR1() != 0x01) {
        USER_SDCARD_Unselect();
        return -2; // not an SDHC/SDXC card (i.e. SDSC, not supported)
    }

    {
        uint8_t resp[4];
        if(SDCARD_ReadBytes(resp, sizeof(resp)) < 0) {
            USER_SDCARD_Unselect();
            return -3;
        }

        if(((resp[2] & 0x01) != 1) || (resp[3] != 0xAA)) {
            USER_SDCARD_Unselect();
            return -4;
        }
    }

    /*
    Step 4.
    And then initiate initialization with ACMD41 with HCS flag (bit 30).
    */
		/*USER_ICP_ResetCondition();*/
    for(;/*USER_ICP_CheckCondition()*/;) {
        if(SDCARD_WaitNotBusy() < 0) { // keep this!
            USER_SDCARD_Unselect();
            return -1;
        }

        {
            /*static const*/ uint8_t cmd[] =
                { 0x40 | 0x37 /* CMD55 */, 0x00, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 /* CRC7 + end bit */ };
            USER_SPI_Transmit((uint8_t*)cmd, sizeof(cmd));
        }

        if(SDCARD_ReadR1() != 0x01) {
            USER_SDCARD_Unselect();
            return -5;
        }

        if(SDCARD_WaitNotBusy() < 0) { // keep this!
            USER_SDCARD_Unselect();
            return -1;
        }

        {
            /*static const*/ uint8_t cmd[] =
                { 0x40 | 0x29 /* ACMD41 */, 0x40, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 /* CRC7 + end bit */ };
            USER_SPI_Transmit((uint8_t*)cmd, sizeof(cmd));
        }

        uint8_t r1 = SDCARD_ReadR1();
        if(r1 == 0x00) {
            break;
        }

        if(r1 != 0x01) {
            USER_SDCARD_Unselect();
            return -6;
        }
    }
		/*if (USER_ICP_ConditionValidation())
			return -30;*/
    /*
    Step 5.
    After the initialization completed, read OCR register with CMD58 and check
    CCS flag (bit 30). When it is set, the card is a high-capacity card known
    as SDHC/SDXC.
    */
    if(SDCARD_WaitNotBusy() < 0) { // keep this!
        USER_SDCARD_Unselect();
        return -1;
    }

    {
        /*static const*/ uint8_t cmd[] =
            { 0x40 | 0x3A /* CMD58 */, 0x00, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 /* CRC7 + end bit */ };
        USER_SPI_Transmit((uint8_t*)cmd, sizeof(cmd));
    }

    if(SDCARD_ReadR1() != 0x00) {
        USER_SDCARD_Unselect();
        return -7;
    }

    {
        uint8_t resp[4];
        if(SDCARD_ReadBytes(resp, sizeof(resp)) < 0) {
            USER_SDCARD_Unselect();
            return -8;
        }

        if((resp[0] & 0xC0) != 0xC0) {
            USER_SDCARD_Unselect();
            return -9;
        }
    }

    USER_SDCARD_Unselect();
		USER_SDCARD_isInitialisedCallback();
    return 0;
}


int SDCARD_GetBlocksNumber(uint32_t* num) {
    uint8_t csd[16];
    uint8_t crc[2];
	
		//==========================
			if (!USER_SDCARD_isAvailable())
				return -29;
		//==========================

    USER_SDCARD_Select();

    if(SDCARD_WaitNotBusy() < 0) { // keep this!
        USER_SDCARD_Unselect();
        return -1;
    }

    /* CMD9 (SEND_CSD) command */
    {
        /*static const*/ uint8_t cmd[] =
            { 0x40 | 0x09 /* CMD9 */, 0x00, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 /* CRC7 + end bit */ };
        USER_SPI_Transmit((uint8_t*)cmd, sizeof(cmd));
    }

    if(SDCARD_ReadR1() != 0x00) {
        USER_SDCARD_Unselect();
        return -2;
    }

    if(SDCARD_WaitDataToken(DATA_TOKEN_CMD9) < 0) {
        USER_SDCARD_Unselect();
        return -3;
    }

    if(SDCARD_ReadBytes(csd, sizeof(csd)) < 0) {
        USER_SDCARD_Unselect();
        return -4;
    }

    if(SDCARD_ReadBytes(crc, sizeof(crc)) < 0) {
        USER_SDCARD_Unselect();
        return -5;
    }

    USER_SDCARD_Unselect();

    // first byte is VVxxxxxxxx where VV is csd.version
    if((csd[0] & 0xC0) != 0x40) // csd.version != 1
        return -6;

    uint32_t tmp = csd[7] & 0x3F; // two bits are reserved
    tmp = (tmp << 8) | csd[8];
    tmp = (tmp << 8) | csd[9];
    // Full volume: (C_SIZE+1)*512KByte == (C_SIZE+1)<<19
    // Block size: 512Byte == 1<<9
    // Blocks number: CARD_SIZE/BLOCK_SIZE = (C_SIZE+1)*(1<<19) / (1<<9) = (C_SIZE+1)*(1<<10)
    tmp = (tmp + 1) << 10;
    *num = tmp;

    return 0;
}

int SDCARD_ReadSingleBlock(uint32_t blockNum, uint8_t* buff) {
    uint8_t crc[2];
	
		//==========================
			if (!USER_SDCARD_isAvailable())
				return -29;
		//==========================

    USER_SDCARD_Select();

    if(SDCARD_WaitNotBusy() < 0) { // keep this!
        USER_SDCARD_Unselect();
        return -1;
    }

    /* CMD17 (SEND_SINGLE_BLOCK) command */
    uint8_t cmd[] = {
        0x40 | 0x11 /* CMD17 */,
        (blockNum >> 24) & 0xFF, /* ARG */
        (blockNum >> 16) & 0xFF,
        (blockNum >> 8) & 0xFF,
        blockNum & 0xFF,
        (0x7F << 1) | 1 /* CRC7 + end bit */
    };
    USER_SPI_Transmit((uint8_t*)cmd, sizeof(cmd));

    if(SDCARD_ReadR1() != 0x00) {
        USER_SDCARD_Unselect();
        return -2;
    }

    if(SDCARD_WaitDataToken(DATA_TOKEN_CMD17) < 0) {
        USER_SDCARD_Unselect();
        return -3;
    }

    if(SDCARD_ReadBytes(buff, 512) < 0) {
        USER_SDCARD_Unselect();
        return -4;
    }

    if(SDCARD_ReadBytes(crc, 2) < 0) {
        USER_SDCARD_Unselect();
        return -5;
    }

    USER_SDCARD_Unselect();
    return 0;
}


int SDCARD_WriteSingleBlock(uint32_t blockNum, const uint8_t* buff) {
		//==========================
			if (!USER_SDCARD_isAvailable())
				return -29;
		//==========================
    USER_SDCARD_Select();

    if(SDCARD_WaitNotBusy() < 0) { // keep this!
        USER_SDCARD_Unselect();
        return -1;
    }

    /* CMD24 (WRITE_BLOCK) command */
    uint8_t cmd[] = {
        0x40 | 0x18 /* CMD24 */,
        (blockNum >> 24) & 0xFF, /* ARG */
        (blockNum >> 16) & 0xFF,
        (blockNum >> 8) & 0xFF,
        blockNum & 0xFF,
        (0x7F << 1) | 1 /* CRC7 + end bit */
    };
    USER_SPI_Transmit((uint8_t*)cmd, sizeof(cmd));

    if(SDCARD_ReadR1() != 0x00) {
        USER_SDCARD_Unselect();
        return -2;
    }

    uint8_t dataToken = DATA_TOKEN_CMD24;
    uint8_t crc[2] = { 0xFF, 0xFF };
    USER_SPI_Transmit(&dataToken, sizeof(dataToken));
    USER_SPI_Transmit((uint8_t*)buff, 512);
    USER_SPI_Transmit(crc, sizeof(crc));

    /*
        dataResp:
        xxx0abc1
            010 - Data accepted
            101 - Data rejected due to CRC error
            110 - Data rejected due to write error
    */
    uint8_t dataResp;
    SDCARD_ReadBytes(&dataResp, sizeof(dataResp));
    if((dataResp & 0x1F) != 0x05) { // data rejected
        USER_SDCARD_Unselect();
        return -3;
    }

    if(SDCARD_WaitNotBusy() < 0) {
        USER_SDCARD_Unselect();
        return -4;
    }

    USER_SDCARD_Unselect();
    return 0;
}

int SDCARD_ReadBegin(uint32_t blockNum) {
		//==========================
			if (!USER_SDCARD_isAvailable())
				return -29;
		//==========================
    USER_SDCARD_Select();

    if(SDCARD_WaitNotBusy() < 0) { // keep this!
        USER_SDCARD_Unselect();
        return -1;
    }

    /* CMD18 (READ_MULTIPLE_BLOCK) command */
    uint8_t cmd[] = {
        0x40 | 0x12 /* CMD18 */,
        (blockNum >> 24) & 0xFF, /* ARG */
        (blockNum >> 16) & 0xFF,
        (blockNum >> 8) & 0xFF,
        blockNum & 0xFF,
        (0x7F << 1) | 1 /* CRC7 + end bit */
    };
    USER_SPI_Transmit((uint8_t*)cmd, sizeof(cmd));

    if(SDCARD_ReadR1() != 0x00) {
        USER_SDCARD_Unselect();
        return -2;
    }

    USER_SDCARD_Unselect();
    return 0;
}

int SDCARD_ReadData(uint8_t* buff) {
		//==========================
			if (!USER_SDCARD_isAvailable())
				return -29;
		//==========================
    uint8_t crc[2];
    USER_SDCARD_Select();

    if(SDCARD_WaitDataToken(DATA_TOKEN_CMD18) < 0) {
        USER_SDCARD_Unselect();
        return -1;
    }

    if(SDCARD_ReadBytes(buff, 512) < 0) {
        USER_SDCARD_Unselect();
        return -2;
    }

    if(SDCARD_ReadBytes(crc, 2) < 0) {
        USER_SDCARD_Unselect();
        return -3;
    }

    USER_SDCARD_Unselect();
    return 0;

}

int SDCARD_ReadEnd() {
		//==========================
			if (!USER_SDCARD_isAvailable())
				return -29;
		//==========================
    USER_SDCARD_Select();

    /* CMD12 (STOP_TRANSMISSION) */
    {
        /*static const*/ uint8_t cmd[] = { 0x40 | 0x0C /* CMD12 */, 0x00, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 };
        USER_SPI_Transmit((uint8_t*)cmd, sizeof(cmd));
    }

    /*
    The received byte immediataly following CMD12 is a stuff byte, it should be
    discarded before receive the response of the CMD12
    */
    uint8_t stuffByte;
    if(SDCARD_ReadBytes(&stuffByte, sizeof(stuffByte)) < 0) {
        USER_SDCARD_Unselect();
        return -1;
    }

    if(SDCARD_ReadR1() != 0x00) {
        USER_SDCARD_Unselect();
        return -2;
    }
    
    USER_SDCARD_Unselect();
    return 0;
}


int SDCARD_WriteBegin(uint32_t blockNum) {
		//==========================
			if (!USER_SDCARD_isAvailable())
				return -29;
		//==========================
    USER_SDCARD_Select();

    if(SDCARD_WaitNotBusy() < 0) { // keep this!
        USER_SDCARD_Unselect();
        return -1;
    }

    /* CMD25 (WRITE_MULTIPLE_BLOCK) command */
    uint8_t cmd[] = {
        0x40 | 0x19 /* CMD25 */,
        (blockNum >> 24) & 0xFF, /* ARG */
        (blockNum >> 16) & 0xFF,
        (blockNum >> 8) & 0xFF,
        blockNum & 0xFF,
        (0x7F << 1) | 1 /* CRC7 + end bit */
    };
    USER_SPI_Transmit((uint8_t*)cmd, sizeof(cmd));

    if(SDCARD_ReadR1() != 0x00) {
        USER_SDCARD_Unselect();
        return -2;
    }

    USER_SDCARD_Unselect();
    return 0;
}

int SDCARD_WriteData(const uint8_t* buff) {
		//==========================
			if (!USER_SDCARD_isAvailable())
				return -29;
		//==========================
    USER_SDCARD_Select();

    uint8_t dataToken = DATA_TOKEN_CMD25;
    uint8_t crc[2] = { 0xFF, 0xFF };
    USER_SPI_Transmit(&dataToken, sizeof(dataToken));
    USER_SPI_Transmit((uint8_t*)buff, 512);
    USER_SPI_Transmit(crc, sizeof(crc));

    /*
        dataResp:
        xxx0abc1
            010 - Data accepted
            101 - Data rejected due to CRC error
            110 - Data rejected due to write error
    */
    uint8_t dataResp;
    SDCARD_ReadBytes(&dataResp, sizeof(dataResp));
    if((dataResp & 0x1F) != 0x05) { // data rejected
        USER_SDCARD_Unselect();
        return -1;
    }

    if(SDCARD_WaitNotBusy() < 0) {
        USER_SDCARD_Unselect();
        return -2;
    }

    USER_SDCARD_Unselect();
    return 0;
}

int SDCARD_WriteEnd() {
		//==========================
			if (!USER_SDCARD_isAvailable())
				return -29;
		//==========================
    USER_SDCARD_Select();

    uint8_t stopTran = 0xFD; // stop transaction token for CMD25
    USER_SPI_Transmit(&stopTran, sizeof(stopTran));

    // skip one byte before readyng "busy"
    // this is required by the spec and is necessary for some real SD-cards!
    uint8_t skipByte;
    SDCARD_ReadBytes(&skipByte, sizeof(skipByte));

    if(SDCARD_WaitNotBusy() < 0) {
        USER_SDCARD_Unselect();
        return -1;
    }

    USER_SDCARD_Unselect();
    return 0;
}
