/*****************************************************************************
* | File      	:   ADS1256.c
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
#include "adc/ads1256/ADS1256.h"

#include "adc/device_config.h"

using namespace adc::ads1256;

/******************************************************************************
function:   Module reset
parameter:
Info:
******************************************************************************/
void ADS1256::reset()
{
    DEV_Digital_Write(DEV_RST_PIN, 1);
    DEV_Delay_ms(200);
    DEV_Digital_Write(DEV_RST_PIN, 0);
    DEV_Delay_ms(200);
    DEV_Digital_Write(DEV_RST_PIN, 1);
}

/******************************************************************************
function:   send command
parameter: 
        Cmd: command
Info:
******************************************************************************/
void ADS1256::WriteCmd(uint8_t Cmd)
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
void ADS1256::WriteReg(uint8_t Reg, uint8_t data)
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
uint8_t ADS1256::Read_data(uint8_t Reg)
{
    uint8_t temp = 0;
    DEV_Digital_Write(DEV_CS_PIN, 0);
    DEV_SPI_WriteByte(CMD_RREG | Reg);
    DEV_SPI_WriteByte(0x00);
    DEV_Delay_ms(1);
    temp = DEV_SPI_ReadByte();
    DEV_Digital_Write(DEV_CS_PIN, 1);
    return temp;
}

/******************************************************************************
function:   Waiting for a busy end
parameter: 
Info:
    Timeout indicates that the operation is not working properly.
******************************************************************************/
void ADS1256::WaitDRDY()
{
    uint32_t i = 0;
    for (i = 0; i < 4000000; i++) {
        if (DEV_Digital_Read(DEV_DRDY_PIN) == 0)
            break;
    }
    if (i >= 4000000) {
        printf("Time Out ...\r\n");
    }
}

/******************************************************************************
function:  Read device ID
parameter: 
Info:
******************************************************************************/
uint8_t ADS1256::ReadChipID()
{
    uint8_t id;
    WaitDRDY();
    id = Read_data(REG_STATUS);
    return id >> 4;
}

/******************************************************************************
function:  Configure ADC gain and sampling speed
parameter: 
    gain : Enumeration type gain
    drate: Enumeration type sampling speed
Info:
******************************************************************************/
void ADS1256::ConfigADC(ADS1256::Gain gain, ADS1256::Drate drate)
{
    WaitDRDY();
    
    uint8_t buf[4] = { 0, 0, 0, 0 };

    buf[0] = (0 << 3) | (1 << 2) | (0 << 1);
    buf[1] = 0x08;
    buf[2] = (0 << 5) | (0 << 3) | (gain << 0);
    buf[3] = drate_e[drate];
    
    DEV_Digital_Write(DEV_CS_PIN, 0);
    DEV_SPI_WriteByte(CMD_WREG | 0);
    DEV_SPI_WriteByte(0x03);

    DEV_SPI_WriteByte(buf[0]);
    DEV_SPI_WriteByte(buf[1]);
    DEV_SPI_WriteByte(buf[2]);
    DEV_SPI_WriteByte(buf[3]);
    DEV_Digital_Write(DEV_CS_PIN, 1);
    DEV_Delay_ms(1);
}

/******************************************************************************
function:  Set the channel to be read
parameter: 
    Channal : Set channel number
Info:
******************************************************************************/
void ADS1256::SetChannal(uint8_t Channal)
{
    if (Channal > 7) {
        return;
    }

    WriteReg(REG_MUX, (Channal << 4) | (1 << 3));
}

void ADS1256::SetDiffChannal(uint8_t Channal)
{
    if (Channal == 0) {
        WriteReg(REG_MUX, (0 << 4) | 1); //DiffChannal  AIN0-AIN1
    } else if (Channal == 1) {
        WriteReg(REG_MUX, (2 << 4) | 3); //DiffChannal   AIN2-AIN3
    } else if (Channal == 2) {
        WriteReg(REG_MUX, (4 << 4) | 5); //DiffChannal    AIN4-AIN5
    } else if (Channal == 3) {
        WriteReg(REG_MUX, (6 << 4) | 7); //DiffChannal   AIN6-AIN7
    }
}

/******************************************************************************
function:  Setting mode
parameter: 
    Mode : 0 Single-ended input
           1 channel1 Differential input
Info:
******************************************************************************/
void ADS1256::SetMode(uint8_t Mode)
{
    if (Mode == 0) {
        ScanMode = 0;
    } else {
        ScanMode = 1;
    }
}

/******************************************************************************
function:  Device initialization
parameter: 
Info:
******************************************************************************/
uint8_t ADS1256::init()
{
    reset();

    DEV_Delay_ms(20);

    for (int i = 0; i < 3; ++i) {
        if (ReadChipID() == 3) {
            printf("ID Read success \r\n");
            break;
        } else {
            printf("ID Read failed \r\n");
            if (i == 2)
                return 1;
        }
    }
    ConfigADC(ADS1256::Gain::ADS1256_GAIN_64, ADS1256::Drate::ADS1256_15000SPS);
    WriteReg(ADS1256::Reg::REG_STATUS, (1 << 1));

    return 0;
}

/******************************************************************************
function:  Read ADC data
parameter: 
Info:
******************************************************************************/
uint32_t ADS1256::Read_ADC_Data()
{
    uint32_t read = 0;
    uint8_t buf[3] = { 0, 0, 0 };
    DEV_Digital_Write(DEV_CS_PIN, 0);
    DEV_SPI_WriteByte(CMD_RDATA);
    DEV_Delay_ms(1);
    buf[0] = DEV_SPI_ReadByte();
    buf[1] = DEV_SPI_ReadByte();
    buf[2] = DEV_SPI_ReadByte();
    DEV_Digital_Write(DEV_CS_PIN, 1);
    read = ((uint32_t)buf[0] << 16) & 0x00FF0000;
    read |= ((uint32_t)buf[1] << 8); /* Pay attention to It is wrong   read |= (buf[1] << 8) */
    read |= buf[2];
    //printf("%d  %d  %d \r\n",buf[0],buf[1],buf[2]);
    if (read & 0x800000)
        read |= 0xFF000000;
    return read;
}
/******************************************************************************
function:  Read ADC data Lite
parameter: 
Info:
******************************************************************************/
uint32_t ADS1256::Read_ADC_Data_Lite()
{
    uint32_t read = 0;
    uint8_t buf[3] = { 0, 0, 0 };
    buf[0] = DEV_SPI_ReadByte();
    buf[1] = DEV_SPI_ReadByte();
    buf[2] = DEV_SPI_ReadByte();
    read = ((uint32_t)buf[0] << 16) & 0x00FF0000;
    read |= ((uint32_t)buf[1] << 8); /* Pay attention to It is wrong   read |= (buf[1] << 8) */
    read |= buf[2];
    //printf("%d  %d  %d \r\n",buf[0],buf[1],buf[2]);
    if (read & 0x800000)
        read |= 0xFF000000;
    return read;
}
/******************************************************************************
function:  Read ADC specified channel data
parameter: 
    Channel: Channel number
Info:
******************************************************************************/
uint32_t ADS1256::GetChannalValue(uint8_t Channel)
{
    uint32_t Value = 0;
    while (DEV_Digital_Read(DEV_DRDY_PIN) == 1)
        ;
    if (ScanMode == 0) { // 0  Single-ended input  8 channel1 Differential input  4 channe
        if (Channel >= 8) {
            return 0;
        } else if (Channel == 7)
            Channel = 0;
        else
            Channel = Channel + 1;
        SetChannal(Channel);
        WriteCmd(CMD_SYNC);
        DEV_Delay_ms(1);
        WriteCmd(CMD_WAKEUP);
        DEV_Delay_ms(1);
        Value = Read_ADC_Data();
    } else {
        if (Channel >= 4) {
            return 0;
        }
        SetDiffChannal(Channel);
        WriteCmd(CMD_SYNC);
        DEV_Delay_ms(1);
        WriteCmd(CMD_WAKEUP);
        DEV_Delay_ms(1);
        Value = Read_ADC_Data();
    }
    return Value;
}

/******************************************************************************
function:  Read data from all channels
parameter: 
    ADC_Value : ADC Value
Info:
******************************************************************************/
void ADS1256::GetAll(uint32_t* ADC_Value)
{
    uint8_t i;
    for (i = 0; i < 8; i++) {
        ADC_Value[i] = GetChannalValue(i);
    }
}
