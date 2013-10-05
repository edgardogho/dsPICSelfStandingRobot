#include "Globals.h"

_FOSCSEL(FNOSC_FRC );
_FOSC( OSCIOFNC_ON &POSCMD_NONE );//FNOSC_FRCPLL ); // Internal Fast RC (FRC) w/ PLL
//_FOSC(OSCIOFNC_OFF & POSCMD_NONE);
_FWDT(FWDTEN_OFF);   // Watchdog Enable OFF


int main()
{
    while(1);

    return 0;
}
