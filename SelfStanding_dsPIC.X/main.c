#include "Globals.h"
#include "Config.h"

_FOSCSEL(FNOSC_FRCPLL); // Internal RC oscillator, use PLL
_FOSC( OSCIOFNC_ON &POSCMD_NONE ); // No Clock Output.
_FWDT(FWDTEN_OFF);   // Watchdog Enable OFF


int main()
{
    // Setup Microcontroller
    CONFIG_Init();
    U1TXREG = 'U';
    while(1){
        _LATB5 = !_LATB5;
    }

    return 0;
}
