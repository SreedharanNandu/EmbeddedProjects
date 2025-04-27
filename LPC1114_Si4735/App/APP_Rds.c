/*filename:- App_Rds.c*/
/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/
#include "type.h"
#include <string.h>
#include "App_Si.h"
#include "type.h"


/*****************************************************************************
*                                 Macro Definitions                          *
******************************************************************************/
#define RDS_PS (0)
#define RDS_RT (2)


/*****************************************************************************
*                                 Type Declarations                          *
******************************************************************************/
u8 rdsdata[8];
u8 rdstext[64];


/*****************************************************************************
*                             Func Definitions                             
******************************************************************************/




/*****************************************************************************
*                    Globally  accessed Variable Declarations    
******************************************************************************/
/*****************************************************************************
** Function name :  
** Descriptions  :  
** Parameters    :  
** Returned value:  
*****************************************************************************/
#if RDS_MINIMAL
u8 Process_RDS(void)
{ 
   u8 ret_state = 0;

   u8 groupid;
   u8 index;//prog service
   char Ch;
   char Cl;
   char Dh;
   char Dl;
   u16 b = (KR_Resp[6]<<8) | KR_Resp[7];
   u16 c = (KR_Resp[8]<<8) | KR_Resp[9];
   u16 d = (KR_Resp[10]<<8)| KR_Resp[11];      
   
   groupid = (b & 0xF000u) >> 12;
   switch(groupid) 
   {
      case RDS_PS: 
           index = (b & 0x3)*2;//2 character per group ; 4 groups so 8 character
           Dh = (d & 0xFF00) >> 8;
           Dl = d;
           rdsdata[index] = Dh;
           rdsdata[index+1u] = Dl;
           if(6u == index)//index 6
           {
              //message complete
              ret_state  = 1;
           }
           break;
      case RDS_RT: 
           index = (b & 0xFu)*4u;//per address code has 4 charaters ,so totally 16*4 = 64 charater
           Ch = (c & 0xFF00) >> 8u;
           Cl = c;
           Dh = (d & 0xFF00) >> 8u;
           Dl = d;
           rdstext[index] = Ch;
           rdstext[index+1u] = Cl;
           rdstext[index+2u] = Dh;
           rdstext[index+3u] = Dl;
           ret_state  = 2;
   }
   return (ret_state);
}
#else

u8 Process_RDS(u8* psPtr, u8*ptPtr)
{ 
   volatile u8 ret_state = 0u;
   static u8 temp_rdsdata[9];
   static u8 temp_rdstext[65];
   volatile u8 groupid;
   volatile u8 index_ps;
   volatile u8 index_pt;
   volatile char Ch,Dh;
   volatile char Cl,Dl;
   volatile u16 b;
   volatile u16 c;
   volatile u16 d;      

   temp_rdsdata[8] = 0;//only 8 characters allowed
   temp_rdstext[64] = 0;//only 64 charaters allowed

   b = (KR_Resp[6]<<8) | KR_Resp[7];
   c = (KR_Resp[8]<<8) | KR_Resp[9];
   d = (KR_Resp[10]<<8)| KR_Resp[11];

   if(prev_fm_freq != fm_freq)
   {
      memset(temp_rdsdata,0u,9u);
      memset(temp_rdstext,0u,65u);
   }

   groupid = (b & 0xF000u) >> 12u;
   switch(groupid) 
   {
      case RDS_PS: 
                  index_ps = (b & 0x3u)*2u;//i.e 0,2,4,6 totall b=[3,2,1,0] i.e 4 * 2 i.e Dh and Dl so total 8 characters
                  Dh = (d & 0xFF00) >> 8u;
                  Dl = d;
                  temp_rdsdata[index_ps] = Dh;
                  temp_rdsdata[index_ps+1u] = Dl;
                  memcpy((char*)psPtr, (char*)&temp_rdsdata[0],9u);
                  ret_state  = 1u;
                  break;
      case RDS_RT: 
                  index_pt = (b & 0xFu)*4u;//per address code has 4 charaters ,so totally 16*4 = 64 charater
                  Ch = (c & 0xFF00) >> 8u;
                  Cl = c;
                  Dh = (d & 0xFF00) >> 8u;
                  Dl = d;
                  temp_rdstext[index_pt] = Ch;
                  temp_rdstext[index_pt+1u] = Cl;
                  temp_rdstext[index_pt+2u] = Dh;
                  temp_rdstext[index_pt+3u] = Dl;
                  memcpy((char*)ptPtr, (char*)&temp_rdstext[0],65u);
                  ret_state  = 2;
                  break;
          default:
                  break;
   }
   return (ret_state);
}
#endif





