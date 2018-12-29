/*
 * SHJK_DAC.h
 *
 *  Created on: 2018Äê3ÔÂ5ÈÕ
 *      Author: yusengo@163.com
 */

#ifndef SHJK_DAC_H_
#define SHJK_DAC_H_


#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

#define NSINE	200


extern const float SINEWAVE_DATA[NSINE];
extern volatile float GSINEBASE;
extern volatile float GAMPSINE;
extern volatile float GADCRMS;

extern volatile Uint16 PCdata;
extern volatile Uint16 ADCdata;

void SHJKSpiDacInit(void);
void SHJKDacOutput(Uint16 code);
void SHJKSineGen(float ampSine,Uint16 index);
Uint16 SHJK_DSP_FPGA_COM(Uint16 code);
Uint16 SHJKComVoltGet(void);
#endif /* SHJK_DAC_H_ */
