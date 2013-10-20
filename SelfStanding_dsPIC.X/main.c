#include "Globals.h"
#include "Config.h"
#include "i2c.h"
#include "Gyro.h"


_FOSCSEL(FNOSC_FRCPLL); // Internal RC oscillator, use PLL
_FOSC( OSCIOFNC_ON &POSCMD_NONE ); // No Clock Output.
_FWDT(FWDTEN_OFF);   // Watchdog Enable OFF


int main()
{
    // Setup Microcontroller
    CONFIG_Init();
    CONFIG_InitGlobals();
    while(1){
        if (UART_flagStart){
            UART_flagStart=0;
            GYRO_State=0;
            GYRO_FSM();
        }
        
        if (I2C_flagGyro)
        {
            I2C_flagGyro = 0;
            GYRO_FSM();
        }



    }

    return 0;
}



