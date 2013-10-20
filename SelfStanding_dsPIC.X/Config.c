#include "Globals.h"
#include "Config.h"

void CONFIG_Init() {
    //
    // Setup Oscillator
    //
    CLKDIVbits.PLLPRE = 0;
    PLLFBD = 41;
    CLKDIVbits.PLLPOST = 0;
    while (!OSCCONbits.LOCK);

    //
    // At this point the PLL is ready
    //

    //
    // Setup I/O pins
    //
    _TRISB5 = 0;
    _LATB5 = 1;

    // Disable A/D on pins used by Digital Peripherals
    _PCFG5 = 1;
    _PCFG4 = 1;

    // Pin 6  --> Uart TX  RB2 RP2
    _TRISB2 = OUTPUT;
    _RP2R = 3; // RP2 to Uart1 TX

    // Pin 7  <-- Uart RX  RB3 RP3
    _TRISB3 = INPUT;
    _U1RXR = 3; // Uart1 RX on RP3


    //
    // Setup UART 1 (http://www.microchip.com/forums/fb.ashx?m=740865)
    //
    U1MODEbits.UARTEN = 0;      // Disable First (Config and Enable later)
    U1MODEbits.USIDL = 0;	// Bit13 Continue in Idle
    U1MODEbits.IREN = 0;	// Bit12 No IR translation
    U1MODEbits.RTSMD = 0;	// Bit11 Simplex Mode
    U1MODEbits.UEN = 0;		// Bits8,9 TX,RX enabled, CTS,RTS not
    U1MODEbits.WAKE = 0;	// Bit7 No Wake up (since we don't sleep here)
    U1MODEbits.LPBACK = 0;	// Bit6 No Loop Back
    U1MODEbits.ABAUD = 0;	// Bit5 No Autobaud (would require sending '55')
    U1MODEbits.URXINV = 0;	// Bit4 IdleState = 1  (for dsPIC)
    U1MODEbits.BRGH = 1;	// High Precision Baud Rate Generator
    U1MODEbits.PDSEL = 0;	// Bits1,2 8bit, No Parity
    U1MODEbits.STSEL = 0;	// Bit0 One Stop Bit
    U1BRG = 86;
    U1STAbits.UTXISEL1 = 0;	//Bit15 Int when Char is transferred (1/2 config!)
    U1STAbits.UTXINV = 0;	//Bit14 N/A, IRDA config
    U1STAbits.UTXISEL0 = 0;	//Bit13 Other half of Bit15
    U1STAbits.UTXBRK = 0;	//Bit11 Disabled
    U1STAbits.UTXEN = 0;	//Bit10 TX pins controlled by periph
    U1STAbits.URXISEL = 0;	//Bits6,7 Int. on character recieved
    U1STAbits.ADDEN = 0;	//Bit5 Address Detect Disabled
    _U1TXIF = 0;                // Clear TX Interrupt Flag
    _U1TXIE = 1;                // Enable TX interrupts
    _U1RXIF = 0;                // Clear RX Interrupt Flag
    _U1RXIE = 1;                // Enable RX Interrups

    U1MODEbits.UARTEN = 1;	// Enable UART
    IFS0bits.U1RXIF = 0;        // Clear Receive Flag
    U1STAbits.UTXEN = 1;


    //
    // Setup Timer 1 to interrupt every 1ms
    //
    _TCS=0;    // Clock Source Fcy
    _TCKPS=3;  // Prescaler OFF (1:256)
    _TGATE=0;
    TMR1=0;    // Reset timer Counter
               // 40MHz Fcy --> 40000000/256 = 156250 pulses per second
               //               So to generate 1ms divide by 1000
    PR1=156;
    _TON=1;    // Start Timer 1
    _T1IE=1;    // Timer 1 interrupt Enable


    //
    // Setup I2C module
    //



	I2C1CONbits.A10M=0;
	I2C1CONbits.SCLREL=1;
	I2C1BRG=300;

	I2C1ADD=0;
	I2C1MSK=0;

	I2C1CONbits.I2CEN=1;
	IEC1bits.MI2C1IE = 1;
  	IFS1bits.MI2C1IF = 0;
    


}


void CONFIG_InitGlobals()
{
    UART_flagStart=0;
    UART_Sending=0;
    I2C_flagGyro=0;
    I2C_State=0;
    GYRO_State=0;
    GYRO_Init=0;
    TIMER_counter=1000;

}


