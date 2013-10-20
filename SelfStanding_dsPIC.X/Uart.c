#include "Globals.h"
#include "Uart.h"
#include <stdio.h>

UCHAR UART_flagStart;
CHAR MessageBuffer[100];
UCHAR pBuffer;

UCHAR UART_Sending;

void UART_Send()
{
    pBuffer=0;
    UART_Sending=1;
    U1TXREG='\r';
    while (UART_Sending);
}

void __attribute__ ((interrupt, no_auto_psv)) _U1RXInterrupt(void)
{

    unsigned char a = U1RXREG;
    if (a=='S')
    {

        UART_flagStart=1;
        pBuffer=0;
        sprintf(MessageBuffer,"OK");
        U1TXREG = '\r';
    }
       _U1RXIF = 0;


}
void __attribute__ ((interrupt, no_auto_psv)) _U1TXInterrupt(void)
{
	_U1TXIF = 0;
        if (MessageBuffer[pBuffer] != 0x00)
        {
            U1TXREG = MessageBuffer[pBuffer++];
        }
        else
        {
            UART_Sending=0;
            pBuffer=0;
        }
        
}
