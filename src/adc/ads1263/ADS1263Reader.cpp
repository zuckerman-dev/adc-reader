#include "adc/ads1263/ADS1263Reader.h"

using namespace adc::ads1263;

constexpr auto TEST_ADC =	1;			// ADC Test part
constexpr auto TEST_RTD = 	0;		// RTD Test part
constexpr auto REF		= 5.08;		//Modify according to actual voltage
								//external AVDD and AVSS(Default), or internal 2.5V

AnalogDataReader::AnalogDataReader() : adc::AnalogDataReader(5)
{
    DEV_Module_Init();

    ADS1263_SetMode(1);

    if(ADS1263_init() == 1) 
    {
        DEV_Module_Exit();
        exit(0);
    }
}

AnalogDataReader::~AnalogDataReader() 
{
    DEV_Module_Exit();
}

adc::Signal AnalogDataReader::getValue(const uint8_t & channel)
{
    if(channel >= channels()) {
        return 0.0;
    }

    UDOUBLE value = ADS1263_GetChannalValue(channel);

    if((value >> 31) == 1) {
        return REF * 2 - value / 2147483648.0 * REF;		//7fffffff + 1
    }

    return value / 2147483647.0 * REF;		//7fffffff
}

adc::SignalData AnalogDataReader::readData()
{
    return adc::SignalData{getTimePoint(), { getValue(0), getValue(1), getValue(2), getValue(3), getValue(3) } };
}
