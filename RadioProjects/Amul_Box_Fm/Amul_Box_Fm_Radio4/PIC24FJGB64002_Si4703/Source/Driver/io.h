#ifndef IO_H
#define IO_H

#include "io_map.h"




#define Set_All_Port_LOW()      \
do                              \
{                               \
   TRISA=0xFFFF;                \
   TRISB=0xFFF;                 \
}while(0)

#define Set_IO(x)         (x=0)
#define Clear_IO(y)       (y=1)
#define Toggle_IO(z)      (z=~z)


extern void Init_Ports(void);

#endif
