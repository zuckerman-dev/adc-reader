# ADC Read

Sample application to read data from `ADS1256`

# Build

    mkdir build && cd build
    cmake <source-dir>
    make 

# Documentation

    mkdir build && cd build
    cmake -DENABLE_API_DOCUMENTATION=ON <source-dir>
    make doxygen

use simple python http server to view it in browser

    python -m http.server 8000 --directory <build>/html
# Usage

    ./adcr --help
    AD1256 differential analog input test application
    Usage: ./adcr [OPTIONS]

    Options:
    -h,--help                   Print this help message and exit
    -g,--gain UINT              ADC Gain
    -r,--sampling_rate UINT     ADC Sampling rate

## Sample output

    ./adcr
    1614453497, 0.636000, 0.636295, 0.636784 
    1614453497, 0.637372, 0.637935, 0.638310 
    1614453497, 0.638204, 0.637800, 0.637183 
    1614453497, 0.636552, 0.635921, 0.635392 
    1614453497, 0.625086, 0.634682, 0.625250 
    1614453497, 0.625824, 0.636266, 0.636976 
    1614453497, 0.637468, 0.637784, 0.638212 

# Links

* https://www.ti.com/product/ADS1256
* https://www.ti.com/lit/gpn/ads1256
* https://www.waveshare.com/wiki/High-Precision_AD/DA_Board