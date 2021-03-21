/*****************************************************************************
* | File      	:   DEV_Config.h
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master
*                and enhance portability
*----------------
* |	This version:   V1.0
* | Date        :   2020-12-21
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#pragma once 

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "Debug.h"
#include "pinout.h"

#ifdef USE_BCM2835_LIB
    #include <bcm2835/1/bcm2835.h>
#elif USE_WIRINGPI_LIB
    #include <WiringPi/2/wiringPi.h>
    #include <WiringPi/2/wiringPiSPI.h>
#elif USE_DEV_LIB
    #include "RPI_sysfs_gpio.h"
    #include "dev_hardware_SPI.h"
#endif

/**
 * data
**/
using UBYTE  = uint8_t;
using UWORD =  uint16_t;
using UDOUBLE = uint32_t;

/*------------------------------------------------------------------------------------------------------*/
void DEV_Digital_Write(UWORD Pin, UBYTE Value);
UBYTE DEV_Digital_Read(UWORD Pin);

UBYTE DEV_SPI_WriteByte(UBYTE Value);
UBYTE DEV_SPI_ReadByte(void);

UBYTE DEV_Module_Init(void);
void DEV_Module_Exit(void);

void DEV_Delay_ms(UDOUBLE xms);
