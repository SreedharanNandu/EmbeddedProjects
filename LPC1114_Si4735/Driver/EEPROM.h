/*----------------------------------------------------------------------------
 * Name:    EEPROM.h
 *----------------------------------------------------------------------------*/

#ifndef EEPROM_H
#define EEPROM_H

#include "type.h"
#include "i2c.h"

#define NVM_TIMEOUT 300u

//#define EEPROM_24C256 1
#define EEPROM_24C02 1
//#define EEPROM_M24C64

#define CHIP_ADDRESS 0xA0 

#define EEPROM_ADDRESS   0U
#if EEPROM_24C02
   #define PAGE_SIZE 8
#else
   #define PAGE_SIZE 32
#endif


#define NVM_THR                 200u
#define Get_Nvm_Tmr()          (nvm_tmr)
#define Is_Nvm_Tmr_Expired()   (nvm_tmr == 0)
#define Dec_Nvm_Tmr()          (nvm_tmr--)
#define Reload_Nvm_Tmr()       (nvm_tmr = NVM_THR)


extern unsigned int nvm_tmr;


extern unsigned char Read_I2C_EEPROM( unsigned char chip_address,
                                unsigned int address ,
                                unsigned char *ptr,
                                unsigned char len);
extern unsigned char Write_I2C_EEPROM(unsigned char chip_address,
                                unsigned int address ,
                                unsigned char *ptr,
                                unsigned char len) ;

extern void Read_EEPROM(unsigned long address,
                 unsigned char *ptr,
                 unsigned int len);

extern unsigned char Write_EEPROM(unsigned long address,
                  unsigned char *ptr,
                  unsigned int len);


#endif

/*-----------------------------------------------------------------------------
 * Version      Description                       Name       Date
 *
 *  1.0         Created initial version           Nandu      2-10-2025
 *---------------------------------------------------------------------------*/

