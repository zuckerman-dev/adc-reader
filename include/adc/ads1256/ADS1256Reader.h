#pragma once

#include <thread>
#include <vector>
#include <future>

#include "adc/ads1256/ADS1256.h"

#include "adc/AnalogDataReader.h"

namespace adc::ads1256 {

class AnalogDataReader : public adc::AnalogDataReader {

public:
    AnalogDataReader();
    virtual ~AnalogDataReader() override;

    virtual adc::Signal getValue(const uint8_t & channel) override;

    virtual adc::SignalData readData() override;

private:
    double _pga = 64;
    double _conversionFactor = 1;

    ADS1256 _ads1256_hal;
};

using AnalogDataReaderPtr = std::shared_ptr<AnalogDataReader>;

} // namespace adc::ads1256