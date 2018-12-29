/*
 * SHJK_UART.h
 *
 *  Created on: 2018Äê3ÔÂ20ÈÕ
 *      Author: guoer
 */

#ifndef SHJK_UART_H_
#define SHJK_UART_H_

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

void SHJKUartCInit(void);
// Transmit a character from the SCI
void SHJKUartCSendChar(int a);
void SHJKUartCSendString(char * msg);
// Initalize the SCI FIFO
void SHJKUartCFIFOInit(void);

#endif /* SHJK_UART_H_ */
