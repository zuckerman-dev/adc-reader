/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2020-12-21
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of theex Software, and to permit persons to  whom the Software is
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
#include <fcntl.h>
#include <spdlog/spdlog.h>

#include "device_config.h"

/**
 * GPIO read and write
**/
void DEV_Digital_Write(uint16_t Pin, uint8_t Value)
{
// spdlog::trace("%s pin %d value %d \n", __PRETTY_FUNCTION__, Pin, Value);
#ifdef USE_BCM2835_LIB
	bcm2835_gpio_write(Pin, Value);
#elif USE_WIRINGPI_LIB
	digitalWrite(Pin, Value);
#elif USE_DEV_LIB
	SYSFS_GPIO_Write(Pin, Value);
#endif
}

uint8_t DEV_Digital_Read(uint16_t Pin)
{
	uint8_t Read_value = 0;

#ifdef USE_BCM2835_LIB
	Read_value = bcm2835_gpio_lev(Pin);
#elif USE_WIRINGPI_LIB
	Read_value = digitalRead(Pin);
#elif USE_DEV_LIB
	Read_value = SYSFS_GPIO_Read(Pin);
#endif
	return Read_value;
}

/**
 * SPI
**/
uint8_t DEV_SPI_WriteByte(uint8_t Value)
{
	uint8_t temp = 0;
	// spdlog::trace("write %x \r\n", Value);
#ifdef USE_BCM2835_LIB
	temp = bcm2835_spi_transfer(Value);
#elif USE_WIRINGPI_LIB
	wiringPiSPIDataRW(0, &Value, 1);
	temp = Value;
#elif USE_DEV_LIB
	temp = DEV_HARDWARE_SPI_TransferByte(Value);
#endif

	// spdlog::trace("[%s] [value: %d] [res: %d] \n", __PRETTY_FUNCTION__, Value, temp);
	// spdlog::trace("Read %x \r\n", temp);
	return temp;
}

uint8_t DEV_SPI_ReadByte(void)
{
	return DEV_SPI_WriteByte(0x00);
}

/**
 * GPIO Mode
**/
void DEV_GPIO_Mode(uint16_t Pin, uint16_t Mode)
{
#ifdef USE_BCM2835_LIB
	if (Mode == 0 || Mode == BCM2835_GPIO_FSEL_INPT)
	{
		bcm2835_gpio_fsel(Pin, BCM2835_GPIO_FSEL_INPT);
	}
	else
	{
		bcm2835_gpio_fsel(Pin, BCM2835_GPIO_FSEL_OUTP);
	}
#elif USE_WIRINGPI_LIB
	if (Mode == 0 || Mode == INPUT)
	{
		pinMode(Pin, INPUT);
		pullUpDnControl(Pin, PUD_UP);
	}
	else
	{
		pinMode(Pin, OUTPUT);
		// Debug (" %d OUT \r\n",Pin);
	}
#elif USE_DEV_LIB
	SYSFS_GPIO_Export(Pin);
	if (Mode == 0 || Mode == SYSFS_GPIO_IN)
	{
		SYSFS_GPIO_Direction(Pin, SYSFS_GPIO_IN);
		// Debug("IN Pin = %d\r\n",Pin);
	}
	else
	{
		SYSFS_GPIO_Direction(Pin, SYSFS_GPIO_OUT);
		// Debug("OUT Pin = %d\r\n",Pin);
	}
#endif
}

/**
 * delay x ms
**/
void DEV_Delay_ms(uint32_t xms)
{

// spdlog::trace("%s %d ms \n", __PRETTY_FUNCTION__, xms);

#ifdef USE_BCM2835_LIB
	bcm2835_delay(xms);
#elif USE_WIRINGPI_LIB
	delay(xms);
#elif USE_DEV_LIB
	uint32_t i;
	for (i = 0; i < xms; i++)
	{
		usleep(1000);
	}
#endif

}

static int DEV_Equipment_Testing(void)
{
	int i;
	int fd;
	char value_str[20];
	fd = open("/etc/issue", O_RDONLY);
	spdlog::trace("Current environment: ");
	while (1)
	{
		if (fd < 0)
		{
			Debug("Read failed Pin\n");
			return -1;
		}
		for (i = 0;; i++)
		{
			if (read(fd, &value_str[i], 1) < 0)
			{
				Debug("failed to read value!\n");
				return -1;
			}
			if (value_str[i] == 32)
			{
				spdlog::trace("\r\n");
				break;
			}
			spdlog::trace("%c", value_str[i]);
		}
		break;
	}

	if (i < 5)
	{
		spdlog::trace("Unrecognizable\r\n");
	}
	else
	{
		char RPI_System[10] = {"Raspbian"};
		for (i = 0; i < 6; i++)
		{
			if (RPI_System[i] != value_str[i])
			{
				spdlog::trace("Please make JETSON !!!!!!!!!!\r\n");
				return -1;
			}
		}
	}

	return 0;
}

void DEV_GPIO_Init(void)
{
	DEV_GPIO_Mode(DEV_RST_PIN, 1);
	DEV_GPIO_Mode(DEV_CS_PIN, 1);

	DEV_GPIO_Mode(DEV_DRDY_PIN, 0);

	DEV_Digital_Write(DEV_CS_PIN, 1);
}

/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
uint8_t DEV_Module_Init(void)
{
	spdlog::trace("/***********************************/ \r\n");
	if (DEV_Equipment_Testing() < 0)
	{
		return 1;
	}

#ifdef USE_BCM2835_LIB
	if (!bcm2835_init())
	{
		spdlog::trace("bcm2835 init failed  !!! \r\n");
		return 1;
	}
	else
	{
		spdlog::trace("bcm2835 init success !!! \r\n");
	}

	// GPIO Config
	DEV_GPIO_Init();

	bcm2835_spi_begin();									   //Start spi interface, set spi pin for the reuse function
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);   //High first transmission
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);				   //spi mode 1, '0, 1'
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64); //Frequency
#elif USE_WIRINGPI_LIB
	// if(wiringPiSetup() < 0) {//use wiringpi Pin number table
	if (wiringPiSetupGpio() < 0)
	{ //use BCM2835 Pin number table
		spdlog::trace("set wiringPi lib failed	!!! \r\n");
		return 1;
	}
	else
	{
		spdlog::trace("set wiringPi lib success !!! \r\n");
	}

	// GPIO Config
	DEV_GPIO_Init();
	// wiringPiSPISetup(0,10000000);
	wiringPiSPISetupMode(0, 1000000, 1);
#elif USE_DEV_LIB
	spdlog::trace("Write and read /dev/spidev0.0 \r\n");
	DEV_GPIO_Init();
	DEV_HARDWARE_SPI_begin("/dev/spidev0.0");
	DEV_HARDWARE_SPI_setSpeed(10000000);
	DEV_HARDWARE_SPI_Mode(SPI_MODE_1);
#endif
	spdlog::trace("/***********************************/ \r\n");
	return 0;
}

/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_Module_Exit(void)
{
#ifdef USE_BCM2835_LIB
	DEV_Digital_Write(DEV_RST_PIN, LOW);
	DEV_Digital_Write(DEV_CS_PIN, LOW);

	bcm2835_spi_end();
	bcm2835_close();
#elif USE_WIRINGPI_LIB
	DEV_Digital_Write(DEV_RST_PIN, 0);
	DEV_Digital_Write(DEV_CS_PIN, 0);
#elif USE_DEV_LIB
	DEV_HARDWARE_SPI_end();
	DEV_Digital_Write(DEV_RST_PIN, 0);
	DEV_Digital_Write(DEV_CS_PIN, 0);
#endif
}
