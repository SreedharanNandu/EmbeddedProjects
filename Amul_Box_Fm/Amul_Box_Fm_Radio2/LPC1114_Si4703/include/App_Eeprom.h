#ifndef APP_EEPROM_H
#define APP_EEPROM_H

/*****************************************************************************
*                         System/Project Includes                            *
******************************************************************************/
#include "type_sys.h"

#define IDLE_EE   0u
#define INIT_EE   1u
#define UPDATE_EE 2u

#define MAX_EE_SIZE ((MAX_CH_SIZE) + 2u + 2u) /* 10 channels (2byte each)  + 2byte index + 2byte CRC */

#define PAGE_EE(x)   (x*128u) /* page is 128 bytes */


extern uint8_t ee_state;

/*****************************************************************************
*                             Macro Definitions                             
******************************************************************************/
void Process_Eeprom(void);
void EE_Read(unsigned short int address,unsigned char *data,unsigned char length);
void EE_Write(unsigned short int address,unsigned char *data,unsigned char length);
void Change_Mode_EE(unsigned char state);
void Validate_EE_Read_Data(void);
void Validate_EE_Write_Data(void);
uint16_t CRC16_CCITT(uint8_t *data, uint16_t length) ;



#endif
