#include "Globals.h"
#include "Timer.h"

UINT TIMER_counter;

void __attribute__((__interrupt__, __auto_psv__)) _T1Interrupt(void)
{
    IFS0bits.T1IF = 0;
    //_LATB5 = !_LATB5;
    TIMER_counter--;
    if (!TIMER_counter)
    {
        
    }
}
