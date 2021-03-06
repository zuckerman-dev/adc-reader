#pragma once

#include <thread>
#include <chrono>
#include <vector>
#include <future>

#include "adc/ads1256/ADS1256.h"
#include "adc/AnalogDataReader.h"

namespace adc::ads1256 {

class AnalogDataReader : public adc::AnalogDataReader {

public:
    AnalogDataReader();
    virtual ~AnalogDataReader() override;

    constexpr uint64_t getUnixTimeStamp(const std::time_t *t = nullptr);

    virtual adc::Signal getValue(const uint8_t channel) override;

    virtual adc::SignalValues readData() override;

private:
    double _pga = 64;
    double _conversionFactor = 1;
};

} // namespace adc::ads1256