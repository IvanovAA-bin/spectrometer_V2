#include "ADC_subsystem.h"
#include "main.h"

static uint8_t _adc_dumm = 0;

static void _adc_dummyDelay(void)
{
	++_adc_dumm;
}

static void _adc_enable_adc(void)
{
	ADC1->ISR |= ADC_ISR_ADRDY;
	ADC1->CR |= ADC_CR_ADEN;
	while(!(ADC1->ISR & ADC_ISR_ADRDY));
	ADC1->ISR |= ADC_ISR_ADRDY;
}

static void _adc_disable_adc(void)
{
	while(ADC1->CR & (ADC_CR_ADSTART | ADC_CR_JADSTART));
	ADC1->CR |= ADC_CR_ADDIS;
	while(ADC1->CR & ADC_CR_ADEN);
}


void __adc_calibrateAndReinit (void)
{
	ADC1_COMMON->CCR &= ~ADC_CCR_CKMODE;
	ADC1_COMMON->CCR |= ADC_CCR_CKMODE_0;
	
	ADC1->CFGR |= ADC_CFGR_DMAEN;
	ADC1->CFGR |= ADC_CFGR_CONT;
	
	if ((ADC1->CR & ADC_CR_DEEPPWD) || !(ADC1->CR & ADC_CR_ADVREGEN) || (ADC1->CR & ADC_CR_ADEN))
		Error_Handler(); // !!!!!!!!!!!!!!!!
	ADC1->CR |= ADC_CR_ADCAL;
	while(ADC1->CR & ADC_CR_ADCAL);
	_adc_dummyDelay();
	_adc_enable_adc();
}

void __adc_startBatConversion (void)
{
	ADC1->CR |= ADC_CR_JADSTART;
}

void __adc_startSigConversion (void)
{
	ADC1->CR |= ADC_CR_ADSTART;
}

uint8_t __adc_isBatDataAvailable (void) // optimise ?
{
	uint8_t retval = (ADC1->ISR & ADC_ISR_JEOS)?0x01:0x00;
	if (retval)
		ADC1->ISR |= ADC_ISR_JEOS;
	return retval;
}

uint8_t __adc_isSigDataAvailable (void)
{
	uint8_t retval = (ADC1->ISR & ADC_ISR_EOS)?0x01:0x00;
	if (retval)
		ADC1->ISR |= ADC_ISR_EOS;
	return retval;
}

uint16_t __adc_getSigData (void)
{
	return ADC1->DR;
}

uint16_t __adc_getBatData (void)
{
	return ADC1->JDR1;
}

void __adc_reCalibrate (void)
{
	_adc_disable_adc();
	ADC1->CR |= ADC_CR_ADCAL;
	while(ADC1->CR & ADC_CR_ADCAL);
	_adc_enable_adc();
}


void _init_adc_subsystem(struct _adc *adc)
{
	_adc_dumm = 0;
	adc->calibrateAndReinit = __adc_calibrateAndReinit;
	adc->startBatConversion = __adc_startBatConversion;
	adc->startSigConversion = __adc_startSigConversion;
	adc->isBatDataAvailable = __adc_isBatDataAvailable;
	adc->isSigDataAvailable = __adc_isSigDataAvailable;
	adc->getSigData = __adc_getSigData;
	adc->getBatData = __adc_getBatData;
	adc->reCalibrate = __adc_reCalibrate;
}

