#include "Globals.h"
#include "i2c.h"

 UCHAR I2C_flagGyro;
 UCHAR I2C_State;
 UCHAR I2C_ReadNotWrite;
 UCHAR I2C_Address;
 UCHAR I2C_Device;
 UCHAR I2C_RX;
 UCHAR I2C_TX;
 UCHAR I2C_Error;


 void I2C_Start(UCHAR Device, UCHAR Address, UCHAR ReadNotWrite,UCHAR TX){

     I2C_State = 0;
     I2C_ReadNotWrite = ReadNotWrite;
     I2C_Device = Device;
     I2C_Address = Address;
     I2C_TX=TX;
     I2C1CONbits.SEN=1;

 }

 void I2C_FSM()
 {
     // This state machine is called from Interrupt
     // So avoid making UART buffer writes
     switch (I2C_State)
     {
         case 0: // Sent Start Bit, now send device ID
             I2C1TRN=I2C_Device&0xFE; // Put a 0 at LSb for Write
             break;
         case 1: // Send the address (1 byte)
             I2C1TRN=I2C_Address;
             break;
         case 2: // Check for ACK
             if (I2C1STATbits.ACKSTAT) // then NAK received
             {
                 I2C_Error = 1; // Error at finding device
                 I2C_flagGyro = 1; // Signal error
                 return;
             }
             // An ACK for the device
             if (I2C_ReadNotWrite) // If reading repeat start
                I2C1CONbits.RSEN=1;
             else
                 // For writing the sequence is different, so bump
                 // to the state 9
                 I2C_State = 9; // Bump State
                 I2C1TRN = I2C_TX; // Send Data
             break;
         case 3: // Read operation in progress, send Device ID
             I2C1TRN=I2C_Device|0x01; //Signal Read Operation
             break;
         case 4: // Check if the device allows Read
             if (I2C1STATbits.ACKSTAT)
             {
                 // Error, device sent NAK
                 I2C_Error = 2; // Error at finding device
                 I2C_flagGyro = 1; // Signal error
                 return;
             }
             // if ACK
             I2C1CONbits.RCEN=1; // Receive Enable
             break;
         case 5:
             // Get Byte
             I2C_RX = I2C1RCV;
             I2C1CONbits.ACKDT=1; // Signal NACK to stop
             I2C1CONbits.ACKEN=1; // Enable ACK sequence
             break;
         case 6:
             I2C1CONbits.PEN=1;// Generate Stop
             break;
         case 7:
             I2C_Error = 0; // No Error
             I2C_flagGyro = 1; // Signal Read
             break;

         // Write part of the FSM
         case 10: // Check ACK of data Write
             if (I2C1STATbits.ACKSTAT)
             {
                 // Error, device sent NAK
                 I2C_Error = 3; // Error at finding device
                 I2C_flagGyro = 1; // Signal error
                 return;
             }
             I2C1CONbits.PEN=1; // Stop TX
             break;
         case 11:
             I2C_Error = 0; // No Error
             I2C_flagGyro = 1; // Signal Write
             break;


     }
     I2C_State++;



 }


void __attribute__((__interrupt__,__auto_psv__)) _MI2C1Interrupt(void)
{

    IFS1bits.MI2C1IF = 0;
    I2C_FSM();

}
