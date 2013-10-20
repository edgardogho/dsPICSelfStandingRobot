#include "Globals.h"
#include "i2c.h"
#include "Gyro.h"
#include "Uart.h"
#include <stdio.h>

UCHAR GYRO_State;
UCHAR GYRO_InitState;
UCHAR GYRO_Init;

UCHAR GYRO_X_L;
UCHAR GYRO_X_H;
UCHAR GYRO_Y_L;
UCHAR GYRO_Y_H;
UCHAR GYRO_Z_L;
UCHAR GYRO_Z_H;

int   GYRO_CAL_X,GYRO_CAL_Y,GYRO_CAL_Z;

int   GYRO_OFFSET_X,GYRO_OFFSET_Y,GYRO_OFFSET_Z;

int   GYRO_X,GYRO_Y,GYRO_Z;

double   DEG_X,DEG_Y,DEG_Z;

int  GYRO_Calibration_Step;

// Inspiration from  http://www.botched.co.uk/pic-tutorials/mpu6050-setup-data-aquisition/
void GYRO_GetValues()
{
    // This FSM runs when the GYRO was properly init
    switch(GYRO_InitState)
    {
        case 0: // Get X_L
            // Init the calibration mode
            GYRO_InitState++;
            I2C_Start(MPU6050_ADDRESS, MPU6050_RA_GYRO_XOUT_L, READ,0x00);
            break;
        case 1: // Get X_H
            GYRO_InitState++;
            GYRO_X_L=I2C_RX;
            I2C_Start(MPU6050_ADDRESS, MPU6050_RA_GYRO_XOUT_H, READ,0x00);
            break;
        case 2: // Get Y_L
            GYRO_InitState++;
            GYRO_X_H=I2C_RX;
            I2C_Start(MPU6050_ADDRESS, MPU6050_RA_GYRO_YOUT_L, READ,0x00);
            break;
        case 3: // Get Y_H
            GYRO_InitState++;
            GYRO_Y_L=I2C_RX;
            I2C_Start(MPU6050_ADDRESS, MPU6050_RA_GYRO_YOUT_H, READ,0x00);
            break;
        case 4: // Get Z_L
            GYRO_InitState++;
            GYRO_Y_H=I2C_RX;
            I2C_Start(MPU6050_ADDRESS, MPU6050_RA_GYRO_ZOUT_L, READ,0x00);
            break;
        case 5: // Get Z_H
            GYRO_InitState++;
            GYRO_Z_L=I2C_RX;
            I2C_Start(MPU6050_ADDRESS, MPU6050_RA_GYRO_ZOUT_H, READ,0x00);
            break;
        case 6: // Calculate
            GYRO_InitState=0;
            GYRO_Z_H=I2C_RX;

            GYRO_X =  (GYRO_X_H<<8)|GYRO_X_L;
            GYRO_Y =  (GYRO_Y_H<<8)|GYRO_Y_L;
            GYRO_Z =  (GYRO_Z_H<<8)|GYRO_Z_L;

            if (GYRO_Calibration_Step)
            {
                // Still calibrating
                GYRO_Calibration_Step--;
                GYRO_CAL_X+=GYRO_X;
                GYRO_CAL_Y+=GYRO_Y;
                GYRO_CAL_Z+=GYRO_Z;

                GYRO_OFFSET_X = GYRO_CAL_X/1000;
                GYRO_OFFSET_Y = GYRO_CAL_Y/1000;
                GYRO_OFFSET_Z = GYRO_CAL_Z/1000;
                sprintf(MessageBuffer,"Calibration...%d\n",GYRO_Calibration_Step);
                UART_Send(); // Blocking Call

                I2C_Start(MPU6050_ADDRESS, MPU6050_RA_GYRO_XOUT_L, READ,0x00);
            }
            else {
                // Gyro is calibrated
                 GYRO_X -=  GYRO_OFFSET_X;
                 GYRO_Y -=  GYRO_OFFSET_Y;
                 GYRO_Z -=  GYRO_OFFSET_Z;
                 DEG_X = (double)GYRO_X/ gyro_xsensitivity;
                 DEG_Y = (double)GYRO_Y/ gyro_xsensitivity;
                 DEG_Z = (double)GYRO_Z/ gyro_xsensitivity;
                 sprintf(MessageBuffer,"X= %f , Y= %f , Z= %f \n",DEG_X,DEG_Y,DEG_Z);
                UART_Send(); // Blocking Call
                sprintf(MessageBuffer,"X= %d , Y= %d , Z= %d \n",GYRO_X,GYRO_Y,GYRO_Z);
                UART_Send(); // Blocking Call
            }
            break;
    }
}

void GYRO_FSM()
{
    if (GYRO_Init)
    {
        GYRO_GetValues();

        return;
    }
    GYRO_InitState=0;
    switch ( GYRO_State)
    {
        case 0:
            // Device powered on
            // Check if the MPU6050 is there

            GYRO_State++;
            I2C_Start(MPU6050_ADDRESS, MPU6050_RA_WHO_AM_I, READ,0x00);
            break;
        case 1:
            // Check the result of the I2C operation
            if (!I2C_Error)
            {
                sprintf(MessageBuffer,"WhoAMI... 0x%X",I2C_RX);
                UART_Send(); // Blocking Call
                GYRO_State++;
                //Sets sample rate to 8000/1+7 = 1000Hz
                I2C_Start(MPU6050_ADDRESS,MPU6050_RA_SMPLRT_DIV,WRITE,0x07);
            }
            else
            {
                sprintf(MessageBuffer,"WhoAMI... ERROR");
                UART_Send(); // Blocking Call
                GYRO_State=0;
            }
            break;
        case 2:
            // Check the result of the I2C operation
            if (!I2C_Error)
            {
                sprintf(MessageBuffer,"MPU6050_RA_SMPLRT_DIV... OK");
                UART_Send(); // Blocking Call
                GYRO_State++;
                //Disable FSync, 256Hz DLPF
                I2C_Start(MPU6050_ADDRESS,MPU6050_RA_CONFIG,WRITE,0x04);
            }
            else
            {
                sprintf(MessageBuffer,"MPU6050_RA_SMPLRT_DIV... ERROR");
                UART_Send(); // Blocking Call
                GYRO_State=0;
            }
            break;
        case 3:
            // Check the result of the I2C operation
            if (!I2C_Error)
            {
                sprintf(MessageBuffer,"MPU6050_RA_CONFIG... OK");
                UART_Send(); // Blocking Call
                GYRO_State++;
                I2C_Start(MPU6050_ADDRESS,MPU6050_RA_GYRO_CONFIG,WRITE,0x18);
            }
            else
            {
                sprintf(MessageBuffer,"MPU6050_RA_CONFIG... ERROR");
                UART_Send(); // Blocking Call
                GYRO_State=0;
            }
            break;
        case 4:
            // Check the result of the I2C operation
            if (!I2C_Error)
            {
                sprintf(MessageBuffer,"MPU6050_RA_GYRO_CONFIG... OK");
                UART_Send(); // Blocking Call
                GYRO_State++;
                I2C_Start(MPU6050_ADDRESS,MPU6050_RA_ACCEL_CONFIG,WRITE,0x00);
            }
            else
            {
                sprintf(MessageBuffer,"MPU6050_RA_GYRO_CONFIG... ERROR");
                UART_Send(); // Blocking Call
                GYRO_State=0;
            }
            break;
        case 5:
            // Check the result of the I2C operation
            if (!I2C_Error)
            {
                sprintf(MessageBuffer,"MPU6050_RA_ACCEL_CONFIG... OK");
                UART_Send(); // Blocking Call
                GYRO_State++;
                I2C_Start(MPU6050_ADDRESS,MPU6050_RA_FF_THR,WRITE,0x00);
            }
            else
            {
                sprintf(MessageBuffer,"MPU6050_RA_ACCEL_CONFIG... ERROR");
                UART_Send(); // Blocking Call
                GYRO_State=0;
            }
            break;
        case 6:
            // Check the result of the I2C operation
            if (!I2C_Error)
            {
                sprintf(MessageBuffer,"MPU6050_RA_FF_THR... OK");
                UART_Send(); // Blocking Call
                GYRO_State++;
                I2C_Start(MPU6050_ADDRESS,MPU6050_RA_FF_DUR,WRITE,0x00);
            }
            else
            {
                sprintf(MessageBuffer,"MPU6050_RA_FF_THR... ERROR");
                UART_Send(); // Blocking Call
                GYRO_State=0;
            }
            break;
        case 7:
            // Check the result of the I2C operation
            if (!I2C_Error)
            {
                sprintf(MessageBuffer,"MPU6050_RA_FF_DUR... OK");
                UART_Send(); // Blocking Call
                GYRO_State++;
                I2C_Start(MPU6050_ADDRESS,MPU6050_RA_MOT_THR,WRITE,0x00);
            }
            else
            {
                sprintf(MessageBuffer,"MPU6050_RA_FF_DUR... ERROR");
                UART_Send(); // Blocking Call
                GYRO_State=0;
            }
            break;
        case 8:
            // Check the result of the I2C operation
            if (!I2C_Error)
            {
                sprintf(MessageBuffer,"MPU6050_RA_MOT_THR... OK");
                UART_Send(); // Blocking Call
                GYRO_State++;
                I2C_Start(MPU6050_ADDRESS,MPU6050_RA_FIFO_EN,WRITE,0x00);
            }
            else
            {
                sprintf(MessageBuffer,"MPU6050_RA_MOT_THR... ERROR");
                UART_Send(); // Blocking Call
                GYRO_State=0;
            }
            break;
        case 9:
            // Check the result of the I2C operation
            if (!I2C_Error)
            {
                sprintf(MessageBuffer,"MPU6050_RA_FIFO_EN... OK");
                UART_Send(); // Blocking Call
                GYRO_State++;
                I2C_Start(MPU6050_ADDRESS,MPU6050_RA_SIGNAL_PATH_RESET,WRITE,0x00);
            }
            else
            {
                sprintf(MessageBuffer,"MPU6050_RA_FIFO_EN... ERROR");
                UART_Send(); // Blocking Call
                GYRO_State=0;
            }
            break;
        case 10:
            // Check the result of the I2C operation
            if (!I2C_Error)
            {
                sprintf(MessageBuffer,"MPU6050_RA_SIGNAL_PATH_RESET... OK");
                UART_Send(); // Blocking Call
                GYRO_State++;
                I2C_Start(MPU6050_ADDRESS,MPU6050_RA_USER_CTRL,WRITE,0x00);
            }
            else
            {
                sprintf(MessageBuffer,"MPU6050_RA_SIGNAL_PATH_RESET... ERROR");
                UART_Send(); // Blocking Call
                GYRO_State=0;
            }
            break;
        case 11:
            // Check the result of the I2C operation
            if (!I2C_Error)
            {
                sprintf(MessageBuffer,"MPU6050_RA_USER_CTRL... OK");
                UART_Send(); // Blocking Call
                GYRO_State++;
                I2C_Start(MPU6050_ADDRESS,MPU6050_RA_PWR_MGMT_1,WRITE,0x00);
            }
            else
            {
                sprintf(MessageBuffer,"MPU6050_RA_USER_CTRL... ERROR");
                UART_Send(); // Blocking Call
                GYRO_State=0;
            }
            break;
        case 12:
            // Check the result of the I2C operation
            if (!I2C_Error)
            {
                sprintf(MessageBuffer,"MPU6050_RA_PWR_MGMT_1... OK");
                UART_Send(); // Blocking Call
                GYRO_State++;
                I2C_Start(MPU6050_ADDRESS,MPU6050_RA_PWR_MGMT_2,WRITE,0x00);
            }
            else
            {
                sprintf(MessageBuffer,"MPU6050_RA_PWR_MGMT_1... ERROR");
                UART_Send(); // Blocking Call
                GYRO_State=0;
            }
            break;
        case 13:
            // Check the result of the I2C operation
            if (!I2C_Error)
            {
                sprintf(MessageBuffer,"MPU6050_RA_PWR_MGMT_2... OK");
                UART_Send(); // Blocking Call
                GYRO_State++;
                I2C_Start(MPU6050_ADDRESS,MPU6050_RA_FIFO_R_W,WRITE,0x00);
            }
            else
            {
                sprintf(MessageBuffer,"MPU6050_RA_PWR_MGMT_2... ERROR");
                UART_Send(); // Blocking Call
                GYRO_State=0;
            }
            break;
        case 14:
            // Check the result of the I2C operation
            if (!I2C_Error)
            {
                sprintf(MessageBuffer,"MPU6050_RA_FIFO_R_W... OK");
                UART_Send(); // Blocking Call
                GYRO_State=0;
                GYRO_Init=1;
                GYRO_Calibration_Step=1000;
                GYRO_GetValues();
                //I2C_Start(MPU6050_ADDRESS,MPU6050_RA_FIFO_R_W,WRITE,0x00);
            }
            else
            {
                sprintf(MessageBuffer,"MPU6050_RA_FIFO_R_W... ERROR");
                UART_Send(); // Blocking Call
                GYRO_State=0;
            }
            break;


    }
}