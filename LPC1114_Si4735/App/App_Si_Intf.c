/*----------------------------------------------------------------------------
 * Name:   App_Si_Intf.c.c
 * Note(s):
 *----------------------------------------------------------------------------*/
#include "LPC11xx.h"
#include "App_Si_Intf.h"
#include "App_Si.h"
#include "i2c.h"
#include "lcd.h"
#include "EEPROM.h"



/*****************************************************************************
** Function name :  
** Descriptions  :  
** Parameters    :  
** Returned value:  
*****************************************************************************/
void Update_Nvm_Radio(u8 state)
{
   u8 i,temp[33];
   
   if(state == READ_DATA)
   {
      Read_EEPROM((unsigned long)EEPROM_ADDRESS,(u8*)temp,sizeof(temp)/sizeof(temp[0]));
      fm_freq = temp[0];
      fm_freq |= temp[1]<<8;
      am_freq = temp[2];
      am_freq |= temp[3]<<8;

      for(i=0;i<MAX_SW_BANDS;i++)
      {
         sw_freq[i] = (u8)temp[4+(i*2)];
         sw_freq[i] |= (temp[5+(i*2)]<<8);
      }

      radio_mode = (si_mode_T)temp[31];
      si_vol  = temp[32];
   }
   else if(state == WRITE_DATA)
   {
      temp[0] = (u8)fm_freq;
      temp[1] = (fm_freq>>8);
      temp[2] = (u8)am_freq;
      temp[3] = (am_freq>>8);

      for(i=0;i<MAX_SW_BANDS;i++)
      {
         temp[4+(i*2)] = (u8)sw_freq[i];
         temp[5+(i*2)] = (sw_freq[i]>>8);
      }

      temp[31] = radio_mode;
      temp[32] = si_vol;
      Write_EEPROM((unsigned long)EEPROM_ADDRESS,(u8*)temp,sizeof(temp)/sizeof(temp[0]));
   }
   else
   {
   
   }
}
/*****************************************************************************
** Function name :  Validate_Nvm_Radio
** Descriptions  :  
** Parameters    :  
** Returned value:  
*****************************************************************************/
void Validate_Nvm_Radio(void)
{
   u8 x,update_reqd=0;

   if((fm_freq < FM_MIN_FREQ) || 
      (fm_freq > FM_MAX_FREQ))
   {
      fm_freq = prev_fm_freq = FM_MIN_FREQ;  
      update_reqd= 1;
   }
   if((am_freq < AM_MIN_FREQ) || 
      (am_freq > AM_MAX_FREQ))
   {
      am_freq = prev_am_freq = AM_MIN_FREQ;
      update_reqd= 1;
   }
   for(x=0;x<MAX_SW_BANDS;x++)
   {
      if((sw_freq[x] < K_Sw_Min_Freq[x]) || 
         (sw_freq[x] > K_Sw_Max_Freq[x]))
      {
         sw_mode_cnt = 0;
         sw_freq[x] = prev_sw_freq[x] = K_Sw_Min_Freq[x];
         update_reqd = 1;
      }
   }
   if(si_vol > MAX_SI_VOL)
   {
      si_vol = si_vol_prev = VOL_INIT;
      update_reqd= 1;
   }
   
   if((radio_mode != FM_MODE) && 
      (radio_mode != AM_MODE) &&
      (radio_mode != SW_MODE))
   {
      radio_mode  = FM_MODE;
   }
   if(update_reqd)
   {
      Update_Nvm_Radio(WRITE_DATA);
   }
}


/*****************************************************************************
** Function name:  Write_Read_Si4735_I2C
**
** Descriptions:  
**
** parameters:   None
** Returned value:  
** 
*****************************************************************************/
unsigned char Write_Read_Si4735_I2C(
                           unsigned char chip_address,
                           unsigned char *wptr,
                           unsigned char wlen,
                           unsigned char *rptr,
                           unsigned char rlen)
{
   unsigned long lock=0;

   Start_I2C();

   //-- Set ADDRESS
   Send_I2C(chip_address & 0xFE);
   if(LPC_I2C->STAT != 0x18)
   {
      Stop_I2C();
      return 0;
   }

   while(wlen)
   {
      Send_I2C(*wptr);
      lock=0;
      while(LPC_I2C->STAT != 0x28)
      {
        if(lock == WHILE_TIMEOUT)
        {
           lock = 0;
           Stop_I2C();
           break;
        }
        lock++;
      };
      wlen--;
      *wptr++;
   }
   Start_I2C();
   Send_I2C(chip_address | 0x01);
   if(LPC_I2C->STAT != 0x40)
   {
      Stop_I2C();
      return 0;
   }

   if(rlen > 1)
   {
      LPC_I2C->CONCLR = I2C_FLAG_SI;
      LPC_I2C->CONSET |= I2C_FLAG_AA;
      for(;;)
      {
         lock=0;
         while(!(LPC_I2C->CONSET & I2C_FLAG_SI))
         {
           if(lock == WHILE_TIMEOUT)
           {
            lock = 0;
            break;
           }
           lock++;
         };  //-- End Data from slave;
         *rptr++ = (unsigned char)LPC_I2C->DAT;
         rlen--;
         if(!rlen)
         {
            break;
         }
         else if(rlen == 1)
         {
          
            LPC_I2C->CONCLR = I2C_FLAG_AA | I2C_FLAG_SI;  //-- After next will NO ASK
         }
         else
         {
            LPC_I2C->CONCLR = I2C_FLAG_SI;
            LPC_I2C->CONSET |= I2C_FLAG_AA;
         }
      }
   }
   else if(rlen == 1)
   {
      LPC_I2C->CONCLR = I2C_FLAG_AA | I2C_FLAG_SI;  //-- After next will NO ASK
      lock=0;
      while(!(LPC_I2C->CONSET & I2C_FLAG_SI))
      {
         if(lock == WHILE_TIMEOUT)
         {
            lock = 0;break;
         }
         lock++;
      };  //-- End Data from slave;
      *rptr = (unsigned char) LPC_I2C->DAT;
   }
   
   
 
   Stop_I2C();
   return 1;
}

/*-----------------------------------------------------------------------------
 * Version      Description                       Name       Date
 *
 *  1.0         Created initial version           Nandu      02-04-2025
 *---------------------------------------------------------------------------*/


