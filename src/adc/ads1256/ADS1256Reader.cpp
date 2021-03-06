#include "adc/ads1256/ADS1256Reader.h"

using namespace adc::ads1256;

AnalogDataReader::AnalogDataReader() 
{
    DEV_ModuleInit();
    DEV_Digital_Write(DEV_CS_PIN, 1);

    if (ADS1256_init() == 1)
    {
        DEV_ModuleExit();
        exit(0);
    }

    DEV_Digital_Write(DEV_CS_PIN, 0);
}

AnalogDataReader::~AnalogDataReader() 
{
    DEV_Digital_Write(DEV_CS_PIN, 1);
}

constexpr uint64_t AnalogDataReader::getUnixTimeStamp(const std::time_t *t)
{
    //if specific time is not passed then get current time
    std::time_t st = t == nullptr ? std::time(nullptr) : *t;
    auto secs = static_cast<std::chrono::seconds>(st).count();
    return static_cast<uint64_t>(secs);
}

adc::Signal AnalogDataReader::getValue(const uint8_t channel)
{
    int32_t read = 0;

    while(DEV_Digital_Read(DEV_DRDY_PIN) == 1) {}
    
    DEV_SPI_WriteByte(CMD_WREG | REG_MUX);

    bcm2835_delayMicroseconds(30);

    DEV_SPI_WriteByte(0x00);
    DEV_SPI_WriteByte(((channel*2) << 4) | ((channel*2)+1)); // Switch to Diff ch 0
    DEV_SPI_WriteByte(CMD_SYNC);

    bcm2835_delayMicroseconds(100);

    DEV_SPI_WriteByte(CMD_WAKEUP);
    DEV_SPI_WriteByte(CMD_RDATA);

    bcm2835_delayMicroseconds(30);
    
    read = ADS1256_Read_ADC_Data_Lite();
    
    if (read  & 0x00800000) {
        read  |= 0xff000000;
    }

    return (((double)read / 0x7FFFFF) * ((2 * 2.5) / (float)_pga)) *  _conversionFactor;
}

adc::SignalValues AnalogDataReader::readData()
{
    return std::make_tuple(getValue(0), getValue(1), getValue(2));
}
