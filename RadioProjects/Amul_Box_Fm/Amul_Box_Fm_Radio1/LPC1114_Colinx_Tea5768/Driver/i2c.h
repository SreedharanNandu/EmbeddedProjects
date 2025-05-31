/*****************************************************************************
 *   i2c.h:  Header file for NXP LPC Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.19  ver 1.00    Preliminary version, first Release
 *
******************************************************************************/
#ifndef __I2C_H 
#define __I2C_H

/* If I2C SEEPROM is tested, make sure FAST_MODE_PLUS is 0.
For board to board test, this flag can be turned on. */

#define FAST_MODE_PLUS   0

#define I2C_BUFSIZE         64uL
#define MAX_TIMEOUT      0x00FFFFFF

#define I2CMASTER      0x01
#define I2CSLAVE      0x02

#define PCF8594_ADDR   0xA0
#define READ_WRITE      0x01

#define RD_BIT         0x01

#define I2C_IDLE             0
#define I2C_STARTED          1
#define I2C_RESTARTED        2
#define I2C_REPEATED_START   3
#define DATA_ACK             4
#define DATA_NACK            5

#define I2CSTATE_IDLE       0x000
#define I2CSTATE_PENDING    0x001
#define I2CSTATE_ACK        0x101
#define I2CSTATE_NACK       0x102
#define I2CSTATE_SLA_NACK   0x103
#define I2CSTATE_ARB_LOSS   0x104

#define  I2C_FLAG_AA    (1<<2)
#define  I2C_FLAG_SI    (1<<3)
#define  I2C_FLAG_STO   (1<<4)
#define  I2C_FLAG_STA   (1<<5)
#define  I2C_FLAG_I2EN  (1<<6)

#define I2CONSET_I2EN      0x00000040  /* I2C Control Set Register */
#define I2CONSET_AA         0x00000004
#define I2CONSET_SI         0x00000008
#define I2CONSET_STO      0x00000010
#define I2CONSET_STA      0x00000020

#define I2CONCLR_AAC      0x00000004  /* I2C Control clear Register */
#define I2CONCLR_SIC      0x00000008
#define I2CONCLR_STAC      0x00000020
#define I2CONCLR_I2ENC      0x00000040

#define I2DAT_I2C         0x00000000  /* I2C Data Reg */
#define I2ADR_I2C         0x00000000  /* I2C Slave Address Reg */

#define I2SCLH_SCLH         0x000000F0  /* I2C SCL Duty Cycle High Reg */
#define I2SCLL_SCLL         0x000000F0  /* I2C SCL Duty Cycle Low Reg */

#define I2SCLH_HS_SCLH      0x00000018  /* Fast Plus I2C SCL Duty Cycle High Reg */
#define I2SCLL_HS_SCLL      0x00000018  /* Fast Plus I2C SCL Duty Cycle Low Reg */



extern void I2CInit(void);
extern void I2CStart( void );
extern void I2CStop( void );
extern void Send_I2C( unsigned char data );

#endif /* end __I2C_H */
/****************************************************************************
**                            End Of File
*****************************************************************************/
