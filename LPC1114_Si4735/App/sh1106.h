/*
   filename :- sh1106.h
 */
#ifndef SH1106_H
#define SH1106_H

#ifndef SH1106_I2C_TIMEOUT
#define SH1106_I2C_TIMEOUT          20000u
#endif


/* C++ detection */
#ifdef __cplusplus
extern C {
#endif

/**
 * This SH1106 LCD uses I2C for communication
 *
 * Library features functions for drawing lines, rectangles and circles.
 *
 * It also allows you to draw texts and characters using appropriate functions provided in library.
 *
 * Default pinout
 *
SH1106     |LPC114    |DESCRIPTION

VCC        |3.3V         |
GND        |GND          |
SCL        |PIO_5        |Serial clock line
SDA        |PIO_4        |Serial data line
 */


#include "fonts.h"

#include "stdlib.h"
#include "string.h"


/* I2C address */
#ifndef SH1106_I2C_ADDR
#define SH1106_I2C_ADDR         0x3C<<1
#endif

/* SH1106 settings */
/* SH1106 width in pixels */
#ifndef SH1106_WIDTH
#define SH1106_WIDTH            128
#endif
/* SH1106 LCD height in pixels */
#ifndef SH1106_HEIGHT
#define SH1106_HEIGHT           64
#endif

/**
 * @brief  SH1106 color enumeration
 */
typedef enum {
	SH1106_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
	SH1106_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on LCD */
} SH1106_COLOR_t;


/**
 * @brief  Initializes SH1106 LCD
 * @param  None
 * @retval Initialization status:
 *           - 0: LCD was not detected on I2C port
 *           - > 0: LCD initialized OK and ready to use
 */
uint8_t SH1106_Init(void);

/** 
 * @brief  Updates buffer from internal RAM to LCD
 * @note   This function must be called each time you do some changes to LCD, to update buffer from RAM to LCD
 * @param  None
 * @retval None
 */
void SH1106_UpdateScreen(void);

/** 
 * @brief  Fills entire LCD with desired color
 * @note   @ref SH1106_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  Color: Color to be used for screen fill. This parameter can be a value of @ref SH1106_COLOR_t enumeration
 * @retval None
 */
void SH1106_Fill(SH1106_COLOR_t Color);

/**
 * @brief  Draws pixel at desired location
 * @note   @ref SH1106_UpdateScreen() must called after that in order to see updated LCD screen
 * @param  x: X location. This parameter can be a value between 0 and SH1106_WIDTH - 1
 * @param  y: Y location. This parameter can be a value between 0 and SH1106_HEIGHT - 1
 * @param  color: Color to be used for screen fill. This parameter can be a value of @ref SH1106_COLOR_t enumeration
 * @retval None
 */
void SH1106_DrawPixel(uint16_t x, uint16_t y, SH1106_COLOR_t color);

/**
 * @brief  Sets cursor pointer to desired location for strings
 * @param  x: X location. This parameter can be a value between 0 and SH1106_WIDTH - 1
 * @param  y: Y location. This parameter can be a value between 0 and SH1106_HEIGHT - 1
 * @retval None
 */
void SH1106_GotoXY(uint16_t x, uint16_t y);

/**
 * @brief  Puts character to internal RAM
 * @note   @ref SH1106_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  ch: Character to be written
 * @param  *Font: Pointer to @ref FontDef_t structure with used font
 * @param  color: Color used for drawing. This parameter can be a value of @ref SH1106_COLOR_t enumeration
 * @retval Character written
 */
char SH1106_Putc(char ch, FontDef_t* Font, SH1106_COLOR_t color);

/**
 * @brief  Puts string to internal RAM
 * @note   @ref SH1106_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  *str: String to be written
 * @param  *Font: Pointer to @ref FontDef_t structure with used font
 * @param  color: Color used for drawing. This parameter can be a value of @ref SH1106_COLOR_t enumeration
 * @retval Zero on success or character value when function failed
 */
char SH1106_Puts(char* str, FontDef_t* Font, SH1106_COLOR_t color);


void SH1106_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color);



/**
 * @brief  Initializes SH1106 LCD
 * @param  None
 * @retval Initialization status:
 *           - 0: LCD was not detected on I2C port
 *           - > 0: LCD initialized OK and ready to use
 */
void SH1106_I2C_Init(void);

/**
 * @brief  Writes single byte to slave
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  reg: register to write to
 * @param  data: data to be written
 * @retval None
 */
void SH1106_I2C_Write(uint8_t address, uint8_t reg, uint8_t data);

/**
 * @brief  Writes multi bytes to slave
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  reg: register to write to
 * @param  *data: pointer to data array to write it to slave
 * @param  count: how many bytes will be written
 * @retval None
 */
void SH1106_I2C_WriteMulti(uint8_t address, uint8_t reg, uint8_t *data, uint16_t count);

// clear the display
void SH1106_Clear(void);

unsigned char Write_I2C_SH1106(unsigned char chip_address,
                               unsigned char *wptr,
                               unsigned char wlen);


/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
