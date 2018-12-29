/*
 *  Created on: 2018��07��08��
 *  Author: yusengo@163.com
 *
##############################################
#        ��Զ���ܿƼ����Ϻ������޹�˾
##############################################
#			��Ȩ����	����ؾ�
#			yusengo@163.com
 */
//20181229����git
#include "DSP2833x_Device.h"     // DSP2833x ͷ�ļ�
#include "DSP2833x_Examples.h"   // DSP2833x �������ͷ�ļ�

/*************************
 * shanghai jinke header files
*/
#include "SHJK_DAC.h"
#include "SHJK_UART.h"
#include "SHJK_PID.h"

//28335��88��GPIO����ΪA��B��C���飬һ����32��GPIO,A��0~31��B��32~63��C����64~87����ÿһλ���в�����
#define	  LED1	GpioDataRegs.GPADAT.bit.GPIO0  //�궨��GPA���GPIO0ΪLED1
#define	  LED2	GpioDataRegs.GPADAT.bit.GPIO1  //�궨��GPA���GPIO1ΪLED2
#define	  LED3	GpioDataRegs.GPADAT.bit.GPIO2  //�궨��GPA���GPIO2ΪLED3
#define	  LED4	GpioDataRegs.GPADAT.bit.GPIO3  //�궨��GPA���GPIO3ΪLED4
#define	  LED5	GpioDataRegs.GPADAT.bit.GPIO4  //�궨��GPA���GPIO4ΪLED5
#define	  SYNCEN	GpioDataRegs.GPBDAT.bit.GPIO58

interrupt void ISRTimer0(void);
//interrupt void ISRTimer1(void);

void LedInit(void);        //����GPIO����

/**************************************�궨��************************************************/
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
// ���� 1. ��ʼ��ϵͳ����:
// ����PLL, WatchDog, ʹ������ʱ��
// ��������������Դ�DSP2833x_SysCtrl.c�ļ����ҵ�..
   InitSysCtrl();
   InitSpiaGpio();
   InitScicGpio();

   EALLOW;
   SysCtrlRegs.HISPCP.all = ADC_MODCLK;	// ADCʱ�ӵ����� HSPCLK = SYSCLKOUT/ADC_MODCLK
   EDIS;

   DINT;

// ��ʼ��PIE�ж�����������ʹ��ָ���жϷ����ӳ���ISR��
// ��Щ�жϷ����ӳ��򱻷�����DSP280x_DefaultIsr.cԴ�ļ���
// �������������DSP2833x_PieVect.cԴ�ļ�����.
   InitPieCtrl();

// ��ֹCPU�жϺ��������CPU�жϱ�־
   IER = 0x0000;
   IFR = 0x0000;

// PIE ������ָ��ָ���жϷ����(ISR)������ʼ��.
// ��ʹ�ڳ����ﲻ��Ҫʹ���жϹ��ܣ�ҲҪ�� PIE ���������г�ʼ��.
// ��������Ϊ�˱���PIE����Ĵ���.
   InitPieVectTable();

   SHJKUartCFIFOInit();
   SHJKUartCInit();
   // ��ʼ��ADC
   InitAdc();
	// ADC�����úͲ�������
	AdcRegs.ADCTRL1.bit.ACQ_PS = 0xff;  // ˳�����ģʽ
					   //                     = 1/(3*40ns) =8.3MHz (for 150 MHz SYSCLKOUT)
					//                     = 1/(3*80ns) =4.17MHz (for 100 MHz SYSCLKOUT)
					// If Simultaneous mode enabled: Sample rate = 1/[(3+ACQ_PS)*ADC clock in ns]
	AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS; //ADC������25Mhz�£����ٷ�Ƶ
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;        // 1 ͨ��ģʽ
	AdcRegs.ADCTRL1.bit.CONT_RUN = 1;        // ��������ģʽ
	AdcRegs.ADCTRL1.bit.SEQ_OVRD = 1;        // ʹ�����򸲸�
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;   // ʹ��A0ͨ�����в���
	AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 0x0;  // ���ɼ�ͨ����Ϊ1·


	EALLOW;
	PieVectTable.TINT0 = &ISRTimer0;  //����ʱ���ж����Ӷ��ж���������
	//PieVectTable.XINT13 = &ISRTimer1;
	EDIS;
	InitCpuTimers();

	//ͨ�����������Ϳ����ö�ʱ�� 0 ÿ��һ��ʱ�����һ���жϣ����ʱ���
	//���㹫ʽΪ�� ��T= Freq * Period /150000000��s���������� 150000000 ��
	//CPU ��ʱ��Ƶ�ʣ��� 150MHz �� ʱ��Ƶ�ʣ���Դ�ʵ�飬Frep Ϊ 150��Period Ϊ 1000000����ô��T=1s��
	ConfigCpuTimer(&CpuTimer0, 150, 97.5);	//100us DAC period with 200 point to meet 20ms
	//ConfigCpuTimer(&CpuTimer1, 150, 1000);	//1ms ADC sample rate and PID T is 10ms or more

	StartCpuTimer0();  	//open timer0
	//StartCpuTimer1();	////timer1 used

    IER |= M_INT1;    //ʹ�ܵ�һ���ж�
    //IER |= M_INT13;//timer1 used

    PieCtrlRegs.PIECTRL.bit.ENPIE = 1; //ʹ�����ж�
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1; //ʹ�ܵ�һ���ж���ĵ��߸��ж�--��ʱ���ж�
	//CpuTimerRegs.TCR.all = 0x4001; //timer1 used
    EINT;   // ���ж� INTM ʹ��
    ERTM;   // ʹ����ʵʱ�ж�


    LedInit();

    SHJKSpiDacInit();
    //when time is right
	AdcRegs.ADCTRL2.all = 0x2000;            //��ʼת��

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
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; //0x0001����12���ж�ACKnowledge�Ĵ���������ȫ������������������ж�
    CpuTimer0Regs.TCR.bit.TIF=1;  // ��ʱ����ָ��ʱ�䣬��־λ��λ�������־
    CpuTimer0Regs.TCR.bit.TRB=1;  // ����Timer0�Ķ�ʱ����
    //LED1���ӷ�����

    //SsetSineAmp = GvoltCom *1.0/10000;
	 // Transmit data
    SHJKSineGen(SsetSineAmp,dacGenIndex);

    dacGenIndex++;
	if(dacGenIndex >= NSINE)
		dacGenIndex = 0;

/*
	AdcRawData[adcIndex] = ADCdata;//��ת���Ľ���͸�AdcRawData����

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
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0; // GPIO����ΪGPIO����
	GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;  // GPIO����Ϊ���
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0; // GPIO����ΪGPIO����
	GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;  // GPIO����Ϊ���
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0; // GPIO����ΪGPIO����
	GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;  // GPIO����Ϊ���
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0; // GPIO����ΪGPIO����
	GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;  // GPIO����Ϊ���
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0; // GPIO����ΪGPIO����
	GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;  // GPIO����Ϊ���
	/*ADDED*/
	GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 0; // GPIO����ΪGPIO����
	GpioCtrlRegs.GPBDIR.bit.GPIO58 = 1;  // GPIO����Ϊ���
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