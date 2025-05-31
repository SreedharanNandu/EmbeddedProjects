
#ifndef UART_H
#define UART_H


#include "io_map.h"
#include "types.h"


#define MAX_STRING_SIZE      100
#define MAX_RX_SIZE          3
#define FOSC                 32000000
#define FCY                  (FOSC/2)
#define UART_TIME_OUT        0xFF   


#define iInit_UART()        Init_UART()
#define iDisable_UART()     do{U1MODE=0x0000;U1STA=0x0000;}while(0);
#define iEnable_UART()      (U1STAbits.UTXEN=1)   

typedef enum
{
 FAILED,
 UART_SUCCESS
}UART_Error;

extern volatile unsigned char UARTData[MAX_RX_SIZE];
extern volatile unsigned char rx_flag,rx_cnt;

extern void Init_UART(void);
extern UART_Error Send_UART_String(const char*,uint8_t);


#endif
//EOF
