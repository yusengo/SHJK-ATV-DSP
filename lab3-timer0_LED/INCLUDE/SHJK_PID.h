/*
 * SHJK_PID.h
 *
 *  Created on: 2018年3月5日
 *      Author: yusengo@163.com
 */

#ifndef SHJK_PID_H_
#define SHJK_PID_H_

#include "DSP2833x_Device.h"     // DSP2833x 头文件
typedef struct PID_Value
{
	float KP;
	float KI;
	float KD;
	float Ek;
	float Ek_1;
	float Ek_2;
	float dacOutRMS;
}PID_ValueStr;

extern PID_ValueStr pidStr;
extern volatile float targetFivePoint[5];

float PID_Caculate(float adcDivAmp,float voltHope);
Uint16 TargetFivePointCalc(Uint16 rmsVoltTarget,Uint16 rmsVoltNow,float ampNow);

#endif /* SHJK_PID_H_ */
