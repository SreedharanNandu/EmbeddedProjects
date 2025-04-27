/*
   filename :- sh1106.c
 */
#include "sh1106.h"
#include "i2c.h"
#include "LPC11xx.h"



/* Write command */
#define SH1106_WRITECOMMAND(command)      SH1106_I2C_Write(SH1106_I2C_ADDR, 0x00, (command))
/* Write data */
#define SH1106_WRITEDATA(data)            SH1106_I2C_Write(SH1106_I2C_ADDR, 0x40, (data))
/* Absolute value */
#define ABS(x)   ((x) > 0 ? (x) : -(x))

/* SH1106 data buffer */
static uint8_t SH1106_Buffer[SH1106_WIDTH * SH1106_HEIGHT / 8];

/* Private SH1106 structure */
typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Inverted;
    uint8_t Initialized;
} SH1106_t;

/* Private variable */
static SH1106_t SH1106;

#define SH1106_NORMALDISPLAY       0xA6
#define SH1106_INVERTDISPLAY       0xA7





uint8_t SH1106_Init(void) 
{
   #if 1
     // Initialize the display
   SH1106_WRITECOMMAND(0xAE); //display off
   SH1106_WRITECOMMAND(0xB0|0x00); //Set Page Start Address for Page Addressing Mode,0-7

   SH1106_WRITECOMMAND(0x81); //--set contrast control register
   SH1106_WRITECOMMAND(0xFF); // contrast value

   SH1106_WRITECOMMAND(0xA1); //--set segment re-map 0 to 127
   SH1106_WRITECOMMAND(0xA6); //--set normal display

   SH1106_WRITECOMMAND(0xA8); //--set multiplex ratio(1 to 64)
   SH1106_WRITECOMMAND(0x3F); // multiplex value

   SH1106_WRITECOMMAND(0xAD); // Set Pump Mode
   SH1106_WRITECOMMAND(0x8B); // Pump ON

   SH1106_WRITECOMMAND(0x30|0x02); // Set Pump Voltage 8.0

   SH1106_WRITECOMMAND(0xC8); //Set COM Output Scan Direction
   SH1106_WRITECOMMAND(0xD3); //-set display offset
   SH1106_WRITECOMMAND(0x00); //-not offset

   SH1106_WRITECOMMAND(0xD5); //--set display clock divide ratio/oscillator frequency
   SH1106_WRITECOMMAND(0x80); //--set divide ratio

   SH1106_WRITECOMMAND(0xD9); //--set pre-charge period
   SH1106_WRITECOMMAND(0x1F); //

   SH1106_WRITECOMMAND(0xDA); //--set com pins hardware configuration
   SH1106_WRITECOMMAND(0x12);

   SH1106_WRITECOMMAND(0xDB); //--set vcomh
   SH1106_WRITECOMMAND(0x40); //

   SH1106_WRITECOMMAND(0xAF); //--turn on SH1106 panel

   #else

   SH1106_WRITECOMMAND(0xAE); //0xae, Display OFF
   
   SH1106_WRITECOMMAND(0x02); //0x02,Low Column shifted because the RAM has 132 columns
   SH1106_WRITECOMMAND(0x10); //0x10,//High Column
   
   SH1106_WRITECOMMAND(0x40); // 0x40, line #0, start line
   
   SH1106_WRITECOMMAND(0xB0); // set page address
   
   SH1106_WRITECOMMAND(0x81); // 0x81, set contrast
   SH1106_WRITECOMMAND(0x80);
   
   SH1106_WRITECOMMAND(0xa1); //0xA1,//remap
   
   SH1106_WRITECOMMAND(0xa4); // 0xA4
   SH1106_WRITECOMMAND(0xA6); // 0xA6, normal display
   
   SH1106_WRITECOMMAND(0xA8); // 0xA8, multiplex ratio
   SH1106_WRITECOMMAND(0x3F); // 63, 1/64 duty
   
   SH1106_WRITECOMMAND(0xAD); // DC-DC ON
   SH1106_WRITECOMMAND(0x8B);
   
   SH1106_WRITECOMMAND(0x33); // VPP=9.0V
   
   SH1106_WRITECOMMAND(0xc8); // scan direction
   
   SH1106_WRITECOMMAND(0xD3); // 0xD3, display offset
   SH1106_WRITECOMMAND(0x0); // no offset
   
   SH1106_WRITECOMMAND(0xD5); // 0xD5, Display clock divide
   SH1106_WRITECOMMAND(0x80); // the suggested ratio 0x80
   
   SH1106_WRITECOMMAND(0xD9); // 0xd9, precharge period
   SH1106_WRITECOMMAND(0x22);
   
   SH1106_WRITECOMMAND(0xDA); // 0xDA, com pins
   SH1106_WRITECOMMAND(0x12);
   
   SH1106_WRITECOMMAND(0xDB); // 0xDB, VCOM deselect
   SH1106_WRITECOMMAND(0x40);
   

   SH1106_WRITECOMMAND(0xAF); //0xAF, display ON

 

   #endif


   /* Clear screen */
   SH1106_Fill(SH1106_COLOR_BLACK);
   
   /* Update screen */
   SH1106_UpdateScreen();
   
   /* Set default values */
   SH1106.CurrentX = 0;
   SH1106.CurrentY = 0;
   
   /* Initialized OK */
   SH1106.Initialized = 1;



   
   /* Return OK */
   return 1;
}



void SH1106_UpdateScreen(void) 
{
   uint8_t m;
    
   for (m = 0; m < 8; m++) 
   {
      SH1106_WRITECOMMAND(0xB0 + m);
      SH1106_WRITECOMMAND(0x02);
      SH1106_WRITECOMMAND(0x10);
       
      /* Write multi data */
      SH1106_I2C_WriteMulti(SH1106_I2C_ADDR, 0x40, &SH1106_Buffer[SH1106_WIDTH * m], SH1106_WIDTH);
   }
}


void SH1106_Fill(SH1106_COLOR_t color) 
{
   /* Set memory */
   memset(SH1106_Buffer, (color == SH1106_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(SH1106_Buffer));
}

void SH1106_DrawPixel(uint16_t x, uint16_t y, SH1106_COLOR_t color) 
{
   if((x >= SH1106_WIDTH) ||
      (y >= SH1106_HEIGHT))
   {
      /* Error */
      return;
   }
   
   /* Check if pixels are inverted */
   if (SH1106.Inverted) 
   {
      color = (SH1106_COLOR_t)!color;
   }
   
   /* Set color */
   if (color == SH1106_COLOR_WHITE) 
   {
      SH1106_Buffer[x + (y / 8) * SH1106_WIDTH] |= 1 << (y % 8);
   }
   else
   {
      SH1106_Buffer[x + (y / 8) * SH1106_WIDTH] &= ~(1 << (y % 8));
   }
}

void SH1106_GotoXY(uint16_t x, uint16_t y)
{
   /* Set write pointers */
   SH1106.CurrentX = x;
   SH1106.CurrentY = y;
}

char SH1106_Putc(char ch, FontDef_t* Font, SH1106_COLOR_t color) 
{
   uint32_t i, b, j;
   
   /* Check available space in LCD */
   if((SH1106_WIDTH <= (SH1106.CurrentX + Font->FontWidth)) ||
      (SH1106_HEIGHT <= (SH1106.CurrentY + Font->FontHeight)))
   {
      /* Error */
      return 0;
   }
   
   /* Go through font */
   for (i = 0; i < Font->FontHeight; i++) 
   {
      b = Font->data[(ch - 32) * Font->FontHeight + i];
      for (j = 0; j < Font->FontWidth; j++) 
      {
         if ((b << j) & 0x8000) 
         {
             SH1106_DrawPixel(SH1106.CurrentX + j, (SH1106.CurrentY + i), (SH1106_COLOR_t) color);
         }
         else 
         {
             SH1106_DrawPixel(SH1106.CurrentX + j, (SH1106.CurrentY + i), (SH1106_COLOR_t)!color);
         }
      }
   }
   
   /* Increase pointer */
   SH1106.CurrentX += Font->FontWidth;
   
   /* Return character written */
   return ch;
}

char SH1106_Puts(char* str, FontDef_t* Font, SH1106_COLOR_t color) 
{
   /* Write characters */
   while (*str) 
   {
      /* Write character by character */
      if (SH1106_Putc(*str, Font, color) != *str) 
      {
          /* Return error */
          return *str;
      }
      
      /* Increase string pointer */
      str++;
   }
   
   /* Everything OK, zero should be returned */
   return *str;
}
 
void SH1106_Clear (void)
{
   SH1106_Fill((SH1106_COLOR_t)0);
   SH1106_UpdateScreen();
}
void SH1106_ON(void) 
{
   SH1106_WRITECOMMAND(0x8D);
   SH1106_WRITECOMMAND(0x14);
   SH1106_WRITECOMMAND(0xAF);
}
void SH1106_OFF(void) 
{
   SH1106_WRITECOMMAND(0x8D);
   SH1106_WRITECOMMAND(0x10);
   SH1106_WRITECOMMAND(0xAE);
}

void SH1106_I2C_WriteMulti(uint8_t address, uint8_t reg, uint8_t* data, uint16_t count) 
{
   uint8_t dt[256];
   uint8_t i;

   dt[0] = reg;
   for(i = 0; i < count; i++)
   {
      dt[i+1] = data[i];
   }
   Write_I2C_SH1106(SH1106_I2C_ADDR,dt,count+1);
}


void SH1106_I2C_Write(uint8_t address, uint8_t reg, uint8_t data) 
{
   uint8_t dt[2];
   dt[0] = reg;
   dt[1] = data;
   Write_I2C_SH1106(SH1106_I2C_ADDR,dt,2);
}

/*****************************************************************************
** Function name:  Write_I2C_SH1106
**
** Descriptions:  
**
** parameters:   None
** Returned value:  
** 
*****************************************************************************/
unsigned char Write_I2C_SH1106(unsigned char chip_address,
                               unsigned char *wptr,
                               unsigned char wlen)
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
   Stop_I2C();
   return 1;
}

void SH1106_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color)
{
   int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
   uint8_t byte = 0;
   int16_t j=0;
	int16_t i=0;
   for(j=0; j<h; j++, y++)
   {
      for(i=0; i<w; i++)
      {
         if(i & 7)
         {
            byte <<= 1;
         }
         else
         {
            byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
         }
         if(byte & 0x80) SH1106_DrawPixel(x+i, y, (SH1106_COLOR_t)color);
      }
   }
}




