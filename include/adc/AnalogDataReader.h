#pragma once

#include <tuple>
#include <chrono>

namespace adc 
{

using Signal = double;
using SignalValues = std::tuple<Signal, Signal, Signal>;

enum Channel : uint8_t {
    FIRST  = 0,
    SECOND = 1,
    THIRD  = 2
};

class AnalogDataReader {

public:
    AnalogDataReader() = default;
    
    virtual ~AnalogDataReader() = default;

    constexpr uint64_t getUnixTimeStamp(const std::time_t *t)
    {
        //if specific time is not passed then get current time
        std::time_t st = t == nullptr ? std::time(nullptr) : *t;
        auto secs = static_cast<std::chrono::seconds>(st).count();
        return static_cast<uint64_t>(secs);
    }

    virtual Signal getValue(const uint8_t channel = Channel::FIRST) = 0;

    virtual SignalValues readData() = 0;
};

} // namespace adc