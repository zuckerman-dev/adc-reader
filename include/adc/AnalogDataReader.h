#pragma once

#include <tuple>

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

    virtual Signal getValue(const uint8_t channel = Channel::FIRST) = 0;

    virtual SignalValues readData() = 0;
};

} // namespace adc