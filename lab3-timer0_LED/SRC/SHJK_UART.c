/*
 * SHJK_UART.c
 *
 *  Created on: 2018Äê3ÔÂ20ÈÕ
 *      Author: guoer
 */

#include"SHJK_UART.h"



void SHJKUartCInit(void)
{
 	ScicRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
                                   // No parity,8 char bits,
                                   // async mode, idle-line protocol
	ScicRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
                                   // Disable RX ERR, SLEEP, TXWAKE
	ScicRegs.SCICTL2.all =0x0003;
	ScicRegs.SCICTL2.bit.TXINTENA = 1;
	ScicRegs.SCICTL2.bit.RXBKINTENA =1;
	#if (CPU_FRQ_150MHZ)
	      ScicRegs.SCIHBAUD    =0x0001;  // 9600 baud @LSPCLK = 37.5MHz.
	      ScicRegs.SCILBAUD    =0x00E7;
	#endif
	#if (CPU_FRQ_100MHZ)
      ScicRegs.SCIHBAUD    =0x0001;  // 9600 baud @LSPCLK = 20MHz.
      ScicRegs.SCILBAUD    =0x0044;
	#endif
	ScicRegs.SCICTL1.all =0x0023;  // Relinquish SCI from Reset

}

// Transmit a character from the SCI
void SHJKUartCSendChar(int a)
{
    while (ScicRegs.SCICTL2.bit.TXRDY == 0) {}
    ScicRegs.SCITXBUF = a;
}

void SHJKUartCSendString(char * msg)
{
    int i;
    i = 0;
    while(msg[i] != '\0')
    {
    	SHJKUartCSendChar(msg[i]);
        i++;
    }
}

// Initalize the SCI FIFO
void SHJKUartCFIFOInit()
{
	ScicRegs.SCIFFTX.all=0x8000;
}
