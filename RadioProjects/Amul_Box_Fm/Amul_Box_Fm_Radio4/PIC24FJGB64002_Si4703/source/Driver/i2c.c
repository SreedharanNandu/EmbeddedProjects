/*****************************************************************************
* Filename   : i2c.c
*****************************************************************************/
#include "i2c.h"

#define EXIT_LOOP_I2C   


/*****************************************************************************
* Function   : 
* Created By : 
* Date       :
* Overview   : 
* Argument   : 
* Output     : 
* Note       : 
*****************************************************************************/
void Init_I2C(void)
{
   OpenI2C1(0xD200u,36u);//for 100khz with 8Mhz crystal
   I2C1CONbits.I2CEN =1u;
}

/*****************************************************************************
* Function   : 
* Created By : 
* Date       :
* Overview   : 
* Argument   : 
* Output     : 
* Note       : 
*****************************************************************************/
void DeInit_I2C(void)
{
   I2C1CONbits.I2CEN = 0u;
}

/*****************************************************************************
* Function   : 
* Created By : 
* Date       :
* Overview   : 
* Argument   : 
* Output     : 
* Note       : 
*****************************************************************************/
void StartI2C1(void)
{
   volatile unsigned long exit_cnt=0u;
 
   I2C1CONbits.SEN = 1u;                /* initiate Start on SDA and SCL pins */
   while(I2C1CONbits.SEN)
   {
      #if defined(EXIT_LOOP_I2C)
      exit_cnt++;
      if(exit_cnt > MAX_EXIT_LOOP_TH)
      {
         exit_cnt = MAX_EXIT_LOOP_TH;
         break;
      }
      #endif
   }
}

/*****************************************************************************
* Function   : 
* Created By : 
* Date       :
* Overview   : 
* Argument   : 
* Output     : 
* Note       : 
*****************************************************************************/
void OpenI2C1(unsigned int config1,unsigned int config2)
{
   I2C1CON = config1;
   I2C1BRG = config2;
}

/*****************************************************************************
* Function   : 
* Created By : 
* Date       :
* Overview   : 
* Argument   : 
* Output     : 
* Note       : 
*****************************************************************************/
void NotAckI2C1(void)
{
   volatile unsigned long exit_cnt=0u;
   I2C1CONbits.ACKDT = 1u;
   I2C1CONbits.ACKEN = 1u;
   while(I2C1CONbits.ACKEN)
   {
      #if defined(EXIT_LOOP_I2C)
      exit_cnt++;
      if(exit_cnt > MAX_EXIT_LOOP_TH)
      {
         exit_cnt = MAX_EXIT_LOOP_TH;
         break;
      }
      #endif
   }
   I2C1CONbits.ACKDT=0u;
}

/*****************************************************************************
* Function   : 
* Created By : 
* Date       :
* Overview   : 
* Argument   : 
* Output     : 
* Note       : 
*****************************************************************************/
unsigned char MasterWriteI2C1(unsigned char data_out)
{
   volatile unsigned char status = 0u;
   volatile unsigned long exit_cnt=0u;
   
   I2C1TRN = data_out;
   if ( I2C1STATbits.IWCOL )           // test for bus collision
   {
       status = 0u;// return with Bus Collision error 
   }
   else
   {
      status = 1u;
      while(I2C1STATbits.TBF)
      {
         #if defined(EXIT_LOOP_I2C)
         exit_cnt++;
         if(exit_cnt > MAX_EXIT_LOOP_TH)
         {
            exit_cnt = MAX_EXIT_LOOP_TH;
            status = 0u;
            break;
         }
         #endif
      }
   }
   return status;
}

/*****************************************************************************
* Function   : 
* Created By : 
* Date       :
* Overview   : 
* Argument   : 
* Output     : 
* Note       : 
*****************************************************************************/
unsigned char MasterReadI2C1(void)
{
   volatile unsigned long exit_cnt=0u;
   
   I2C1CONbits.RCEN = 1u;
   while(I2C1CONbits.RCEN)
   {
      #if defined(EXIT_LOOP_I2C)
      exit_cnt++;
      if(exit_cnt > MAX_EXIT_LOOP_TH)
      {
         exit_cnt = MAX_EXIT_LOOP_TH;
         break;
      }
      #endif
   }
   I2C1STATbits.I2COV = 0u;
   return(I2C1RCV);
}

/*****************************************************************************
* Function   : 
* Created By : 
* Date       :
* Overview   : 
* Argument   : 
* Output     : 
* Note       : 
*****************************************************************************/
unsigned char MasterGetsI2C1(unsigned int length, unsigned char * rdptr, unsigned int I2C1_data_wait)
{
   volatile unsigned char status = 1u;
   volatile unsigned char temp_data;
   
   while(length)                    /* Receive the number of bytes specified by length */
   {
      temp_data = MasterReadI2C1();
      *rdptr = temp_data;             /* save byte received */
      rdptr++;
      length--;    
      if(!length)              /* If last char, generate NACK sequence */
      {
         NotAckI2C1();
      }
      else                         /* For other chars,generate ACK sequence */
      {
         AckI2C1();
      }

   }
   return status;    /* return status that number of bytes specified by length was received */
}
/*****************************************************************************
* Function   : 
* Created By : 
* Date       :
* Overview   : 
* Argument   : 
* Output     : 
* Note       : 
*****************************************************************************/
void IdleI2C1(void)
{
   volatile unsigned long exit_cnt=0u;
   /* Wait until I2C Bus is Inactive */
   while(I2C1CONbits.SEN || I2C1CONbits.PEN || I2C1CONbits.RCEN || I2C1CONbits.ACKEN || I2C1STATbits.TRSTAT)
   {
      #if defined(EXIT_LOOP_I2C)
      exit_cnt++;
      if(exit_cnt > MAX_EXIT_LOOP_TH)
      {
         exit_cnt = MAX_EXIT_LOOP_TH;
         break;
      }
      #endif
   }
}
/*****************************************************************************
* Function   : 
* Created By : 
* Date       :
* Overview   : 
* Argument   : 
* Output     : 
* Note       : 
*****************************************************************************/
void AckI2C1(void)
{
   volatile unsigned long exit_cnt=0u;
   I2C1CONbits.ACKDT = 0u;
   I2C1CONbits.ACKEN = 1u;
   while(I2C1CONbits.ACKEN)
   {
      #if defined(EXIT_LOOP_I2C)
      exit_cnt++;
      if(exit_cnt > MAX_EXIT_LOOP_TH)
      {
         exit_cnt = MAX_EXIT_LOOP_TH;
         break;
      }
      #endif
   }
}
/*****************************************************************************
* Function   : 
* Created By : 
* Date       :
* Overview   : 
* Argument   : 
* Output     : 
* Note       : 
*****************************************************************************/
void StopI2C1(void)
{
   volatile unsigned long exit_cnt=0u;
   I2C1CONbits.PEN = 1u;                /* initiate Stop on SDA and SCL pins */
   while(I2C1CONbits.PEN)
   {
      #if defined(EXIT_LOOP_I2C)
      exit_cnt++;
      if(exit_cnt > MAX_EXIT_LOOP_TH)
      {
         exit_cnt = MAX_EXIT_LOOP_TH;
         break;
      }
      #endif
   }
}
/*****************************************************************************
* Function   : 
* Created By : 
* Date       :
* Overview   : 
* Argument   : 
* Output     : 
* Note       : 
*****************************************************************************/
unsigned char SendI2C1(unsigned char slave_id,unsigned char *ptr,unsigned char num_of_bytes)
{
   
   unsigned char write_status=0u,ack_status=0u,wr_cnt=0u;
   IdleI2C1();                      // ensure module is idle
   StartI2C1();  
   if ( I2C1STATbits.BCL )           // test for bus collision
   {
      write_status = 0u;// return with Bus Collision error 
      I2C1STATbits.BCL = 0u;
   }
   else
   {
      if (MasterWriteI2C1(slave_id))    // write 1 byte
      {
         write_status = 1u;              // return with write collision error
         IdleI2C1();                    // ensure module is idle
         if (!I2C1STATbits.ACKSTAT)   // test for ACK condition, if received
         {
            ack_status = 1u;
         }
      }
      else//safe
      {
         write_status = 0u;
         ack_status = 0u ; // return with Not Ack error
      }

      if((write_status) && (ack_status) )
      {
         while(wr_cnt < num_of_bytes)
         {
            if ( MasterWriteI2C1(*ptr) )    // write 1 byte
            {
               write_status = 1u;              // return with write collision error
               IdleI2C1();                    // ensure module is idle
               if ( !I2C1STATbits.ACKSTAT )   // test for ACK condition, if received
               {
                  ack_status = 1u;
                  wr_cnt++;
                  *ptr++;
               }
            }
            else
            {
               write_status = 0u;
               ack_status = 0u;
            }
             
         }
      }
   }
   IdleI2C1(); // ensure module is idle
   StopI2C1();    

   return ((ack_status) & (write_status))  ;     // return with data
}

/*****************************************************************************
* Function   : 
* Created By : 
* Date       :
* Overview   : 
* Argument   : 
* Output     : 
* Note       : 
*****************************************************************************/
void RestartI2C1(void)
{ 
   volatile unsigned long exit_cnt=0u;
   I2C1CONbits.RSEN = 1u;   /* initiate restart on SDA and SCL pins */
   while(I2C1CONbits.RSEN)
   {
      #if defined(EXIT_LOOP_I2C)
      exit_cnt++;
      if(exit_cnt > MAX_EXIT_LOOP_TH)
      {
         exit_cnt = MAX_EXIT_LOOP_TH;
         break;
      }
      #endif
   }
}

/*****************************************************************************
* Function   : 
* Created By : 
* Date       :
* Overview   : 
* Argument   : 
* Output     : 
* Note       : 
*****************************************************************************/
unsigned char ReadI2C1(unsigned char slave_id,unsigned char *ptr,unsigned char num_of_bytes)
{
   
   unsigned char write_status=0u,ack_status=0u;

   IdleI2C1();                      // ensure module is idle
   StartI2C1();                     // initiate START condition
   if ( I2C1STATbits.IWCOL )           // test for bus collision
   {
      write_status = 0u;// return with Bus Collision error 
      I2C1STATbits.IWCOL = 0u;
   }
   else
   {
      if ( MasterWriteI2C1( slave_id+1u) )    // write 1 byte
      {
         write_status = 1u;              // return with write collision error
         IdleI2C1();                    // ensure module is idle
         if ( !I2C1STATbits.ACKSTAT )   // test for ACK condition, if received
         {
            ack_status = 1u;
         }
      }
      else//safe
      {
         write_status = 0u;
         ack_status = 0u ; // return with Not Ack error
      }
      if((write_status) && (ack_status) )
      {
         MasterGetsI2C1(num_of_bytes,ptr,200u);
      }
   }
   IdleI2C1();                      // ensure module is idle
   StopI2C1();    

   return ((ack_status) & (write_status))  ;     // return with data
}






