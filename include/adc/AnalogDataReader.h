#pragma once

#include <vector>
#include <chrono>

namespace adc 
{

using Signal = double;
using SignalValues = std::vector<Signal>;
using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds>;
using Gain = uint16_t;

struct SignalData {
    TimePoint time_point;
    SignalValues values;
};

class AnalogDataReader {

public:
    AnalogDataReader(uint8_t channels) : _channels(channels) {};    
    virtual ~AnalogDataReader() = default;

    virtual Signal getValue(const uint8_t & channel = 0) = 0;

    virtual SignalData readData() = 0;

    virtual uint8_t channels() const { return _channels; }

    virtual bool initialized() { return _initialized; }
    virtual uint8_t chipID() { return _chipID; }

protected:

    virtual TimePoint getTimePoint() 
    {
        return std::chrono::high_resolution_clock::now();
    }

    void setInitialized(const bool & v) { _initialized = v; }
    void setChipID(const uint8_t & id) { _chipID = id; }

private:
    uint8_t _channels;
    bool _initialized;
    uint8_t _chipID;
};

using AnalogDataReaderPtr = std::shared_ptr<AnalogDataReader>;

} // namespace adc