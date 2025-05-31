#include "pic12f1840.h"
#include "types.h"

#define DEBUG_LED           0
#define BAUD_9600           1u
#define DELAY_9600          13u

#define SCL_PIN             RA1
#define SDA_PIN             RA2
#define TX_PIN              RA5



#define SW_RESET_CNT       250u


#define PCF1_I2C_ADDRESS      (0x38u)
#define PCF2_I2C_ADDRESS      (0x39u)
#define RTC_I2C_ADDRESS       (0x6Fu)



                                  //  0 ,  1     2,   3,   4,   5,   6,   7,   8,   9,   A,   b,   C,   d,   E,   F,  - ,   y,   n,   r,   u
const uint8_t sevenSegTableGen[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E,0xBF,0x91,0xAB,0xAF,0xE3};
const uint8_t sevenSegTableMin[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E,0xBF};
const uint8_t sevenSegTableHr[] =  {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc0};


volatile uint8_t s;
volatile uint8_t m;
volatile uint8_t h;


volatile uint8_t sw1_pin , prev_sw1_pin;
volatile uint8_t sw2_pin , prev_sw2_pin;

volatile uint8_t rtc_write;
volatile uint32_t sw_settle_cnt;



static void InitializeSystem(void);
static void Delay_Ms(uint32_t dlyMs);
static void Delay_Us(uint32_t dlyUs);
static uint8_t I2C_Write(uint8_t addr, uint8_t *data, uint8_t len) ;
static uint8_t I2C_Read(uint8_t i2c_addr, 
                              uint8_t *data, 
                              uint8_t restart_required,
                              uint8_t addr_byte,
                              uint8_t len) ;
static void I2C_Init(void);
static void Init_All(void);
static void Disp_Init(void);
static void Read_RTC(void);
static void Disp_Time(void);
static void Read_Sw(void);
static void Write_RTC(void);
static uint8_t Bcd2dec(uint8_t num);
static uint8_t Dec2bcd(uint8_t num);
#if 0
static void Soft_USART_Write(uint8_t chr);
#endif
static void Edit_Hr(void);
static void Edit_Min(void);






/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void main(void)
{
   volatile  uint8_t data1 = 0;
   volatile  uint16_t time2read = 0;

   //power stabilization
   volatile  uint16_t data = 9000u;
   while(data--);
   
   InitializeSystem();
   I2C_Init();
   Init_All();
   Disp_Init();

   while(1)
   {
      Read_RTC();
      Read_Sw();
      Disp_Time();
      Write_RTC();
   } 
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
static void InitializeSystem(void) 
{
   //write main config 
   OSCCON = 0x78u;   //clock uses 16Mhz HFINTOSC
   OPTION_REG = 0x03u; //1:16 for TMR0 i.e 250Khz
   CM1CON0 = 7u;    //comparators off
   ANSELA = 1u;     //all digital GPIO pins except ANSA0

   //set port
   //VDD  pin1
   TRISA5 = 0u;//gpio pin2
   TRISA4 = 0u;//gpio pin3
   TRISA3 = 0u;//mclr pin4
   TRISA2 = 1u;//sda pin5
   TRISA1 = 1u;//scl  pin6
   TRISA0 = 1u;//gpio  pin7
   //VSS  pin8

   
   //enable interrupts
   PEIE = 1;

}

static void I2C_Init(void)
{
   /* CKE disabled; SMP Standard Speed;  */
   SSP1STAT = 0x80;
   /* SSPM FOSC/4_SSPxADD_I2C; CKP disabled; SSPEN disabled; SSPOV no_overflow; WCOL no_collision;  */
   SSP1CON1 = 0x8;
   /* SEN disabled; RSEN disabled; PEN disabled; RCEN disabled; ACKEN disabled; ACKDT acknowledge; GCEN disabled;  */
   SSP1CON2 = 0x0;
   /* DHEN disabled; AHEN disabled; SBCDE disabled; SDAHT 100ns; BOEN disabled; SCIE disabled; PCIE disabled;  */
   SSP1CON3 = 0x0;
   /* SSPADD 39;  */
   SSP1ADD = 37u; //for 100khz
   SSP1CON1bits.SSPEN = 1;
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
static void Delay_Ms(uint32_t dlyMs)
{
   //1msec X a
   while(dlyMs)
   {
      TMR0 = 0u;//1ms timer i.e 8 bit TMR with fosc/4  = 16mhz/4 = 4mhz with 1:16 prescalar = 250Khz clock,so MAX = 255 * 4us = 1020us
      TMR0IF = 0u;
      TMR0IE = 0u;
      while(!TMR0IF);
      dlyMs--;
   }
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
static void Delay_Us(uint32_t dlyUs)
{
   while(dlyUs)
   {
       asm("nop");
      dlyUs--;
   }  
  
}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
static uint8_t I2C_Read(uint8_t i2c_addr, 
                        uint8_t *data, 
                        uint8_t restart_required,
                        uint8_t addr_byte,
                        uint8_t len) 
{
    volatile uint8_t ret = 1u;
    volatile uint8_t i=0;
    //1 CLear IRQ
    PIR1bits.SSP1IF = 0;
    
    //2. enable serial port hw
    SSP1CON1bits.SSPEN = 1;
    
    if(restart_required)
    {
       
       // Generate a START condition by setting Start Enable bit
       SSP1CON2bits.SEN = 1;
       
       // Wait for START to be completed. (1) interrupt pending, (0) interrupt not pending.
       while (!PIR1bits.SSP1IF);
       PIR1bits.SSP1IF = 0; // Clear flag
       
       SSP1BUF = (uint8_t) (i2c_addr << 1) ; //shift address up 1. write the address

       while (!PIR1bits.SSP1IF);
       PIR1bits.SSP1IF = 0; // Clear flag


       if (SSP1CON2bits.ACKSTAT) 
       {
        ret = 0u;
        goto clean_up;
       }
 
       SSP1BUF = (uint8_t) (addr_byte) ;

       while (!PIR1bits.SSP1IF);
       PIR1bits.SSP1IF = 0; // Clear flag

       while (SSP1CON2bits.ACKSTAT);
       #if 0       
       if (SSP1CON2bits.ACKSTAT) //ack not received
       {
          ret = 0u;
          goto clean_up;
       }
       #endif 
       // Generate a STOP condition
       SSP1CON2bits.PEN = 1;
    
       // Wait for STOP to be completed
       while (!PIR1bits.SSP1IF);
       PIR1bits.SSP1IF = 0; // Clear flag
    }

    // Generate a START condition by setting Start Enable bit
    SSP1CON2bits.SEN = 1;
    
    // Wait for START to be completed. (1) interrupt pending, (0) interrupt not pending.
    while (!PIR1bits.SSP1IF);
    PIR1bits.SSP1IF = 0; // Clear flag

    SSP1BUF = (uint8_t) (i2c_addr << 1) | 1; //shift address up 1. OR with 1 for read
    
    while (!PIR1bits.SSP1IF);
    PIR1bits.SSP1IF = 0; // Clear flag
    
    //Check ack status.
    //ack:ACKSTAT == 0.
    //nack:ACKSTAT == 1
    if (SSP1CON2bits.ACKSTAT) 
    {
     ret = 0u;
     goto clean_up;
    }
    
    for(i = 0; i < len; i++) 
    {
       SSP1CON2bits.RCEN = 1; //receive another byte
    
       while (!PIR1bits.SSP1IF); // Wait for byte to be received
       PIR1bits.SSP1IF = 0; // Clear flag
       
       // read data from reg
       *data = SSP1BUF;
       
       // Set ack bit
       if (i < (len - 1)) 
       {
          SSP1CON2bits.ACKDT = 0; //0 == ack, 1 == nack
          *data++;
       }
       else
       {
          SSP1CON2bits.ACKDT = 1;
       }
       SSP1CON2bits.ACKEN = 1; //push ack data out to scl/sda
       
       
       while (!PIR1bits.SSP1IF); //wait for ack completion
       PIR1bits.SSP1IF = 0; //clear flag
    }


  clean_up:
    
    // Generate a STOP condition
    SSP1CON2bits.PEN = 1;
 
    // Wait for STOP to be completed
    while (!PIR1bits.SSP1IF);
    PIR1bits.SSP1IF = 0; // Clear flag
 
    return ret;
}
 
/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
static uint8_t I2C_Write(uint8_t addr, 
                         uint8_t *data, 
                         uint8_t len) 
{
   uint8_t ret = 1u;
   
   //1 Clear IRQ if set
   PIR1bits.SSP1IF = 0;
   
   //2. enable serial port hw
   SSP1CON1bits.SSPEN = 1;
   
   // Generate a START condition by setting Start Enable bit
   SSP1CON2bits.SEN = 1;
   
   // Wait for START to be completed. (1) interrupt pending, (0) interrupt not pending.
   while (!PIR1bits.SSP1IF);
   PIR1bits.SSP1IF = 0; // Clear flag
   
   SSP1BUF = (uint8_t) (addr << 1); //shift address up 1
   
   while (!PIR1bits.SSP1IF);
   PIR1bits.SSP1IF = 0; // Clear flag
   
   //Check ack status.
   //ack:ACKSTAT == 0.
   //nack:ACKSTAT == 1
   if (SSP1CON2bits.ACKSTAT) 
   {
      ret = 0u;
      goto clean_up;
   }
   
   for (int i = 0; i < len; i++) 
   {
      // Load data in SSP1BUF
      SSP1BUF = *data;
      
      // Wait for MSSP interrupt
      while (!PIR1bits.SSP1IF); // Clear flag
      PIR1bits.SSP1IF = 0;
      
      // Check for ack
      if (SSP1CON2bits.ACKSTAT)
      {
         if (i < (len - 1)) 
         {
           //if its NOT the last byte, err.
           ret = 0u;
         }
         break;
      }
      else //ack received
      {
         *data++;
      }
   
   }
clean_up:
   // Generate a STOP condition
   SSP1CON2bits.PEN = 1;
 
   // Wait for STOP to be completed
   while (!PIR1bits.SSP1IF);
   PIR1bits.SSP1IF = 0; // Clear flag
 
   return ret;
}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
static void Read_RTC(void)
{
   volatile  uint8_t i;
   uint8_t data_temp[7];
   
   if((!rtc_write) && (!sw_settle_cnt))
   {
      for(i=0;i<7;i++)
      {
         data_temp[i] = 0u;
      }

      I2C_Read(RTC_I2C_ADDRESS,(uint8_t*)&data_temp[0],1,0,7);

      s = Bcd2dec(data_temp[0] & 0x7Fu);
      m = Bcd2dec(data_temp[1] & 0x7Fu);
      h = Bcd2dec(data_temp[2] & 0x1Fu);//HR21 is not checked here
   }
}
/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
static void Init_All(void)
{
   volatile  uint8_t data[2] = {0u,0u};

   //set all 7seg to 0, turn OFF
   I2C_Write(PCF1_I2C_ADDRESS,(uint8_t*)&data[0],1u);
   //turn all CA nodes to OFF
   I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data[0],1u);

   //Read the RTC 0x0D reg
   I2C_Read(RTC_I2C_ADDRESS,(uint8_t*)&data[1],1,0x0D,1u);
   data[0] = 0x0Du;
   data[1] = data[1] | 0xA0;
   I2C_Write(RTC_I2C_ADDRESS,(uint8_t*)&data[0],2u);
   I2C_Read(RTC_I2C_ADDRESS,(uint8_t*)&data[1],1,0x0D,1u);


}
/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
static void Disp_Init(void) 
{
   volatile  uint8_t cnt = 50;
   volatile  uint8_t data = 0;
   volatile  uint8_t blank_disp_seg = 0xFF;
   while(cnt--)
   {
      I2C_Write(PCF1_I2C_ADDRESS,(uint8_t*)&sevenSegTableGen[13],1u);//d
      data = 0xFD;
      I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);
      Delay_Ms(7);
      I2C_Write(PCF1_I2C_ADDRESS,(uint8_t*)&sevenSegTableGen[10],1u);//a
      data = 0xFB;
      I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);
      Delay_Ms(7);
      I2C_Write(PCF1_I2C_ADDRESS,(uint8_t*)&sevenSegTableGen[11],1u);//d
      data = 0xF7;
      I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);
      Delay_Ms(7);
   }
   data = 0xFF;
   I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);
   cnt = 50;
   while(cnt--)
   { 
      I2C_Write(PCF1_I2C_ADDRESS,(uint8_t*)&sevenSegTableGen[17],1u);//y
      data = 0xFD;
      I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);
      Delay_Ms(7);
      I2C_Write(PCF1_I2C_ADDRESS,(uint8_t*)&sevenSegTableGen[0],1u);//o
      data = 0xFB;
      I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);
      Delay_Ms(7);
      I2C_Write(PCF1_I2C_ADDRESS,(uint8_t*)&sevenSegTableGen[11],1u);//b
      data = 0xF7;
      I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);
      Delay_Ms(7);
   }    
   data = 0xFF;
   I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);
   cnt = 50;
   while(cnt--)
   { 
      I2C_Write(PCF1_I2C_ADDRESS,(uint8_t*)&sevenSegTableGen[18],1u);//n
      data = 0xFD;
      I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);
      Delay_Ms(7);
      I2C_Write(PCF1_I2C_ADDRESS,(uint8_t*)&sevenSegTableGen[10],1u);//a
      data = 0xFB;
      I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);
      Delay_Ms(7);
      I2C_Write(PCF1_I2C_ADDRESS,(uint8_t*)&sevenSegTableGen[18],1u);//n
      data = 0xF7;
      I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);
      Delay_Ms(7);
   }    
   data = 0xFF;
   I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);
   cnt = 50;
   while(cnt--)
   { 
      I2C_Write(PCF1_I2C_ADDRESS,(uint8_t*)&blank_disp_seg,1u);//blank
      data = 0xFD;
      I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);
      Delay_Ms(7);
      I2C_Write(PCF1_I2C_ADDRESS,(uint8_t*)&sevenSegTableGen[20],1u);//u
      data = 0xFB;
      I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);
      Delay_Ms(7);
      I2C_Write(PCF1_I2C_ADDRESS,(uint8_t*)&sevenSegTableGen[13],1u);//d
      data = 0xF7;
      I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);
      Delay_Ms(7);
   }    
   data = 0xFF;
   I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);
 
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
static void Disp_Time(void)
{
   static volatile  uint8_t prev_s;
   static volatile  uint8_t dot_toggle;
   volatile  uint8_t data = 0; uint8_t temp = 0;

   
   //display minute in 1s and 10s  
   I2C_Write(PCF1_I2C_ADDRESS,(uint8_t*)&sevenSegTableMin[m%10],1u);
   data = 0xFD;
   I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);
   Delay_Ms(5);

   I2C_Write(PCF1_I2C_ADDRESS,(uint8_t*)&sevenSegTableMin[m/10],1u);
   data = 0xFB;
   I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);
   Delay_Ms(5);

   //display seconds
   if(s != prev_s)
   {
      dot_toggle = !dot_toggle;
      prev_s = s;
   }
   //display hour in 1s  
   if(h > 9)
   {
      temp = sevenSegTableHr[h] & ((dot_toggle==1)?0x7F:0xFF);
      I2C_Write(PCF1_I2C_ADDRESS,(uint8_t*)&temp,1u);
   }  
   else
   {
      temp = sevenSegTableHr[h%10] & ((dot_toggle==1)?0x7F:0xFF);
      I2C_Write(PCF1_I2C_ADDRESS,(uint8_t*)&temp,1u);
   }
   data = 0xF7;
   I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);
   Delay_Ms(5);
   data = 0xFF;
   I2C_Write(PCF2_I2C_ADDRESS,(uint8_t*)&data,1u);


}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
static void Read_Sw(void)
{
   uint8_t data = 0;
   
   I2C_Read(PCF2_I2C_ADDRESS,(uint8_t*)&data,0,0,1u);

   sw1_pin = ((data & 0x10) == 0x10)? 0:1;
   sw2_pin = ((data & 0x20) == 0x20)? 0:1;

   if(sw1_pin != prev_sw1_pin)
   {
      if((sw1_pin == 0) && (prev_sw1_pin == 1))
      {
         rtc_write = 1;
         sw_settle_cnt = SW_RESET_CNT;
         Edit_Hr();
      }
      else if((sw1_pin == 0) && (prev_sw1_pin == 0))
      {
         rtc_write = 1;
         sw_settle_cnt = SW_RESET_CNT;
         Edit_Hr();
      }
      else if((sw1_pin == 1) && (prev_sw1_pin == 0))
      {
      
      }
      else if((sw1_pin == 1) && (prev_sw1_pin == 1))
      {
      
      }
      else
      {
         //do nothing
      }
      prev_sw1_pin = sw1_pin;
   }      
   

   if(sw2_pin != prev_sw2_pin)
   {
      if((sw2_pin == 0) && (prev_sw2_pin == 1))
      {
         rtc_write = 1;
         sw_settle_cnt = SW_RESET_CNT;
         Edit_Min();
      }
      else if((sw2_pin == 0) && (prev_sw2_pin == 0))
      {
         rtc_write = 1;
         sw_settle_cnt = SW_RESET_CNT;
         Edit_Min();
      }
      else if((sw2_pin == 1) && (prev_sw2_pin == 0))
      {
         
      }
      else if((sw2_pin == 1) && (prev_sw2_pin == 1))
      {
      
      }
      else
      {
         //do nothing
      }
      prev_sw2_pin = sw2_pin;
   }      
   if(sw_settle_cnt > 0)
   {
      sw_settle_cnt--;
   }
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
static void Write_RTC(void)
{
   uint8_t data[8]={0,0,0,0,0,0,0,0};

   if((rtc_write) && (!sw_settle_cnt))
   {
      I2C_Read(RTC_I2C_ADDRESS,(uint8_t*)&data[1],1,0x08,1u);
      data[0] = 8;
      data[1] = data[1] | 0x40;
      I2C_Write(RTC_I2C_ADDRESS,(uint8_t*)&data[0],2u);
      
      data[0] = 0;
      data[1] = 0;
      data[2] = Dec2bcd(m);
      data[3] = Dec2bcd(h);
      data[4] = Dec2bcd(0);
      data[5] = Dec2bcd(0);
      data[6] = Dec2bcd(0);
      data[7] = Dec2bcd(0);
      I2C_Write(RTC_I2C_ADDRESS,(uint8_t*)&data[0],8u);
      Delay_Ms(10);
      rtc_write = 0;
   }
}
#if 0
/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
static void Soft_USART_Write(uint8_t chr) 
{                      
   uint16_t mask=1u,i;
   uint16_t data; 
                        
   data = chr << 1;
   data &= ~0x0001u; 
                        
   // 10 output bits    
   for (i=0u;i<9u;i++)
   { 
      // lsb 1st  8 bits
      if (mask & data)
      {                
          TX_PIN = 1u;
      }
      else 
      {                
          TX_PIN = 0u;
      }                
      mask <<=1u;      
      Delay_Us(DELAY_9600); 
   }
   TX_PIN = 1u; // stop bit
   Delay_Us(DELAY_9600); 

}
#endif


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
static uint8_t Bcd2dec(uint8_t num)
{
  return ((num/16 * 10) + (num % 16));
}
/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
static uint8_t Dec2bcd(uint8_t num)
{
  return ((num/10 * 16) + (num % 10));
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
static void Edit_Min(void)
{
   m++;
   if(Dec2bcd(m)>0x59)
   {
      m = 0;
   }
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
static void Edit_Hr(void)
{
   h++;
   if(Dec2bcd(h)>0x12)
   {
      h = 0;
   }
}

