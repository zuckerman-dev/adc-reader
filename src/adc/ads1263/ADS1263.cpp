/*****************************************************************************
* | File      	:   ADS1263.c
* | Author      :   Waveshare team
* | Function    :   ADS1263 driver
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2020-10-28
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
#include "adc/ads1263/ADS1263.h"

#include <cassert>

using namespace adc::ads1263;

/******************************************************************************
function:   Module reset
parameter:
Info:
******************************************************************************/
void ADS1263::reset()
{
	// printf("%s \n", __PRETTY_FUNCTION__);

    DEV_Digital_Write(DEV_RST_PIN, 1);
    DEV_Delay_ms(300);
    DEV_Digital_Write(DEV_RST_PIN, 0);
    DEV_Delay_ms(300);
    DEV_Digital_Write(DEV_RST_PIN, 1);
    DEV_Delay_ms(300);
}

/******************************************************************************
function:   send command
parameter: 
        Cmd: command
Info:
******************************************************************************/
void ADS1263::WriteCmd(UBYTE Cmd)
{
    DEV_Digital_Write(DEV_CS_PIN, 0);
    DEV_SPI_WriteByte(Cmd);
    DEV_Digital_Write(DEV_CS_PIN, 1);
}

/******************************************************************************
function:   Write a data to the destination register
parameter: 
        Reg : Target register
        data: Written data
Info:
******************************************************************************/
void ADS1263::WriteReg(UBYTE Reg, UBYTE data)
{
    DEV_Digital_Write(DEV_CS_PIN, 0);
    DEV_SPI_WriteByte(CMD_WREG | Reg);
    DEV_SPI_WriteByte(0x00);
    DEV_SPI_WriteByte(data);
    DEV_Digital_Write(DEV_CS_PIN, 1);
}

/******************************************************************************
function:   Read a data from the destination register
parameter: 
        Reg : Target register
Info:
    Return the read data
******************************************************************************/
UBYTE ADS1263::Read_data(UBYTE Reg)
{
    UBYTE temp = 0;
    DEV_Digital_Write(DEV_CS_PIN, 0);
    DEV_SPI_WriteByte(CMD_RREG | Reg);
    DEV_SPI_WriteByte(0x00);
    DEV_Delay_ms(1);
    temp = DEV_SPI_ReadByte();
    DEV_Digital_Write(DEV_CS_PIN, 1);

	// printf("Read data %d \n", temp);

    return temp;
}

/******************************************************************************
function:   Check data
parameter: 
        val : 4 bytes(ADC2 is 3 bytes) data
		byt : CRC byte
Info:
		Check success, return 0
******************************************************************************/
UBYTE ADS1263::Checksum(UDOUBLE val, UBYTE byt)
{
	UBYTE sum = 0;
	UBYTE mask = -1;		// 8 bits mask, 0xff
	while(val) {
		sum += val & mask;	// only add the lower values
		val >>= 8;			// shift down
	}
	sum += 0x9b;
	// printf("--- %x %x--- \r\n", sum, byt);
	return sum ^ byt;		// if equal, this will be 0
}

/******************************************************************************
function:   Waiting for a busy end
parameter: 
Info:
    Timeout indicates that the operation is not working properly.
******************************************************************************/
void ADS1263::WaitDRDY()
{
	// printf("ADS1263::WaitDRDY \r\n");
    UDOUBLE i = 0;
    for(i=0;i<4000000;i++) {
		DEV_Delay_ms(1);
        if(DEV_Digital_Read(DEV_DRDY_PIN) == 0)
            break;
    }
    if(i >= 4000000) {
       printf("Time Out ...\r\n"); 
    }	
	// printf("ADS1263::WaitDRDY Release \r\n");
}

/******************************************************************************
function:  Read device ID
parameter: 
Info:
******************************************************************************/
UBYTE ADS1263::ReadChipID()
{
	// printf("%s \n", __PRETTY_FUNCTION__);

    UBYTE id;
    id = Read_data(ADS1263::Reg::REG_ID);

	// printf("ChipID: %d \n", id);

    return id>>5;
}

/******************************************************************************
function:  Setting mode
parameter: 
    Mode : 0 Single-ended input
           1 channel1 Differential input
Info:
******************************************************************************/
void ADS1263::SetMode(UBYTE Mode)
{
    if(Mode == 0) {
        ScanMode = 0;
    }else {
        ScanMode = 1;
    }
}

/******************************************************************************
function:  Configure ADC gain and sampling speed
parameter: 
    gain : Enumeration type gain
    drate: Enumeration type sampling speed
Info:
******************************************************************************/
void ADS1263::ConfigADC1(ADS1263::Gain gain, ADS1263::Drate drate)
{
	UBYTE MODE2 = 0x80;				//0x80:PGA bypassed, 0x00:PGA enabled
	MODE2 |= (gain << 4) | drate;
	WriteReg(ADS1263::Reg::REG_MODE2, MODE2);
    DEV_Delay_ms(1);
	if(Read_data(ADS1263::Reg::REG_MODE2) == MODE2)
	{
		printf("ADS1263::Reg::REG_MODE2 success \r\n");
	}
	else
	{
		printf("ADS1263::Reg::REG_MODE2 unsuccess \r\n");
	}
	
	UBYTE REFMUX = 0x24;		//0x00:+-2.5V as REF, 0x24:VDD,VSS as REF
	WriteReg(ADS1263::Reg::REG_REFMUX, REFMUX);
	DEV_Delay_ms(1);
	if(Read_data(ADS1263::Reg::REG_REFMUX) == REFMUX)
	{
		printf("ADS1263::Reg::REG_REFMUX success \r\n");
	}
	else
	{
		printf("ADS1263::Reg::REG_REFMUX unsuccess \r\n");
	}
	
	UBYTE MODE0 = ADS1263::Delay::ADS1263_DELAY_8d8ms;
	WriteReg(ADS1263::Reg::REG_MODE0, MODE0);	
	DEV_Delay_ms(1);
	if(Read_data(ADS1263::Reg::REG_MODE0) == MODE0)
	{
		printf("ADS1263::Reg::REG_MODE0 success \r\n");
	}
	else
	{
		printf("ADS1263::Reg::REG_MODE0 unsuccess \r\n");
	}
}

/******************************************************************************
function:  Configure ADC gain and sampling speed
parameter: 
    gain : Enumeration type gain
    drate: Enumeration type sampling speed
Info:
******************************************************************************/
void ADS1263::ConfigADC2(ADS1263::Adc2Gain gain, ADS1263::Adc2Drate drate)
{
	UBYTE ADC2CFG = 0x20;				//REF, 0x20:VAVDD and VAVSS, 0x00:+-2.5V

	ADC2CFG |= (drate << 6) | gain;
	WriteReg(ADS1263::Reg::REG_ADC2CFG, ADC2CFG);
    DEV_Delay_ms(1);

	if(Read_data(ADS1263::Reg::REG_ADC2CFG) == ADC2CFG)
	{
		printf("ADS1263::Reg::REG_ADC2CFG success \r\n");
	}
	else
	{
		printf("ADS1263::Reg::REG_ADC2CFG unsuccess \r\n");
	}
	
	UBYTE MODE0 = ADS1263::Delay::ADS1263_DELAY_8d8ms;
	WriteReg(ADS1263::Reg::REG_MODE0, MODE0);	
	DEV_Delay_ms(1);

	if(Read_data(ADS1263::Reg::REG_MODE0) == MODE0)
	{
		printf("ADS1263::Reg::REG_MODE0 success \r\n");
	}
	else
	{
		printf("ADS1263::Reg::REG_MODE0 unsuccess \r\n");
	}
}

/******************************************************************************
function:  Device initialization
parameter: 
Info:
******************************************************************************/
UBYTE ADS1263::init()
{
	printf("%s \n", __PRETTY_FUNCTION__);

    reset();
	UBYTE chip_id = ReadChipID();
    if(chip_id == 1) 
	{
        printf("ID Read success: %d \r\n", chip_id);
    } 
	else 
	{
        printf("ID Read failed: %d \r\n", chip_id);
        return 1;
    }
	WriteCmd(CMD_STOP1);
	WriteCmd(CMD_STOP2);

	ConfigADC1(ADS1263::Gain::ADS1263_GAIN_1, ADS1263::Drate::ADS1263_19200SPS);
	ConfigADC2(ADS1263::Adc2Gain::ADS1263_ADC2_GAIN_1, ADS1263::Adc2Drate::ADS1263_ADC2_10SPS);

    return 0;
}

/******************************************************************************
function:  Set the channel to be read
parameter: 
    Channal : Set channel number
Info:
******************************************************************************/
void ADS1263::SetChannal(UBYTE Channal)
{
    if(Channal > 10) 
	{
        return ;
    }
	
	UBYTE INPMUX = (Channal << 4) | 0x0a;		//0x0a:VCOM as Negative Input
    WriteReg(ADS1263::Reg::REG_INPMUX, INPMUX);
	
	if(Read_data(ADS1263::Reg::REG_INPMUX) == INPMUX) 
	{
		// printf("ADC1_SetChannal success \r\n");
	} 
	else 
	{
		printf("ADC1_SetChannal unsuccess \r\n");
	}
}

/******************************************************************************
function:  Set the channel to be read
parameter: 
    Channal : Set channel number
Info:
******************************************************************************/
void ADS1263::SetChannal_ADC2(UBYTE Channal)
{
    if(Channal > 10) {
        return ;
    }
	UBYTE INPMUX = (Channal << 4) | 0x0a;		//0x0a:VCOM as Negative Input
    WriteReg(ADS1263::Reg::REG_ADC2MUX, INPMUX);
	if(Read_data(ADS1263::Reg::REG_ADC2MUX) == INPMUX) {
		// printf("ADC2_SetChannal success \r\n");
	} else {
		printf("ADC2_SetChannal unsuccess \r\n");
	}
}

/******************************************************************************
function:  Set the channel to be read
parameter: 
    Channal : Set channel number
Info:
******************************************************************************/
void ADS1263::SetDiffChannal(UBYTE Channal)
{
	UBYTE INPMUX;
    if (Channal == 0) {
		INPMUX = (0<<4) | 1;	//DiffChannal	AIN0-AIN1
    } else if(Channal == 1) {
		INPMUX = (2<<4) | 3;	//DiffChannal	AIN2-AIN3
    } else if(Channal == 2) {
		INPMUX = (4<<4) | 5;	//DiffChannal	AIN4-AIN5
    } else if(Channal == 3) {
		INPMUX = (6<<4) | 7;	//DiffChannal	AIN6-AIN7
    } else if(Channal == 4) {
		INPMUX = (8<<4) | 9;	//DiffChannal	AIN8-AIN9
    }
	WriteReg(ADS1263::Reg::REG_INPMUX, INPMUX); 	
	if(Read_data(ADS1263::Reg::REG_INPMUX) == INPMUX) {
		// printf("SetDiffChannal success \r\n");
	} else {
		printf("SetDiffChannal unsuccess \r\n");
	}
}

/******************************************************************************
function:  Set the channel to be read
parameter: 
    Channal : Set channel number
Info:
******************************************************************************/
void ADS1263::SetDiffChannal_ADC2(UBYTE Channal)
{
	UBYTE INPMUX;
    if (Channal == 0) {
		INPMUX = (0<<4) | 1;	//DiffChannal	AIN0-AIN1
    } else if(Channal == 1) {
		INPMUX = (2<<4) | 3;	//DiffChannal	AIN2-AIN3
    } else if(Channal == 2) {
		INPMUX = (4<<4) | 5;	//DiffChannal	AIN4-AIN5
    } else if(Channal == 3) {
		INPMUX = (6<<4) | 7;	//DiffChannal	AIN6-AIN7
    } else if(Channal == 4) {
		INPMUX = (8<<4) | 9;	//DiffChannal	AIN8-AIN9
    }
	WriteReg(ADS1263::Reg::REG_ADC2MUX, INPMUX); 	
	if(Read_data(ADS1263::Reg::REG_ADC2MUX) == INPMUX) {
		// printf("SetDiffChannal_ADC2 success \r\n");
	} else {
		printf("SetDiffChannal_ADC2 unsuccess \r\n");
	}
}

/******************************************************************************
function:  Read ADC data
parameter: 
Info:
******************************************************************************/
UDOUBLE ADS1263::Read_ADC1_Data()
{
    UDOUBLE read = 0;
    UBYTE buf[4] = {0, 0, 0, 0};
	UBYTE Status, CRC;
    DEV_Digital_Write(DEV_CS_PIN, 0);
	do {
		DEV_SPI_WriteByte(CMD_RDATA1);
		DEV_Delay_ms(10);
		Status = DEV_SPI_ReadByte();
	}while((Status & 0x40) == 0);
	
    buf[0] = DEV_SPI_ReadByte();
    buf[1] = DEV_SPI_ReadByte();
    buf[2] = DEV_SPI_ReadByte();
    buf[3] = DEV_SPI_ReadByte();
	CRC = DEV_SPI_ReadByte();
    DEV_Digital_Write(DEV_CS_PIN, 1);
    read |= ((UDOUBLE)buf[0] << 24);
    read |= ((UDOUBLE)buf[1] << 16);
    read |= ((UDOUBLE)buf[2] << 8);
	read |= (UDOUBLE)buf[3];
    // printf("%x %x %x %x %x %x\r\n", Status, buf[0], buf[1], buf[2], buf[3], CRC);
	if(Checksum(read, CRC) != 0)
		printf("ADC1 Data read error! \r\n");
    return read;
}

/******************************************************************************
function:  Read ADC data
parameter: 
Info:
******************************************************************************/
UDOUBLE ADS1263::Read_ADC2_Data()
{
    UDOUBLE read = 0;
    UBYTE buf[4] = {0, 0, 0, 0};
	UBYTE Status, CRC;
	
    DEV_Digital_Write(DEV_CS_PIN, 0);
	do {
		DEV_SPI_WriteByte(CMD_RDATA2);
		DEV_Delay_ms(10);
		Status = DEV_SPI_ReadByte();
	}while((Status & 0x80) == 0);
	
    buf[0] = DEV_SPI_ReadByte();
    buf[1] = DEV_SPI_ReadByte();
    buf[2] = DEV_SPI_ReadByte();
    buf[3] = DEV_SPI_ReadByte();
	CRC = DEV_SPI_ReadByte();
    DEV_Digital_Write(DEV_CS_PIN, 1);
    read |= ((UDOUBLE)buf[0] << 16);
    read |= ((UDOUBLE)buf[1] << 8);
	read |= (UDOUBLE)buf[2];
    // printf("%x %x %x %x %x\r\n", Status, buf[0], buf[1], buf[2], CRC);
	if(Checksum(read, CRC) != 0)
		printf("ADC2 Data read error! \r\n");
    return read;
}

/******************************************************************************
function:  Read ADC specified channel data
parameter: 
    Channel: Channel number
Info:
******************************************************************************/
UDOUBLE ADS1263::GetChannalValue(UBYTE Channel)
{
    UDOUBLE Value = 0;
    if(ScanMode == 0) {// 0  Single-ended input  10 channel1 Differential input  5 channe 
        if(Channel>10) {
            return 0;
        }
        SetChannal(Channel);
		DEV_Delay_ms(2);
        WriteCmd(ADS1263::Cmd::CMD_START1);
        DEV_Delay_ms(2);
		WaitDRDY();
        Value = Read_ADC1_Data();
    } else {
        if(Channel>4) {
            return 0;
        }
        SetDiffChannal(Channel);
		DEV_Delay_ms(2);
        WriteCmd(ADS1263::Cmd::CMD_START1);
        DEV_Delay_ms(2);
		WaitDRDY();
        Value = Read_ADC1_Data();
    }
	// printf("Get IN%d value success \r\n", Channel);
    return Value;
}

UDOUBLE ADS1263::GetDiffChannalValue(UBYTE Channel)
{
    UDOUBLE Value = 0;

	assert(Channel <= 4);

	SetDiffChannal(Channel);
	DEV_Delay_ms(2);
	WriteCmd(ADS1263::Cmd::CMD_START1);
	DEV_Delay_ms(2);
	WaitDRDY();
	Value = Read_ADC1_Data();

    return Value;
}

/******************************************************************************
function:  Read ADC specified channel data
parameter: 
    Channel: Channel number
Info:
******************************************************************************/
UDOUBLE ADS1263::GetChannalValue_ADC2(UBYTE Channel)
{
    UDOUBLE Value = 0;
    if(ScanMode == 0) {// 0  Single-ended input  10 channel1 Differential input  5 channe 
        if(Channel>10) {
            return 0;
        }
        SetChannal_ADC2(Channel);
		DEV_Delay_ms(2);
        WriteCmd(CMD_START2);
        DEV_Delay_ms(2);
        Value = Read_ADC2_Data();
    } else {
        if(Channel>4) {
            return 0;
        }
        SetDiffChannal_ADC2(Channel);
		DEV_Delay_ms(2);
        WriteCmd(CMD_START2);
        DEV_Delay_ms(2);
        Value = Read_ADC2_Data();
    }
	// printf("Get IN%d value success \r\n", Channel);
    return Value;
}

/******************************************************************************
function:  Read data from all channels
parameter: 
    ADC_Value : ADC Value
Info:
******************************************************************************/
void ADS1263::GetAll(UDOUBLE *ADC_Value)
{
    UBYTE i;
    for(i = 0; i<10; i++) {
        ADC_Value[i] = GetChannalValue(i);
		WriteCmd(CMD_STOP1);
		DEV_Delay_ms(20);
    }
	// printf("----------Read ADC1 value success----------\r\n");
}

/******************************************************************************
function:  Read data from all channels
parameter: 
    ADC_Value : ADC Value
Info:
******************************************************************************/
void ADS1263::GetAll_ADC2(UDOUBLE *ADC_Value)
{
    UBYTE i;
    for(i = 0; i<10; i++) {
        ADC_Value[i] = GetChannalValue_ADC2(i);
		WriteCmd(CMD_STOP2);
		DEV_Delay_ms(20);
    }
	// printf("----------Read ADC2 value success----------\r\n");
}

/******************************************************************************
function:  RTD Test function
parameter: 
    delay :	Conversion delay
	gain :	Conversion gain
	drate :	speed
Info:
******************************************************************************/
UDOUBLE ADS1263::Rtd(ADS1263::Delay delay, ADS1263::Gain gain, ADS1263::Drate drate)
{
	UDOUBLE Value;

	//MODE0 (CHOP OFF)
	UBYTE MODE0 = delay;
	WriteReg(ADS1263::Reg::REG_MODE0, MODE0);
	DEV_Delay_ms(1);
	
	//(IDACMUX) IDAC2 AINCOM,IDAC1 AIN3
	UBYTE IDACMUX = (0x0a<<4) | 0x03;
	WriteReg(ADS1263::Reg::REG_IDACMUX, IDACMUX);
	DEV_Delay_ms(1);
	
	//((IDACMAG)) IDAC2 = IDAC1 = 250uA
	UBYTE IDACMAG = (0x03<<4) | 0x03;
	WriteReg(ADS1263::Reg::REG_IDACMAG, IDACMAG);
	DEV_Delay_ms(1);
	
	UBYTE MODE2 = (gain << 4) | drate;
	WriteReg(ADS1263::Reg::REG_MODE2, MODE2);
	DEV_Delay_ms(1);
	
	//INPMUX (AINP = AIN7, AINN = AIN6)
	UBYTE INPMUX = (0x07<<4) | 0x06;
	WriteReg(ADS1263::Reg::REG_INPMUX, INPMUX);
	DEV_Delay_ms(1);
	
	// REFMUX AIN4 AIN5
	UBYTE REFMUX = (0x03<<3) | 0x03;
	WriteReg(ADS1263::Reg::REG_REFMUX, REFMUX);
	DEV_Delay_ms(1);
	
	//Read one conversion
	WriteCmd(CMD_START1);
	DEV_Delay_ms(10);
	WaitDRDY();
	Value = Read_ADC1_Data();
	WriteCmd(CMD_STOP1);

	return Value;
}

/******************************************************************************
function:  DAC Test function
parameter: 
    volt :			output volt value
	isPositive :	postive or negative
	isOpen :		open or close
Info:
******************************************************************************/
void ADS1263::Dac(ADS1263::DacVoltage volt, UBYTE isPositive, UBYTE isOpen)
{
	UBYTE Reg, Value;
	
	if(isPositive)
		Reg = ADS1263::Reg::REG_TDACP;		// IN6
	else
		Reg = ADS1263::Reg::REG_TDACN;		// IN7
	
	if(isOpen)
		Value = volt | 0x80; 
	else 
		Value = 0x00;
	
	WriteReg(Reg, Value);
}

