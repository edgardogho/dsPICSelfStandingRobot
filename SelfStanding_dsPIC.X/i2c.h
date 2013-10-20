/*
 * File:   I2c.h
 * Author: edgardogho
 *
 * Created on October 6, 2013, 3:50 PM
 */

#ifndef I2c_H
#define	I2c_H

#ifdef	__cplusplus
extern "C" {
#endif

    void I2C_Start(UCHAR Device, UCHAR Address, UCHAR ReadNotWrite, UCHAR TX);


#ifdef	__cplusplus
}
#endif

#endif	/* I2c_H */


