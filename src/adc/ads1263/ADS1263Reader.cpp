#include "adc/ads1263/ADS1263Reader.h"

#include <cassert>

using namespace adc::ads1263;

constexpr auto REF		= 5.08;		//Modify according to actual voltage
								//external AVDD and AVSS(Default), or internal 2.5V

AnalogDataReader::AnalogDataReader() : adc::AnalogDataReader(ADS1263::Channels)
{
    DEV_Module_Init();

    ads1263_hal.SetMode(1);

    if(ads1263_hal.init() == 1) 
    {
        DEV_Module_Exit();
        setInitialized(false);
    }
    
    setInitialized(true);
}

AnalogDataReader::~AnalogDataReader() 
{
    DEV_Module_Exit();
}

adc::Signal AnalogDataReader::getValue(const uint8_t & channel)
{
    // if(channel >= channels()) {
    //     return 0.0;
    // }

    UDOUBLE value{}; 

    assert(channel <= 4);

	ads1263_hal.WriteReg(ADS1263::Reg::REG_INPMUX, ((channel*2 << 4) | (channel*2+1))); 	

	DEV_Delay_ms(2);
	ads1263_hal.WriteCmd(ADS1263::Cmd::CMD_START1);
	DEV_Delay_ms(2);
	ads1263_hal.WaitDRDY();

    UBYTE buf[4] = {0, 0, 0, 0};
	UBYTE Status, CRC;
    DEV_Digital_Write(DEV_CS_PIN, 0);
	do {
		DEV_SPI_WriteByte(ADS1263::Cmd::CMD_RDATA1);
		DEV_Delay_ms(10);
		Status = DEV_SPI_ReadByte();
	}while((Status & 0x40) == 0);
	
    buf[0] = DEV_SPI_ReadByte();
    buf[1] = DEV_SPI_ReadByte();
    buf[2] = DEV_SPI_ReadByte();
    buf[3] = DEV_SPI_ReadByte();
	CRC = DEV_SPI_ReadByte();

    DEV_Digital_Write(DEV_CS_PIN, 1);
    value |= ((UDOUBLE)buf[0] << 24);
    value |= ((UDOUBLE)buf[1] << 16);
    value |= ((UDOUBLE)buf[2] << 8);
	value |= (UDOUBLE)buf[3];

    if((value >> 31) == 1) {
        return REF * 2 - value / 2147483648.0 * REF;		//7fffffff + 1
    }

    return value / 2147483647.0 * REF;		//7fffffff
}

adc::SignalData AnalogDataReader::readData()
{
    return adc::SignalData{getTimePoint(), { getValue(0), getValue(1), getValue(2), getValue(3), getValue(4) } };
}
