/*****************************************************************************
* Filename   : i2c.c
*****************************************************************************/
#include "i2c.h"


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
   I2C2CONbits.I2CEN =0u;
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
   I2C1CONbits.SEN = 1;                /* initiate Start on SDA and SCL pins */
   while(I2C1CONbits.SEN);
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
   I2C1CONbits.ACKDT = 1;
   I2C1CONbits.ACKEN = 1;
   while(I2C1CONbits.ACKEN);
   I2C1CONbits.ACKDT=0;
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
   unsigned char status = 0;
   
   I2C1TRN = data_out;
   if ( I2C1STATbits.IWCOL )           // test for bus collision
   {
       status = 0;// return with Bus Collision error 
   }
   else
   {
      while(I2C1STATbits.TBF);
      status = 1;
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
   
   I2C1CONbits.RCEN = 1;
   while(I2C1CONbits.RCEN);
   I2C1STATbits.I2COV = 0;
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
   volatile unsigned char status = 1;
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
   /* Wait until I2C Bus is Inactive */
   while(I2C1CONbits.SEN || I2C1CONbits.PEN || I2C1CONbits.RCEN || I2C1CONbits.ACKEN || I2C1STATbits.TRSTAT);
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
   I2C1CONbits.ACKDT = 0;
   I2C1CONbits.ACKEN = 1;
   while(I2C1CONbits.ACKEN);
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
   I2C1CONbits.PEN = 1;                /* initiate Stop on SDA and SCL pins */
   while(I2C1CONbits.PEN);
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
   
   unsigned char write_status=0,ack_status=0,wr_cnt=0;
   IdleI2C1();                      // ensure module is idle
   StartI2C1();  
   if ( I2C1STATbits.BCL )           // test for bus collision
   {
      write_status = 0;// return with Bus Collision error 
      I2C1STATbits.BCL = 0;
   }
   else
   {
      if (MasterWriteI2C1(slave_id))    // write 1 byte
      {
         write_status = 1;              // return with write collision error
         IdleI2C1();                    // ensure module is idle
         if (!I2C1STATbits.ACKSTAT)   // test for ACK condition, if received
         {
            ack_status = 1;
         }
      }
      else//safe
      {
         write_status = 0;
         ack_status = 0 ; // return with Not Ack error
      }

      if((write_status) && (ack_status) )
      {
         while(wr_cnt < num_of_bytes)
         {
            if ( MasterWriteI2C1(*ptr) )    // write 1 byte
            {
               write_status = 1;              // return with write collision error
               IdleI2C1();                    // ensure module is idle
               if ( !I2C1STATbits.ACKSTAT )   // test for ACK condition, if received
               {
                  ack_status = 1;
                  wr_cnt++;
                  *ptr++;
               }
            }
            else
            {
               write_status = 0;
               ack_status = 0;
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
   I2C1CONbits.RSEN = 1;   /* initiate restart on SDA and SCL pins */
   while(I2C1CONbits.RSEN);
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
   
   unsigned char write_status=0,ack_status=0;

   IdleI2C1();                      // ensure module is idle
   StartI2C1();                     // initiate START condition
   if ( I2C1STATbits.IWCOL )           // test for bus collision
   {
      write_status = 0;// return with Bus Collision error 
      I2C1STATbits.IWCOL = 0;
   }
   else
   {
      if ( MasterWriteI2C1( slave_id+1) )    // write 1 byte
      {
         write_status = 1;              // return with write collision error
         IdleI2C1();                    // ensure module is idle
         if ( !I2C1STATbits.ACKSTAT )   // test for ACK condition, if received
         {
            ack_status = 1;
         }
      }
      else//safe
      {
         write_status = 0;
         ack_status = 0 ; // return with Not Ack error
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
/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
unsigned char exitLoop(unsigned char id)
{
   volatile unsigned char status = 0;//means timeout
   static unsigned int cnt = 0;
   static unsigned char prev_id = 0;
   
   if(id != prev_id)
   {
      cnt = 0;
   }
   else
   {
      cnt++;
   }
   if(cnt > MAX_EXIT_LOOP_TH)
   {
 //     status = 1;
   }
   prev_id = id;
   return status;
}






