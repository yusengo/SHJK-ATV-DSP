/*
 *  Created on: 2018年07月08日
 *  Author: yusengo@163.com
 *
##############################################
#        炬远智能科技（上海）有限公司
##############################################
#			版权所有	盗版必究
#			yusengo@163.com
 */

#include "DSP2833x_Device.h"     // DSP2833x 头文件
#include "DSP2833x_Examples.h"   // DSP2833x 例子相关头文件

/*************************
 * shanghai jinke header files
*/
#include "SHJK_DAC.h"
#include "SHJK_UART.h"
#include "SHJK_PID.h"

//28335有88个GPIO，分为A、B、C三组，一组是32个GPIO,A组0~31，B组32~63，C组是64~87。对每一位进行操作。
#define	  LED1	GpioDataRegs.GPADAT.bit.GPIO0  //宏定义GPA组的GPIO0为LED1
#define	  LED2	GpioDataRegs.GPADAT.bit.GPIO1  //宏定义GPA组的GPIO1为LED2
#define	  LED3	GpioDataRegs.GPADAT.bit.GPIO2  //宏定义GPA组的GPIO2为LED3
#define	  LED4	GpioDataRegs.GPADAT.bit.GPIO3  //宏定义GPA组的GPIO3为LED4
#define	  LED5	GpioDataRegs.GPADAT.bit.GPIO4  //宏定义GPA组的GPIO4为LED5
#define	  SYNCEN	GpioDataRegs.GPBDAT.bit.GPIO58

interrupt void ISRTimer0(void);
//interrupt void ISRTimer1(void);

void LedInit(void);        //声明GPIO配置

/**************************************宏定义************************************************/
#define POST_SHIFT   0  // Shift results after the entire sample table is full
#define INLINE_SHIFT 1  // Shift results as the data is taken from the results regsiter
#define NO_SHIFT     0  // Do not shift the results

// ADC start parameters
#if (CPU_FRQ_150MHZ)     // Default - 150 MHz SYSCLKOUT
  #define ADC_MODCLK 0x3 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 150/(2*3)   = 25.0 MHz
#endif
#if (CPU_FRQ_100MHZ)
  #define ADC_MODCLK 0x2 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 100/(2*2)   = 25.0 MHz
#endif
#define ADC_CKPS   0x0   // ADC module clock = HSPCLK/1      = 25.5MHz/(1)   = 25.0 MHz
#define ADC_SHCLK  0x1   // S/H width in ADC module periods                  = 2 ADC cycle
#define AVG        1000  // Average sample limit
#define ZOFFSET    0x00  // Average Zero offset




//for ADC para
volatile float GADC_BASE = 1956;


volatile Uint16 Gwork = 1;
volatile Uint16 GvoltCom = 0;


volatile Uint16 TimerCNT = 0;
volatile float SsetSineAmp = 0;
volatile Uint16 voltNow = 0;
volatile Uint16 pidi = 0;
volatile Uint16 pidOK = 1;

//PID BELOW
#define DACAMP 200
volatile float dacAmpSet[DACAMP] = {0};



/*
float SineFilteredADCRMSCalc(void)
{
	float max = AdcFltData[0];
	float min = AdcFltData[0];
	Uint16 i = 1,maxIndex = 0,minIndex = 0;

	for(i = 1;i < ADCBUFSIZE;i++)
	{
		if(AdcFltData[i] > max)
		{
			max = AdcFltData[i];
			maxIndex = i;
		}

		if(AdcFltData[i] < min)
		{
			min = AdcFltData[i];
			minIndex = i;
		}
	}

	return (max - min) * 0.707f;// dpp: max - min ;vpp : dpp*3.0 /4096
}
*/
void main(void)
{
// 步骤 1. 初始化系统控制:
// 设置PLL, WatchDog, 使能外设时钟
// 下面这个函数可以从DSP2833x_SysCtrl.c文件中找到..
   InitSysCtrl();
   InitSpiaGpio();
   InitScicGpio();

   EALLOW;
   SysCtrlRegs.HISPCP.all = ADC_MODCLK;	// ADC时钟的配置 HSPCLK = SYSCLKOUT/ADC_MODCLK
   EDIS;

   DINT;

// 初始化PIE中断向量表，并使其指向中断服务子程序（ISR）
// 这些中断服务子程序被放在了DSP280x_DefaultIsr.c源文件中
// 这个函数放在了DSP2833x_PieVect.c源文件里面.
   InitPieCtrl();

// 禁止CPU中断和清除所有CPU中断标志
   IER = 0x0000;
   IFR = 0x0000;

// PIE 向量表指针指向中断服务程(ISR)完成其初始化.
// 即使在程序里不需要使用中断功能，也要对 PIE 向量表进行初始化.
// 这样做是为了避免PIE引起的错误.
   InitPieVectTable();

   SHJKUartCFIFOInit();
   SHJKUartCInit();
   // 初始化ADC
   InitAdc();
	// ADC的配置和参数设置
	AdcRegs.ADCTRL1.bit.ACQ_PS = 0xff;  // 顺序采样模式
					   //                     = 1/(3*40ns) =8.3MHz (for 150 MHz SYSCLKOUT)
					//                     = 1/(3*80ns) =4.17MHz (for 100 MHz SYSCLKOUT)
					// If Simultaneous mode enabled: Sample rate = 1/[(3+ACQ_PS)*ADC clock in ns]
	AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS; //ADC工作在25Mhz下，不再分频
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;        // 1 通道模式
	AdcRegs.ADCTRL1.bit.CONT_RUN = 1;        // 连续采样模式
	AdcRegs.ADCTRL1.bit.SEQ_OVRD = 1;        // 使能排序覆盖
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;   // 使能A0通道进行采样
	AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 0x0;  // 最大采集通道数为1路


	EALLOW;
	PieVectTable.TINT0 = &ISRTimer0;  //将定时器中断添加都中断向量表里
	//PieVectTable.XINT13 = &ISRTimer1;
	EDIS;
	InitCpuTimers();

	//通过以下面程序就可以让定时器 0 每隔一段时间产生一次中断，这段时间的
	//计算公式为： △T= Freq * Period /150000000（s）；（其中 150000000 是
	//CPU 的时钟频率，即 150MHz 的 时钟频率）针对此实验，Frep 为 150，Period 为 1000000，那么△T=1s。
	ConfigCpuTimer(&CpuTimer0, 150, 97.5);	//100us DAC period with 200 point to meet 20ms
	//ConfigCpuTimer(&CpuTimer1, 150, 1000);	//1ms ADC sample rate and PID T is 10ms or more

	StartCpuTimer0();  	//open timer0
	//StartCpuTimer1();	////timer1 used

    IER |= M_INT1;    //使能第一组中断
    //IER |= M_INT13;//timer1 used

    PieCtrlRegs.PIECTRL.bit.ENPIE = 1; //使能总中断
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1; //使能第一组中断里的第七个中断--定时器中断
	//CpuTimerRegs.TCR.all = 0x4001; //timer1 used
    EINT;   // 总中断 INTM 使能
    ERTM;   // 使能总实时中断


    LedInit();

    SHJKSpiDacInit();
    //when time is right
	AdcRegs.ADCTRL2.all = 0x2000;            //开始转换

	char RcharTemp;
	char RChar[8];


	Uint16 adccnt = 0;
	Gwork = 1;
	SYNCEN = 1;
/*
	while(1)
	{
		if(ScicRegs.SCIRXST.bit.RXRDY == 1)
		{
			RcharTemp = ScicRegs.SCIRXBUF.all;

			RChar[0] = RChar[1];
			RChar[1] = RChar[2];
			RChar[2] = RChar[3];
			RChar[3] = RChar[4];
			RChar[4] = RChar[5];
			RChar[5] = RChar[6];
			RChar[6] = RChar[7];
			RChar[7] = RcharTemp;

			if((RChar[0] == 'S')&&(RChar[1] == 'E')&&(RChar[2] == 'T')&&(RChar[7] == 'V'))
				SHJKUartCSendChar('K');
		}
	}
*/
	while(1)
	{
		if(TimerCNT == 0)//PID period is 20ms
		{
//			TimerCNT = 0;
/*
			if(voltNow != GvoltCom)
			{
				voltNow = GvoltCom;
				pidOK = 200;
				pidi = 0;
			}

			if(pidOK > 1)
			{
				pidOK--;

				if((pidOK < 199)&&(GADCRMS < 5))
				{
					pidOK = 1;
					SsetSineAmp = 0;
					SHJKUartCSendChar('\n');
					SHJKUartCSendChar('E');
					SHJKUartCSendChar('2');
					SHJKUartCSendChar('0');
				}
				else
					PID_Caculate(GADCRMS,voltNow);
			}

			if(pidi < DACAMP)
				dacAmpSet[pidi++] = SsetSineAmp;
*/
//			SsetSineAmp = GvoltCom *1.0/10000;//will delete after

			adccnt++;
			if(adccnt > 500)
			{
				adccnt = 0;
				SHJKUartCSendChar('\n');
				SHJKUartCSendChar((Uint16)( 1.0F * GvoltCom)/10000 + '0');
				SHJKUartCSendChar(((Uint16)(1.0F * GvoltCom)%10000)/1000 + '0');
				SHJKUartCSendChar(((Uint16)(1.0F * GvoltCom)%1000)/100 + '0');
				SHJKUartCSendChar(((Uint16)(1.0F * GvoltCom)%100)/10 + '0');
				SHJKUartCSendChar(((Uint16)(1.0F * GvoltCom)%10) + '0');//GADCRMS
				SHJKUartCSendChar('\n');
			}
		}
/*
		if(ScicRegs.SCIRXST.bit.RXRDY == 1)
		{
			RcharTemp = ScicRegs.SCIRXBUF.all;

			RChar[0] = RChar[1];
			RChar[1] = RChar[2];
			RChar[2] = RChar[3];
			RChar[3] = RChar[4];
			RChar[4] = RChar[5];
			RChar[5] = RChar[6];
			RChar[6] = RChar[7];
			RChar[7] = RcharTemp;

			if((RChar[0] == 'S')&&(RChar[1] == 'E')&&(RChar[2] == 'T')&&(RChar[7] == 'V'))
			{
				SHJKUartCSendChar('V');
				GvoltCom = ((RChar[3] - '0')*1000 + (RChar[4] - '0')*100 + (RChar[5] - '0')*10 + RChar[6] - '0');

				if(GvoltCom < 5)
					GvoltCom = 5;
				if(GvoltCom > 5000)
					GvoltCom = 5000;
			}

			if((RChar[0] == 'S')&&(RChar[1] == 'T')&&(RChar[7] == 'P'))
			{
				SHJKUartCSendChar('P');
				pidStr.KP = RChar[2] - '0' + ((RChar[3] - '0')*1000 + (RChar[4] - '0')*100 + (RChar[5] - '0')*10 + RChar[6] - '0')*1.000/10000;
			}

			if((RChar[0] == 'S')&&(RChar[1] == 'T')&&(RChar[7] == 'I'))
			{
				SHJKUartCSendChar('I');
				pidStr.KI = RChar[2] - '0' + ((RChar[3] - '0')*1000 + (RChar[4] - '0')*100 + (RChar[5] - '0')*10 + RChar[6] - '0')*1.000/10000;
			}

			if((RChar[0] == 'S')&&(RChar[1] == 'T')&&(RChar[7] == 'D'))
			{
				SHJKUartCSendChar('D');
				pidStr.KD = RChar[2] - '0' + ((RChar[3] - '0')*1000 + (RChar[4] - '0')*100 + (RChar[5] - '0')*10 + RChar[6] - '0')*1.000/10000;
			}
		}
*/
	}
	for(;;);
}

/*
void AdcBaseCalc(void)
{
	Uint16 i = 0;
	Uint32 sumofAll = 0;

	for(i = 0;i < ADCBUFSIZE;i++)
		sumofAll += AdcRawData[i];

	GADC_BASE = sumofAll * 1.00 / ADCBUFSIZE;
}
*/
interrupt void ISRTimer0(void)
{
	static Uint16 dacGenIndex = 0;
	static Uint16 restart = 0;

   // Acknowledge this interrupt to receive more interrupts from group 1
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; //0x0001赋给12组中断ACKnowledge寄存器，对其全部清除，不接受其他中断
    CpuTimer0Regs.TCR.bit.TIF=1;  // 定时到了指定时间，标志位置位，清除标志
    CpuTimer0Regs.TCR.bit.TRB=1;  // 重载Timer0的定时数据
    //LED1连接蜂鸣器

    //SsetSineAmp = GvoltCom *1.0/10000;
	 // Transmit data
    SHJKSineGen(SsetSineAmp,dacGenIndex);

    dacGenIndex++;
	if(dacGenIndex >= NSINE)
		dacGenIndex = 0;

/*
	AdcRawData[adcIndex] = ADCdata;//将转换的结果送给AdcRawData数组

	AdcFltData[adcIndex] = SHJKFIRFilter(ADCdata);

	adcIndex++;
	if(adcIndex >= ADCBUFSIZE)
		adcIndex = 0;
*/

	TimerCNT++;

	if(TimerCNT >= 200)//PID period is 20ms
	{
		TimerCNT = 0;

	    GvoltCom = SHJKComVoltGet();
	    //SsetSineAmp = ((GvoltCom >= 32768)? (GvoltCom - 32768):GvoltCom) * 1.0f / 10000;

		if(GvoltCom >= 32768)
		{
			SsetSineAmp = (GvoltCom - 32768) * 1.0f / 10000;
			restart = 1;
			voltNow = 0;
		}
		else
		{
			if(voltNow != GvoltCom)
			{
				if(TargetFivePointCalc(GvoltCom,voltNow,SsetSineAmp))
				{
					voltNow = GvoltCom;
					pidOK = 5;
					pidi  = 0;
				}
			}

			if((restart == 1)||(GvoltCom == 0))
			{
				restart = 0;
				Gwork = 1;
			}
			else
			{
				if((Gwork == 1)&&(GADCRMS < 5)&&(SsetSineAmp > 0.01f))
				{
					Gwork = 0;
				}
			}

			if(pidOK > 0)
			{
				//SsetSineAmp = voltNow * 1.0f / 10000;
				SsetSineAmp = (Gwork == 1)? targetFivePoint[pidOK - 1] : 0.0000f;
				pidOK--;
			}
		}
	}
}

void LedInit(void)
{
	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0; // GPIO复用为GPIO功能
	GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;  // GPIO设置为输出
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0; // GPIO复用为GPIO功能
	GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;  // GPIO设置为输出
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0; // GPIO复用为GPIO功能
	GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;  // GPIO设置为输出
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0; // GPIO复用为GPIO功能
	GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;  // GPIO设置为输出
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0; // GPIO复用为GPIO功能
	GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;  // GPIO设置为输出
	/*ADDED*/
	GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 0; // GPIO复用为GPIO功能
	GpioCtrlRegs.GPBDIR.bit.GPIO58 = 1;  // GPIO设置为输出
	//END
	EDIS;

	LED1=1;
	LED2=1;
	LED3=0;
	LED4=1;
	LED5=0;
}



//===========================================================================
// No more.
//===========================================================================
