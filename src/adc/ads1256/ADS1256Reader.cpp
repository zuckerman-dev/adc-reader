#include "adc/ads1256/ADS1256Reader.h"

#include "adc/device_config.h"

using namespace adc::ads1256;

AnalogDataReader::AnalogDataReader() : adc::AnalogDataReader(ADS1256::Channels)
{
    DEV_Module_Init();
    DEV_Digital_Write(DEV_CS_PIN, 1);

    if (_ads1256_hal.init() == 1)
    {
        DEV_Module_Exit();
        setInitialized(false);
    }

    DEV_Digital_Write(DEV_CS_PIN, 0);
    
    setInitialized(true);
}

AnalogDataReader::~AnalogDataReader() 
{
    DEV_Digital_Write(DEV_CS_PIN, 1);
}

adc::Signal AnalogDataReader::getValue(const uint8_t & channel)
{
    if(channel >= channels()) {
        return 0;
    }

    int32_t read = 0;

    while(DEV_Digital_Read(DEV_DRDY_PIN) == 1) {}
    
    DEV_SPI_WriteByte(ADS1256::Cmd::CMD_WREG | ADS1256::Reg::REG_MUX);

    DEV_Delay_ms(2);

    DEV_SPI_WriteByte(0x00);
    DEV_SPI_WriteByte(((channel*2) << 4) | ((channel*2)+1)); // Switch to Diff ch 0
    DEV_SPI_WriteByte(ADS1256::Cmd::CMD_SYNC);

    DEV_Delay_ms(2);

    DEV_SPI_WriteByte(ADS1256::Cmd::CMD_WAKEUP);
    DEV_SPI_WriteByte(ADS1256::Cmd::CMD_RDATA);

    DEV_Delay_ms(2);
    
    read = _ads1256_hal.Read_ADC_Data_Lite();
    
    if (read  & 0x00800000) {
        read  |= 0xff000000;
    }

    return (((double)read / 0x7FFFFF) * ((2 * 2.5) / (float)_pga)) *  _conversionFactor;
}

adc::SignalData AnalogDataReader::readData()
{
    return adc::SignalData{getTimePoint(), { getValue(0), getValue(1), getValue(2), getValue(3) } };
}
