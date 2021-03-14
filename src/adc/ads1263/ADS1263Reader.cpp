#include "adc/ads1263/ADS1263Reader.h"

using namespace adc::ads1263;

AnalogDataReader::AnalogDataReader() 
{
    // DEV_ModuleInit();
    // DEV_Digital_Write(DEV_CS_PIN, 1);

    // if (ADS1263_init() == 1)
    // {
    //     DEV_ModuleExit();
    //     exit(0);
    // }

    // DEV_Digital_Write(DEV_CS_PIN, 0);
}

AnalogDataReader::~AnalogDataReader() 
{
    // DEV_Digital_Write(DEV_CS_PIN, 1);
}

adc::Signal AnalogDataReader::getValue(const uint8_t channel)
{
}

adc::SignalValues AnalogDataReader::readData()
{
    // return std::make_tuple(getValue(0), getValue(1), getValue(2));
}
