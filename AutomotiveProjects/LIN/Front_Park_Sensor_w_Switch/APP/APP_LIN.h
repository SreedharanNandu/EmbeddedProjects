//filename:- App_Lin.h

#ifndef APP_LIN_H
#define APP_LIN_H

#define LIN_NEXT_MSG_PERIODIC_CNT  3u  //i.e 10ms  for 5ms x 3times


/***********************************************************************************
Function Prototypes
***********************************************************************************/

extern void App_Update_LIN_Data(void);
extern void App_Notify_Frame_Received_USART1(unsigned char id_index);
extern void App_Notify_Frame_Transmitted_USART0(unsigned char id_index);



#endif 