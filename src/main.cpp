#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <string>
#include <chrono>
#include <signal.h>

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <future>

#include "ADS1256.h"

#include <CLI/Error.hpp>
#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>

#include <concurrentqueue/concurrentqueue.h>

using namespace moodycamel;

using SignalValues = std::tuple<double, double, double>;
std::promise<void> exitSignal;
ConcurrentQueue<SignalValues> q;

void Handler(int signo)
{
    exitSignal.set_value();
    DEV_ModuleExit();
    exit(0);
}

class AnalogDataReader {

public:
    AnalogDataReader() 
    {
        std::cout << "ADC Initialization" << std::endl;

        DEV_ModuleInit();
        DEV_Digital_Write(DEV_CS_PIN, 1);

        if (ADS1256_init() == 1)
        {
            DEV_ModuleExit();
            exit(0);
        }

        DEV_Digital_Write(DEV_CS_PIN, 0);
    }

    ~AnalogDataReader() 
    {
        std::cout << "ADC cleanup" << std::endl;
        DEV_Digital_Write(DEV_CS_PIN, 1);
    }

    constexpr uint64_t getUnixTimeStamp(const std::time_t *t = nullptr)
    {
        //if specific time is not passed then get current time
        std::time_t st = t == nullptr ? std::time(nullptr) : *t;
        auto secs = static_cast<std::chrono::seconds>(st).count();
        return static_cast<uint64_t>(secs);
    }

    template<uint8_t channel>
    double getValue()
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

    void readData()
    {
        double values[3] = {};

        while (true)
        {
            values[0] = getValue<0>();
            values[1] = getValue<1>();
            values[2] = getValue<2>();

            printf("%lld, %f, %f, %f \n", getUnixTimeStamp(), values[0], values[1], values[2]);
        }
    }

private:
    double _pga = 64;
    double _conversionFactor = 1;
};


int main(int argc, char **argv)
{
    CLI::App app{"AD1256 differential analog input test application"};

    uint32_t adc_gain, adc_sampling_rate;
    std::string output_filename{}; 
    long duration{};

    app.add_option("-g,--gain", adc_gain, "ADC Gain");
    app.add_option("-r,--sampling_rate", adc_gain, "ADC Sampling rate");
    app.add_option("-o,--output",output_filename, "Path to the output file");
    app.add_option("-d,--duration",duration,"Recording duration");

    CLI11_PARSE(app, argc, argv);

    signal(SIGINT, Handler);

    AnalogDataReader reader;

    std::future<void> futureObj = exitSignal.get_future();

    std::thread producer([&]() {
        std::cout << "Starting reading thread" << std::endl;

        while (futureObj.wait_for(std::chrono::microseconds(10)) == std::future_status::timeout)
        {
            q.enqueue(std::make_tuple(reader.getValue<0>(), reader.getValue<1>(), reader.getValue<2>()));
        }
    });

    std::thread consumer([&]() {
        std::cout << "Starting storing thread" << std::endl;

        std::ofstream output(output_filename);

        while (futureObj.wait_for(std::chrono::microseconds(10)) == std::future_status::timeout)
        {
            SignalValues values;
			if (q.try_dequeue(values)) 
            {
                auto [signal1, signal2, signal3] = values;

                // printf("%f, %f, %f \n", signal1, signal2, signal3);

                output.precision(std::numeric_limits< double >::digits10);

                output << std::fixed << signal1 << ","
                          << std::fixed << signal2 << ","
                          << std::fixed << signal3
                          << std::endl;
			}
		}
	});

    producer.join();
    consumer.join();

    return 0;
}
