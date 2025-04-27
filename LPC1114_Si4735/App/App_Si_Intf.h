
#ifndef APP_SI_INTF_H
#define APP_SI_INTF_H

#include "type.h"
#include "i2c.h"

#define READ_DATA    1u
#define WRITE_DATA   2u


unsigned char Write_Read_Si4735_I2C(
                           unsigned char chip_address,
                           unsigned char *wptr,
                           unsigned char wlen,
                           unsigned char *rptr,
                           unsigned char rlen);

void Update_Nvm_Radio(u8 state);
void Validate_Nvm_Radio(void);


#endif

/*-----------------------------------------------------------------------------
 * Version      Description                       Name       Date
 *
 *  1.0         Created initial version           Nandu      02-04-2025
 *---------------------------------------------------------------------------*/

