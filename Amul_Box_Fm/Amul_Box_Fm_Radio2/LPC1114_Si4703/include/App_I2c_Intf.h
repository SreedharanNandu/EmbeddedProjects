
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
extern unsigned char Write_Si_I2C(unsigned char *wptr,unsigned char wlen);
extern unsigned char Read_Si_I2C(unsigned char *rptr,unsigned char rlen);


#endif


