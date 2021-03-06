#include <cstdlib>
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

#include <concurrentqueue/concurrentqueue.h>

#include "adc/ads1256/ADS1256Reader.h"

using namespace moodycamel;

std::promise<void> exitSignal;
ConcurrentQueue<adc::SignalValues> q;

void Handler(int signo)
{
    exitSignal.set_value();
    DEV_ModuleExit();
    exit(0);
}

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

    adc::ads1256::AnalogDataReader reader;

    std::future<void> futureObj = exitSignal.get_future();

    std::thread producer([&]() {
        std::cout << "Starting reading thread" << std::endl;

        while (futureObj.wait_for(std::chrono::microseconds(10)) == std::future_status::timeout)
        {
            q.enqueue(reader.readData());
        }
    });

    std::thread consumer([&]() {
        std::cout << "Starting storing thread" << std::endl;

        std::ofstream output(output_filename);

        while (futureObj.wait_for(std::chrono::microseconds(10)) == std::future_status::timeout)
        {
            adc::SignalValues values;

			if (q.try_dequeue(values)) 
            {
                auto [signal1, signal2, signal3] = values;

                output.precision(std::numeric_limits< double >::digits10);

                output << std::fixed << signal1 << ","
                       << std::fixed << signal2 << ","
                       << std::fixed << signal3 << std::endl;
			}
		}
	});

    producer.join();
    consumer.join();

    return 0;
}
