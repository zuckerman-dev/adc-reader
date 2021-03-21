/*****************************************************************************
* | File      	:   ADS1256.h
* | Author      :   Waveshare team
* | Function    :   ADS1256 driver
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2019-03-25
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

#include <array>
#include "DEV_Config.h"

namespace adc::ads1256 {

class ADS1256 {

public:
/* gain channel*/
	enum Gain
	{
		ADS1256_GAIN_1			= 0,	/* GAIN   1 */
		ADS1256_GAIN_2			= 1,	/* GAIN   2 */
		ADS1256_GAIN_4			= 2,	/* GAIN   4 */
		ADS1256_GAIN_8			= 3,	/* GAIN   8 */
		ADS1256_GAIN_16			= 4,	/* GAIN  16 */
		ADS1256_GAIN_32			= 5,	/* GAIN  32 */
		ADS1256_GAIN_64			= 6,	/* GAIN  64 */

	};

	enum Drate
	{
		ADS1256_30000SPS = 0,
		ADS1256_15000SPS,
		ADS1256_7500SPS,
		ADS1256_3750SPS,
		ADS1256_2000SPS,
		ADS1256_1000SPS,
		ADS1256_500SPS,
		ADS1256_100SPS,
		ADS1256_60SPS,
		ADS1256_50SPS,
		ADS1256_30SPS,
		ADS1256_25SPS,
		ADS1256_15SPS,
		ADS1256_10SPS,
		ADS1256_5SPS,
		ADS1256_2d5SPS,
		
		ADS1256_DRATE_MAX
		
	};


	enum Reg
	{
		/*Register address, followed by reset the default values */
		REG_STATUS = 0,	// x1H
		REG_MUX    = 1, // 01H
		REG_ADCON  = 2, // 20H
		REG_DRATE  = 3, // F0H
		REG_IO     = 4, // E0H
		REG_OFC0   = 5, // xxH
		REG_OFC1   = 6, // xxH
		REG_OFC2   = 7, // xxH
		REG_FSC0   = 8, // xxH
		REG_FSC1   = 9, // xxH
		REG_FSC2   = 10, // xxH
	};


	enum Cmd
	{
		CMD_WAKEUP  = 0x00,	// Completes SYNC and Exits Standby Mode 0000  0000 (00h)
		CMD_RDATA   = 0x01, // Read Data 0000  0001 (01h)
		CMD_RDATAC  = 0x03, // Read Data Continuously 0000   0011 (03h)
		CMD_SDATAC  = 0x0F, // Stop Read Data Continuously 0000   1111 (0Fh)
		CMD_RREG    = 0x10, // Read from REG rrr 0001 rrrr (1xh)
		CMD_WREG    = 0x50, // Write to REG rrr 0101 rrrr (5xh)
		CMD_SELFCAL = 0xF0, // Offset and Gain Self-Calibration 1111    0000 (F0h)
		CMD_SELFOCAL= 0xF1, // Offset Self-Calibration 1111    0001 (F1h)
		CMD_SELFGCAL= 0xF2, // Gain Self-Calibration 1111    0010 (F2h)
		CMD_SYSOCAL = 0xF3, // System Offset Calibration 1111   0011 (F3h)
		CMD_SYSGCAL = 0xF4, // System Gain Calibration 1111    0100 (F4h)
		CMD_SYNC    = 0xFC, // Synchronize the A/D Conversion 1111   1100 (FCh)
		CMD_STANDBY = 0xFD, // Begin Standby Mode 1111   1101 (FDh)
		CMD_RESET   = 0xFE, // Reset to Power-Up Values 1111   1110 (FEh)
	};


	UBYTE init(void);
	void SetMode(UBYTE Mode);
	UDOUBLE Read_ADC_Data_Lite(void);
	void ConfigADC(Gain gain, Drate drate);
	UDOUBLE GetChannalValue(UBYTE Channel);
	void GetAll(UDOUBLE *ADC_Value);

protected: 

	void reset();

	void WriteCmd(UBYTE Cmd);
	void WriteReg(UBYTE Reg, UBYTE data);

	void WaitDRDY();

	UBYTE ReadChipID();
	UBYTE Read_data(UBYTE Reg);
	UDOUBLE Read_ADC_Data();

	void SetChannal(UBYTE Channal);
	void SetDiffChannal(UBYTE Channal);

private: 
	const std::array<uint8_t, Drate::ADS1256_DRATE_MAX> drate_e =
	{
		0xF0,		/*reset the default values  */
		0xE0,
		0xD0,
		0xC0,
		0xB0,
		0xA1,
		0x92,
		0x82,
		0x72,
		0x63,
		0x53,
		0x43,
		0x33,
		0x20,
		0x13,
		0x03
	};

	UBYTE ScanMode = 0;

};


} // namespace adc::ads1256