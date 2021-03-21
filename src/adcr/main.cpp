#include <ctime>
#include <cstdio>
#include <string>
#include <chrono>
#include <signal.h>

#include <iostream>

#include <CLI/Error.hpp>
#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>

#include <spdlog/spdlog.h>

#include <date/date.h>

#include <concurrentqueue/concurrentqueue.h>

#include "adc/ads1256/ADS1256Reader.h"
#include "adc/ads1263/ADS1263Reader.h"

using namespace moodycamel;
using namespace date;
using namespace std::chrono;

std::promise<void> exitSignal;
ConcurrentQueue<adc::SignalData> signal_queue;

void Handler(int signo)
{
    exitSignal.set_value();
    DEV_Module_Exit();
    exit(0);
}

class AnalogDataReaderFactory {
public:
    static adc::AnalogDataReaderPtr make(const std::string & type) {

        adc::AnalogDataReaderPtr reader{nullptr};

        if(type == "ads1256") 
        {
            reader = std::make_shared<adc::ads1256::AnalogDataReader>();
        } 
        else if(type == "ads1263") 
        {
            reader = std::make_shared<adc::ads1263::AnalogDataReader>();
        } 

        return reader;
    }
};

int main(int argc, char **argv)
{
    CLI::App app{"AD1256 differential analog input test application"};

    uint32_t adc_gain, adc_sampling_rate;
    std::string output_filename{}; 
    long duration{};
    std::string reader_type{"ads1256"};

    app.add_option("-g,--gain", adc_gain, "Gain");
    app.add_option("-r,--sampling_rate", adc_gain, "Sampling rate");
    app.add_option("-o,--output",output_filename, "Path to the output file");
    app.add_option("-d,--duration",duration,"Recording duration");
    app.add_option("-t,--type", reader_type, "ADC Chip (ads1256 or ads1263)");

    CLI11_PARSE(app, argc, argv);

    signal(SIGINT, Handler);

    adc::AnalogDataReaderPtr reader = std::make_shared<adc::ads1256::AnalogDataReader>();

    std::future<void> futureObj = exitSignal.get_future();

    std::thread producer([&]() {
        spdlog::info("Starting reading thread");

        while (futureObj.wait_for(std::chrono::microseconds(10)) == std::future_status::timeout)
        {
            signal_queue.enqueue(reader->readData());
        }
    });

    std::thread consumer([&]() {
        spdlog::info("Starting storing thread");

        std::ofstream output(output_filename);

        while (futureObj.wait_for(std::chrono::microseconds(10)) == std::future_status::timeout)
        {
            adc::SignalData data;

			if (signal_queue.try_dequeue(data)) 
            {
                output.precision(std::numeric_limits<adc::Signal>::digits10);

                output << data.time_point << ",";
                
                std::for_each(data.values.begin(), data.values.end(), [&](const adc::Signal & signal) {
                    output << std::fixed << signal << ",";
                });

                output << std::endl;
			}
		}
	});

    producer.join();
    consumer.join();

    return 0;
}
