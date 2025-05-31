#ifndef I2C_INTF_H
#define I2C_INTF_H

#include "io_map.h"
#include "types.h"
#include "int.h"






extern uint8_t Write_I2C(uint8_t *data);
extern uint8_t Read_I2C(uint8_t *data);



#endif 

//EOF
