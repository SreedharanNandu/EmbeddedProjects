#ifndef _I2C_H_
#define _I2C_H_

#define I2C_WHILE_TIMEOUT 30000uL


#define FAST_MODE_PLUS  0

#define I2C_BUFSIZE      60

#define I2CMASTER    0x01
#define I2CSLAVE    0x02

#define PCF8594_ADDR  0xA0
#define READ_WRITE    0x01

#define RD_BIT      0x01

#define I2C_IDLE      0
#define I2C_STARTED      1
#define I2C_RESTARTED    2
#define I2C_REPEATED_START  3
#define DATA_ACK      4
#define DATA_NACK      5

#define I2CONSET_I2EN    0x00000040  /* I2C Control Set Register */
#define I2CONSET_AA      0x00000004
#define I2CONSET_SI      0x00000008
#define I2CONSET_STO    0x00000010
#define I2CONSET_STA    0x00000020

#define I2CONCLR_AAC    0x00000004  /* I2C Control clear Register */
#define I2CONCLR_SIC    0x00000008
#define I2CONCLR_STAC    0x00000020
#define I2CONCLR_I2ENC    0x00000040

#define I2DAT_I2C      0x00000000  /* I2C Data Reg */
#define I2ADR_I2C      0x00000000  /* I2C Slave Address Reg */
#define I2SCLH_SCLH      0x00000180  /* I2C SCL Duty Cycle High Reg */
#define I2SCLL_SCLL      0x00000180  /* I2C SCL Duty Cycle Low Reg */
#define I2SCLH_HS_SCLH    0x00000030  /* Fast Plus I2C SCL Duty Cycle High Reg */
#define I2SCLL_HS_SCLL    0x00000030  /* Fast Plus I2C SCL Duty Cycle Low Reg */

#define  I2C_FLAG_AA    (1<<2)
#define  I2C_FLAG_SI    (1<<3)
#define  I2C_FLAG_STO   (1<<4)
#define  I2C_FLAG_STA   (1<<5)
#define  I2C_FLAG_I2EN  (1<<6)



extern void Init_I2C( void );
extern void Start_I2C( void );
extern void Stop_I2C( void );
extern void Send_I2C( unsigned char );



#endif


