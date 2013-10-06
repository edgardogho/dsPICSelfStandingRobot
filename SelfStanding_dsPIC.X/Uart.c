#include "Globals.h"

void __attribute__ ((interrupt, no_auto_psv)) _U1RXInterrupt(void)
{


       _U1RXIF = 0;


}
void __attribute__ ((interrupt, no_auto_psv)) _U1TXInterrupt(void)
{
	_U1TXIF = 0;
        U1TXREG = 'U';
}
