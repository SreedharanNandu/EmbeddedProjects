#ifndef I2C_H /*I2C_H  */
#define I2C_H
#include "hardware.h"
#include "types.h"


#define MAX_EXIT_LOOP_TH     250u

#define MastergetcI2C1   MasterReadI2C1

/* Macros to  Enable/Disable interrupts and set Interrupt priority of SI2C1 module*/
#define EnableIntSI2C1                    asm("BSET IEC3,#1")
#define DisableIntSI2C1                   asm("BCLR IEC3,#1")
#define SetPriorityIntSI2C1(priority)     (IPC12bits.SI2C1IP = priority)

/* Macros to  Enable/Disable interrupts and set Interrupt priority of MI2C1 module*/
#define EnableIntMI2C1                    asm("BSET IEC3,#2")
#define DisableIntMI2C1                   asm("BCLR IEC3,#2")
#define SetPriorityIntMI2C1(priority)     (IPC12bits.MI2C1IP = priority)



/* I2C1 module Converter Function Prototypes */
void AckI2C1(void) __attribute__ ((section (".libperi")));
void CloseI2C1(void) __attribute__ ((section (".libperi")));
void ConfigIntI2C1(unsigned int) __attribute__ ((section (".libperi")));
unsigned char MasterGetsI2C1(unsigned int length, unsigned char * rdptr, unsigned int I2C1_data_wait) __attribute__ ((section (".libperi")));
void IdleI2C1(void) __attribute__ ((section (".libperi")));
void NotAckI2C1(void) __attribute__ ((section (".libperi")));
void OpenI2C1(unsigned int,unsigned int) __attribute__ ((section (".libperi")));
unsigned char MasterReadI2C1(void) __attribute__ ((section (".libperi")));
unsigned char MasterWriteI2C1(unsigned char) __attribute__ ((section (".libperi")));
void RestartI2C1(void) __attribute__ ((section (".libperi")));
void StartI2C1(void) __attribute__ ((section (".libperi")));
void StopI2C1(void) __attribute__ ((section (".libperi")));
unsigned char SendI2C1(unsigned char slave_id,unsigned char *ptr,unsigned char num_of_bytes);
unsigned char ReadI2C1(unsigned char slave_id,unsigned char *ptr,unsigned char num_of_bytes);
unsigned char exitLoop(unsigned char id);
void Init_I2C(void);
void DeInit_I2C(void);

#endif /*I2C_H  */
