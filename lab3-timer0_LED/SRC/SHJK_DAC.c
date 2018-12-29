#include"SHJK_DAC.h"

extern volatile float GSINEBASE = 2047;
extern volatile float GAMPSINE	= 1.0;
extern volatile float GADCRMS	= 0.0f;

extern volatile Uint16 PCdata	= 0;
extern volatile Uint16 ADCdata	= 0;

volatile Uint16 AdcRawData[NSINE] = {0};
volatile float AdcFltData[NSINE] = {0};


//200 point
extern const float SINEWAVE_DATA[NSINE] = {\
		0,
		64.3,
		128.53,
		192.64,
		256.56,
		320.22,
		383.57,
		446.54,
		509.07,
		571.09,
		632.56,
		693.4,
		753.55,
		812.96,
		871.57,
		929.32,
		986.15,
		1042.01,
		1096.84,
		1150.58,
		1203.2,
		1254.62,
		1304.81,
		1353.71,
		1401.27,
		1447.45,
		1492.2,
		1535.48,
		1577.24,
		1617.45,
		1656.06,
		1693.03,
		1728.34,
		1761.94,
		1793.8,
		1823.89,
		1852.18,
		1878.64,
		1903.25,
		1925.98,
		1946.81,
		1965.72,
		1982.69,
		1997.7,
		2010.74,
		2021.8,
		2030.86,
		2037.92,
		2042.96,
		2045.99,
		2047,
		2045.99,
		2042.96,
		2037.92,
		2030.86,
		2021.8,
		2010.74,
		1997.7,
		1982.69,
		1965.72,
		1946.81,
		1925.98,
		1903.25,
		1878.64,
		1852.18,
		1823.89,
		1793.8,
		1761.94,
		1728.34,
		1693.03,
		1656.06,
		1617.45,
		1577.24,
		1535.48,
		1492.2,
		1447.45,
		1401.27,
		1353.71,
		1304.81,
		1254.62,
		1203.2,
		1150.58,
		1096.84,
		1042.01,
		986.15,
		929.32,
		871.57,
		812.96,
		753.55,
		693.4,
		632.56,
		571.09,
		509.07,
		446.54,
		383.57,
		320.22,
		256.56,
		192.64,
		128.53,
		64.3,
		0,
		-64.3,
		-128.53,
		-192.64,
		-256.56,
		-320.22,
		-383.57,
		-446.54,
		-509.07,
		-571.09,
		-632.56,
		-693.4,
		-753.55,
		-812.96,
		-871.57,
		-929.32,
		-986.15,
		-1042.01,
		-1096.84,
		-1150.58,
		-1203.2,
		-1254.62,
		-1304.81,
		-1353.71,
		-1401.27,
		-1447.45,
		-1492.2,
		-1535.48,
		-1577.24,
		-1617.45,
		-1656.06,
		-1693.03,
		-1728.34,
		-1761.94,
		-1793.8,
		-1823.89,
		-1852.18,
		-1878.64,
		-1903.25,
		-1925.98,
		-1946.81,
		-1965.72,
		-1982.69,
		-1997.7,
		-2010.74,
		-2021.8,
		-2030.86,
		-2037.92,
		-2042.96,
		-2045.99,
		-2047,
		-2045.99,
		-2042.96,
		-2037.92,
		-2030.86,
		-2021.8,
		-2010.74,
		-1997.7,
		-1982.69,
		-1965.72,
		-1946.81,
		-1925.98,
		-1903.25,
		-1878.64,
		-1852.18,
		-1823.89,
		-1793.8,
		-1761.94,
		-1728.34,
		-1693.03,
		-1656.06,
		-1617.45,
		-1577.24,
		-1535.48,
		-1492.2,
		-1447.45,
		-1401.27,
		-1353.71,
		-1304.81,
		-1254.62,
		-1203.2,
		-1150.58,
		-1096.84,
		-1042.01,
		-986.15,
		-929.32,
		-871.57,
		-812.96,
		-753.55,
		-693.4,
		-632.56,
		-571.09,
		-509.07,
		-446.54,
		-383.57,
		-320.22,
		-256.56,
		-192.64,
		-128.53,
		-64.3,
};

/*50 point
extern const float SINEWAVE_DATA[NSINE] = {0,
		256.56,
		509.07,
		753.55,
		986.15,
		1203.2,
		1401.27,
		1577.24,
		1728.34,
		1852.18,
		1946.81,
		2010.74,
		2042.96,
		2042.96,
		2010.74,
		1946.81,
		1852.18,
		1728.34,
		1577.24,
		1401.27,
		1203.2,
		986.15,
		753.55,
		509.07,
		256.56,
		0,
		-256.56,
		-509.07,
		-753.55,
		-986.15,
		-1203.2,
		-1401.27,
		-1577.24,
		-1728.34,
		-1852.18,
		-1946.81,
		-2010.74,
		-2042.96,
		-2042.96,
		-2010.74,
		-1946.81,
		-1852.18,
		-1728.34,
		-1577.24,
		-1401.27,
		-1203.2,
		-986.15,
		-753.55,
		-509.07,
		-256.56,
};
*/
//FILTER BELOW
#define FIRTAP 11
const float HfilterParaSet[FIRTAP] = {0,0.0189,0.0688,0.1308,0.1812,0.2004,0.1812,0.1308,0.0688,0.0189,0};
/*
 * input : 16 bit ADC sample data
 * output: float output
 * fun   : FIR 10-ord hann window filter
 */
float SHJKFIRFilter(Uint16 adcRawData);
/*
 * input : 12 bit ADC sample data
 * output: float output
 * fun   : FIR 10-ord hann window filter
 */
float SHJKFIRFilter(Uint16 rawData)
{
	static char filterStartFlag = 0;
	static Uint16 windowData[FIRTAP] = {0};
	char m = 0;
	float fltData = 0.0;

	if(filterStartFlag < FIRTAP)
	{
		windowData[filterStartFlag] = rawData;
		filterStartFlag++;

		fltData = (float)rawData;
	}
	else
	{
		for(m = 0;m < FIRTAP;m++)
			fltData += HfilterParaSet[m]*windowData[FIRTAP - 1 - m];

		for(m = 0;m < (FIRTAP - 1);m++)
			windowData[m] = windowData[m+1];

		windowData[FIRTAP - 1] = rawData;
	}

	return fltData;
}

/*
 * input:			none
 * output:			none
 * function:		init spi for dac
 * description: 	DAC121S01 SPI mode : CPHA = 1 CPOL = 0
 * */
void SHJKSpiDacInit(void)
{/*
	EALLOW;
	GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0;	// GPIO复用为GPIO功能
	GpioCtrlRegs.GPBDIR.bit.GPIO57 	= 1;	// GPIO设置为输出
	SENH;
	EDIS;*/
// Initialize SPI FIFO registers
	SpiaRegs.SPIFFTX.all=0xE040;
	SpiaRegs.SPIFFRX.all=0x204f;
	SpiaRegs.SPIFFCT.all=0x0;

	SpiaRegs.SPICCR.all = 0x000F;	             // Reset on, rising edge, 16-bit char bits
	SpiaRegs.SPICTL.all = 0x0006;    		     // Enable master mode, clk phase 1,
												 // enable talk, and SPI int disabled.
	//SpiaRegs.SPICTL.bit.CLK_PHASE = 1;
	SpiaRegs.SPIBRR = 0x0007;//0007
	SpiaRegs.SPICCR.all = 0x008F;		         // Relinquish SPI from Reset,clock polarity 0,loopback disable
	SpiaRegs.SPIPRI.bit.FREE = 1;                // Set so breakpoints don't disturb xmission

	/*
	//初始化SPI配置控制寄存器
	SpiaRegs.SPICCR.bit.SPISWRESET = 0;//清除SPI相关操作标志位，准备配置SPI接口
	SpiaRegs.SPICCR.bit.CLKPOLARITY = 0;//设置SPI移位时钟极性
	SpiaRegs.SPICCR.bit.SPILBK = 0;//该位为0则禁止SPI自测试功能，为1则允许SPI自测试功能
	SpiaRegs.SPICCR.bit.SPICHAR = 15;//SPI数据长度为8位
	//初始化SPI操作控制寄存器
	SpiaRegs.SPICTL.bit.OVERRUNINTENA = 0;	//禁止接收器溢出中断
	SpiaRegs.SPICTL.bit.CLK_PHASE = 1;		//
	SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;	//SPI工作模式选择，1主控制器模式,0从机模式
	SpiaRegs.SPICTL.bit.TALK = 1;			//使能SPI接口4个引脚的相应功能
	SpiaRegs.SPICTL.bit.SPIINTENA = 1;		//使能SPI中断
	SpiaRegs.SPICCR.bit.SPISWRESET = 1;
	*/
}

void SHJKDacOutput(Uint16 code)
{
	Uint16 dataTemp = 0;
	SpiaRegs.SPITXBUF = code&0x0FFF;

    // Wait until data is received
    while(SpiaRegs.SPIFFRX.bit.RXFFST !=1) { }
	dataTemp = SpiaRegs.SPIRXBUF;		//read RX BUF is necessary
}

/*void SHJKSineGen(float ampSine,Uint16 index)
{
	if(ampSine > 0.7f)
		ampSine = 0.7f;

	if(ampSine < 0.0000f)
		ampSine = 0.000f;

	SHJKDacOutput(1);
	SHJKDacOutput((Uint16)(GSINEBASE + ampSine*SINEWAVE_DATA[index]));
}*/


Uint16 SHJK_DSP_FPGA_COM(Uint16 code)
{
	Uint16 dataTemp = 0;
	SpiaRegs.SPITXBUF = (code)&0x0FFF;

    // Wait until data is received
    while(SpiaRegs.SPIFFRX.bit.RXFFST !=1) { }
	dataTemp = SpiaRegs.SPIRXBUF;		//read RX BUF is necessary

	return dataTemp;
}

Uint16 SHJKComVoltGet()
{
	Uint16 comData = 0;

	SHJK_DSP_FPGA_COM(10);
	comData = SHJK_DSP_FPGA_COM(0);

	return comData;
}

void SHJKSineGen(float rmsSine,Uint16 index)
{
	static float max = 0.0f;
	static float min = 0.0f;
	//static Uint16 ADCTemp = 0；

	//static float avg = 0.0f;

	if(rmsSine > 0.6f)
		rmsSine = 0.55f;

	if(rmsSine < 0.0000f)
		rmsSine = 0.000f;

	PCdata = SHJK_DSP_FPGA_COM(1);
	ADCdata= SHJK_DSP_FPGA_COM((Uint16)(GSINEBASE + rmsSine*1.414f*SINEWAVE_DATA[index]));

	AdcRawData[index] = ADCdata;					//将转换的结果送给AdcRawData数组
	AdcFltData[index] = SHJKFIRFilter(ADCdata);
/*
	if(AdcRegs.ADCST.bit.INT_SEQ1 == 0)
	{
		AdcRawData[index] = ADCTemp;				//将转换的结果送给AdcRawData数组
		AdcFltData[index] = SHJKFIRFilter(ADCTemp);
	}//转换未结束
	else
	{
		AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;      	//清除中断标志位
		ADCTemp = ( (AdcRegs.ADCRESULT0)>>4);

		AdcRawData[index] = ADCTemp;				//将转换的结果送给AdcRawData数组
		AdcFltData[index] = SHJKFIRFilter(ADCTemp);
	}//转换结束
*/
	if(index == 0)
	{
		max = AdcFltData[0];
		min = AdcFltData[0];
		//avg = AdcFltData[0];
	}
	else
	{
		//avg += AdcFltData[index];

		if(AdcFltData[index] > max)
			max = AdcFltData[index];

		if(AdcFltData[index] < min)
			min = AdcFltData[index];

		if(index == 199)
			GADCRMS = ((max - min) * 0.707f);// + (avg * 1.0/200);// rms = avg + vpp/2sqrt(2)
	}
}
