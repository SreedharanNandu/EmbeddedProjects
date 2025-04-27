#ifndef ISR_H
#define ISR_H

#include "hardware.h"

#define SET     1
#define CLEAR   0

#define Clear_All_Interrupt()         \
do                                    \
{                                     \
   IEC0=0X0000;                       \
   IEC1=0X0000;                       \
   IEC2=0X0000;                       \
   IEC3=0X0000;                       \
   IEC4=0X0000;                       \
   IEC5=0X0000;                       \
}while(0);

#endif

//EOF
