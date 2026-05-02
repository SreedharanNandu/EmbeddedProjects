/*filename:-App_I2C_Intf.h*/
#ifndef APP_I2C_INTF_H
#define APP_I2C_INTF_H

#include "type_sys.h"
#include "i2c.h"

/*****************************************************************************
*                             Macro Definition                               *
******************************************************************************/



/*****************************************************************************
*                             Function Declaration                             
******************************************************************************/
extern unsigned char Write_I2C(unsigned char chip_address,unsigned char *wptr,unsigned char wlen);
extern unsigned char Read_I2C(unsigned char chip_address,unsigned char *rptr,unsigned char rlen);


#endif


