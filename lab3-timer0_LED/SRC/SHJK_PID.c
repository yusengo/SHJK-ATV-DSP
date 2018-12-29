/*
 * SHJK_PID.c
 *
 *  Created on: 2018年3月5日
 *      Author: yusengo@163.com
 */

#include "SHJK_PID.h"
/* para1:P 0.32		I 0.0071 	D 0.0045
 * para2:P 0.05 	I 0.0071 	D 0.0045
 * para3:P 0.042	I 0.042		D 0.0024
 *
 */
#define gpNUM 17

//[k0,k100,k1000,k2000,k3000,k4000,k5000];RMS校准依据
Uint16 g_pValue[gpNUM] 	      = {0,   	114,    500,	1230,  	 1440,	 1920,   2160,	 2512,   2640,	 3120,	 3360, 	 3865,   3840,	 4320,	 4560,	 5162,    5419};
float g_pAmp[gpNUM] 		  = {0.0, 	0.01,   0.05,   0.10,    0.12,   0.16,   0.18,   0.20,   0.22,   0.26,   0.28,   0.30,   0.32,   0.36,   0.38,   0.40,    0.42};

extern volatile float targetFivePoint[5] = {0};//0.1s

Uint16 HalfJudgeVoltIndex(Uint16 volt)
{
	Uint16 start = 0;
	Uint16 end = (gpNUM - 1);
	Uint16 pos = (Uint16)((gpNUM - 1)/2);

	while(start != pos)
	{
		if(volt < g_pValue[pos])
		{
			end = pos;
			pos = (Uint16)((start + end)/2);
		}
		else
		{
			start = pos;
			pos = (Uint16)((start + end)/2);
		}
	}

	return pos;
}

Uint16 TargetFivePointCalc(Uint16 rmsVoltTarget,Uint16 rmsVoltNow,float ampNow)
{
	Uint16 indexOfArray = 0;
	float gpAmp = 0.0f;

    if((rmsVoltTarget  == rmsVoltNow)||(rmsVoltTarget >= g_pValue[gpNUM - 1]))
		return 0;

    indexOfArray = HalfJudgeVoltIndex(rmsVoltTarget);

	if(indexOfArray >= (gpNUM - 1))
		return 0;

    gpAmp = g_pAmp[indexOfArray] + ((rmsVoltTarget - g_pValue[indexOfArray]) * 1.0f * (g_pAmp[indexOfArray + 1] - g_pAmp[indexOfArray])/ (g_pValue[indexOfArray + 1] - g_pValue[indexOfArray]));

    if(gpAmp > g_pAmp[gpNUM - 1])
    	return 0;

    if(gpAmp < 0.0005f)
    	gpAmp = 0.00f;

	if(rmsVoltTarget > rmsVoltNow)
	{
		targetFivePoint[4] = (gpAmp + ampNow) * 0.5f;
		targetFivePoint[3] = gpAmp * 1.04;
		targetFivePoint[2] = gpAmp * 0.98;
		targetFivePoint[1] = gpAmp * 1.01;
		targetFivePoint[0] = gpAmp;
	}
	else
	{
		targetFivePoint[4] = (gpAmp + ampNow) * 0.5f;
		targetFivePoint[3] = gpAmp * 0.96;
		targetFivePoint[2] = gpAmp * 1.02;
		targetFivePoint[1] = gpAmp * 0.99;
		targetFivePoint[0] = gpAmp;
	}

	return 1;
}

extern PID_ValueStr pidStr = {
		0.42, 	//Kp
		0.042,	//Ki
		0.0024,	//Kd
		0.0,//Ek
		0.0,//Ek_1
		0.0,//Ek_2
		0.0,//dacOutAmp
};
//0.1 1138
//188 1138
//#1
// 0.1    / 1060.82  @1060.82V = 0.0000943
//1060.82 / 219      @1060.82V = 4.8439
const float DAC_RMS_TO_TRANS_RMS = 0.0000943f;
const float TRANS_RMS_TO_ADC_RMS = 4.8439f;

//RMS:100:5：5000
//DPP:50：2.5：2500

/*
 * input : adc vpp after filtered,vlotHope output with RMS
 * output: dac output amp
 * fun	 : PID fun calc
 */
float PID_Caculate(float adcFltRMS,float voltHope)
{
	float Inck = 0.0;
	pidStr.Ek = voltHope - adcFltRMS * TRANS_RMS_TO_ADC_RMS * 1.0f;

	Inck = pidStr.KP*(pidStr.Ek - pidStr.Ek_1) + pidStr.KI*pidStr.Ek + pidStr.KD*(pidStr.Ek - 2*pidStr.Ek_1 + pidStr.Ek_2);

	pidStr.Ek_2 = pidStr.Ek_1;
	pidStr.Ek_1 = pidStr.Ek;

	pidStr.dacOutRMS += (Inck * DAC_RMS_TO_TRANS_RMS);

	if(pidStr.dacOutRMS < 0.0000)
		pidStr.dacOutRMS = 0.0000;

	if(pidStr.dacOutRMS > 0.5)
		pidStr.dacOutRMS = 0.5;

	return pidStr.dacOutRMS;
}


