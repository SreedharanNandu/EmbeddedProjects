#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdio.h>
#include "LPC11xx.h" /* LPC11xx definitions */
#include "gpio.h"
#include "type.h"
#include "timer16.h"


#ifndef LEFT
#define LEFT 0
#endif

#ifndef RIGHT
#define RIGHT 1
#endif


#define TMR_PERIODIC_RATE_MS 1u
#define I2C_WHILE_TIMEOUT 30000uL

#define SI_READY  (1<<0u)
#define LCD_READY (1<<1u)
typedef enum
{
   PWR_OFF,
   PWR_ON

}power_mode_T;

extern power_mode_T Power_Mode;

typedef enum
{
   IDLE,
   INIT,
   UPDATE
}state_t;

typedef enum
{
   SERIAL_STATE,
   ALL_STATE
}main_state_T;





typedef enum
{
   TMR_IDLE,
   TMR_ACTIVE,
   TMR_EXPIRED
          
}tmr_status_t;

typedef enum
{  
   SI_TMR,
   TMR_MAX_CNT
}tmr_id_t;  

typedef struct
{
   tmr_status_t tmr_status;
   unsigned long tmr_count;

}tmr_data_t;

extern state_t serial_state;
extern u8 sleep_check;


extern void Process_Timer(void);
extern void Start_Timer(tmr_id_t id , unsigned long time);
extern tmr_status_t  Get_Timer_Status(tmr_id_t id);
extern void  Set_Timer_Status(tmr_id_t  id , tmr_status_t  status);
extern void Set_Timer_Count(tmr_id_t id ,unsigned long count);
extern void Reset_All_Timers(void);
extern void Stop_Timer(tmr_id_t id);
extern void Init_Shutdown(void);
extern void Sleep_Check(void);






   
#endif  /* __TYPE_H__ */
