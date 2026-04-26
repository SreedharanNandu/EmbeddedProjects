#ifndef MEMORY_H
#define MEMORY_H

/*****************************************************************************
*                         System/Project Includes                            *
******************************************************************************/
#include "types.h"

#define IDLE_EE   0u
#define INIT_EE   1u
#define UPDATE_EE 2u

#define MAX_EE_SIZE (MAX_CH_SIZE + MAX_INDEX_SIZE + MAX_VOL_SIZE + MAX_CRC_SIZE) /* 10 channels (2byte each)  + 2byte index + 2byte vol + 2byte CRC */


#define Change_Mode_EE(x)   (ee_state = x)

extern volatile uint8_t ee_state;

/*****************************************************************************
*                             Macro Definitions                             
******************************************************************************/
void Process_Eeprom(void);
void EE_Read(unsigned char *data,unsigned char length);
void EE_Write(unsigned char *data,unsigned char length);
void Validate_EE_Read_Data(void);
void Validate_EE_Write_Data(void);
uint16_t CRC16_CCITT(uint8_t *data, uint16_t length) ;
void Update_EE_To_RAM(void);
void Update_RAM_To_EE(void);





#endif
