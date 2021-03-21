#pragma once

#include <thread>
#include <vector>
#include <future>

#include "adc/ads1263/ADS1263.h"
#include "adc/AnalogDataReader.h"

namespace adc::ads1263 {

class AnalogDataReader : public adc::AnalogDataReader {

public:
    AnalogDataReader();
    virtual ~AnalogDataReader() override;

    virtual adc::Signal getValue(const uint8_t & channel) override;

    virtual adc::SignalData readData() override;

private:
    double _pga = 64;
    double _conversionFactor = 1;
};

using AnalogDataReaderPtr = std::shared_ptr<AnalogDataReader>;

} // namespace adc::ads1263