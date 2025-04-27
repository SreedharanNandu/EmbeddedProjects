/*gpio.c*/

#include <stdio.h>
#include "LPC11xx.h"         /* LPC11xx Peripheral Registers */
#include "gpio.h"
#include "System.h"
#include "App_Si.h"



/*****************************************************************************
** Function name:      GPIOInit
**
** Descriptions:      Initialize GPIO, install the
**                  GPIO interrupt handler
**
** parameters:         None
** Returned value:      true or false, return false if the VIC table
**                  is full and GPIO interrupt handler can be
**                  installed.
** 
*****************************************************************************/
void GPIOInit( void )
{
  /* Enable AHB clock to the GPIO domain. */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

#ifdef __JTAG_DISABLED  
  LPC_IOCON->JTAG_TDO_PIO1_1  &= ~0x07;
  LPC_IOCON->JTAG_TDO_PIO1_1  |= 0x01;
#endif

  /* Set up NVIC when I/O pins are configured as external interrupts. */
  NVIC_DisableIRQ(EINT0_IRQn);
  NVIC_DisableIRQ(EINT1_IRQn);
  NVIC_DisableIRQ(EINT2_IRQn);
  NVIC_DisableIRQ(EINT3_IRQn);
  
  LPC_IOCON->PIO0_1 = 0;
  LPC_IOCON->PIO0_2 = 1;
  LPC_IOCON->PIO0_3 = 0;
  LPC_IOCON->PIO0_4 = 0;
  LPC_IOCON->PIO0_5 = 0;
  LPC_IOCON->PIO0_6 = 2;
  LPC_IOCON->PIO0_7 = 0;
  LPC_IOCON->PIO0_8 = 0;
  LPC_IOCON->PIO0_9 = 0;
  LPC_IOCON->JTAG_TDI_PIO0_11 = 0x11;//pull up + gpio

  LPC_IOCON->JTAG_TMS_PIO1_0 = 0x91;//gpio pull up enabled
  LPC_IOCON->JTAG_TDO_PIO1_1 = 0x91;//gpio pull up enabled

  LPC_IOCON->JTAG_nTRST_PIO1_2 = 0x91;//gpio pull up enabled
  LPC_IOCON->PIO1_4 = 0x90;//gpio pull up enabled
  LPC_IOCON->PIO1_5 = 0;
  
  LPC_IOCON->PIO1_6 = 1;//RXD
  LPC_IOCON->PIO1_7 = 1;//TXD
  
  LPC_IOCON->PIO1_8 = 0;
  LPC_IOCON->PIO1_9 = 0;

 
  //direction (1 out, 0 input)
  GPIOSetDir( PORT0, 1, 1);//Si4735 senb 
  GPIOSetDir( PORT0, 2, 1);//
  GPIOSetDir( PORT0, 3, 1);//
  GPIOSetDir( PORT0, 4, 1);//I2C SCL
  GPIOSetDir( PORT0, 5, 1);//I2C SDA
  GPIOSetDir( PORT0, 6, 1);//
  GPIOSetDir( PORT0, 7, 1);//Si4735 rstb
  GPIOSetDir( PORT0, 8, 0);//info pin
  GPIOSetDir( PORT0, 9, 1);//lcd SPI MOSI
  GPIOSetDir( PORT0,11, 1);//TPA mute amp

  GPIOSetDir( PORT1, 0, 0 );//vol up
  GPIOSetDir( PORT1, 1, 0 );//vol dwn
  GPIOSetDir( PORT1, 2, 0 );//band
  GPIOSetDir( PORT1, 4, 0 );//enc B
  GPIOSetDir( PORT1, 5, 0 );//enc A
  GPIOSetDir( PORT1, 6, 0 );//RXD
  GPIOSetDir( PORT1, 7, 1 );//TXD
  GPIOSetDir( PORT1, 8, 1 );//
  GPIOSetDir( PORT1, 9, 1 );//

  
  //data 
  GPIOSetValue(PORT0,4 ,1);
  GPIOSetValue(PORT0,5 ,1); 
  GPIOSetValue(PORT0,6 ,1);
  GPIOSetValue(PORT0,7 ,1); 
  GPIOSetValue(PORT0,11 ,1); //held in mute state
    

  return;
}

/*****************************************************************************
** Function name:      GPIOSetDir
**
** Descriptions:      Set the direction in GPIO port
**
** parameters:         port num, bit position, direction (1 out, 0 input)
** Returned value:      None
** 
*****************************************************************************/
void GPIOSetDir( uint32_t portNum, uint32_t bitPosi, uint32_t dir )
{
  /* if DIR is OUT(1), but GPIOx_DIR is not set, set DIR
  to OUT(1); if DIR is IN(0), but GPIOx_DIR is set, clr
  DIR to IN(0). All the other cases are ignored. 
  On port3(bit 0 through 3 only), no error protection if 
  bit value is out of range. */
  switch ( portNum )
  {
   case PORT0:
     if ( !(LPC_GPIO0->DIR & (0x1<<bitPosi)) && (dir == 1) )
      LPC_GPIO0->DIR |= (0x1<<bitPosi);
     else if ( (LPC_GPIO0->DIR & (0x1<<bitPosi)) && (dir == 0) )
      LPC_GPIO0->DIR &= ~(0x1<<bitPosi);     
   break;
    case PORT1:
     if ( !(LPC_GPIO1->DIR & (0x1<<bitPosi)) && (dir == 1) )
      LPC_GPIO1->DIR |= (0x1<<bitPosi);
     else if ( (LPC_GPIO1->DIR & (0x1<<bitPosi)) && (dir == 0) )
      LPC_GPIO1->DIR &= ~(0x1<<bitPosi);     
   break;
   case PORT2:
     if ( !(LPC_GPIO2->DIR & (0x1<<bitPosi)) && (dir == 1) )
      LPC_GPIO2->DIR |= (0x1<<bitPosi);
     else if ( (LPC_GPIO2->DIR & (0x1<<bitPosi)) && (dir == 0) )
      LPC_GPIO2->DIR &= ~(0x1<<bitPosi);     
   break;
   case PORT3:
     if ( !(LPC_GPIO3->DIR & (0x1<<bitPosi)) && (dir == 1) )
      LPC_GPIO3->DIR |= (0x1<<bitPosi);
     else if ( (LPC_GPIO3->DIR & (0x1<<bitPosi)) && (dir == 0) )
      LPC_GPIO3->DIR &= ~(0x1<<bitPosi);     
   break;
   default:
     break;
  }
  return;
}

/*****************************************************************************
** Function name:      GPIOSetValue
**
** Descriptions:      Set/clear a bitvalue in a specific bit position
**                  in GPIO portX(X is the port number.)
**
** parameters:         port num, bit position, bit value
** Returned value:      None
** 
*****************************************************************************/
void GPIOSetValue( uint32_t portNum, uint32_t bitPosi, uint32_t bitVal )
{
   /* if bitVal is 1, but GPIOx_DAT is not set, set DATA
  to 1; if bitVal is 0, but GPIOx_DAT is set, clear
  DATA to 0. All the other cases are ignored. On port3(bit 0 
  through 3 only), no error protection if bit value is out of range. */
  switch ( portNum )
  {
   case PORT0:
     if ( !(LPC_GPIO0->DATA & (0x1<<bitPosi)) && (bitVal == 1) )
      LPC_GPIO0->DATA |= (0x1<<bitPosi);
     else if ( (LPC_GPIO0->DATA & (0x1<<bitPosi)) && (bitVal == 0) )
      LPC_GPIO0->DATA &= ~(0x1<<bitPosi);     
   break;
    case PORT1:
     if ( !(LPC_GPIO1->DATA & (0x1<<bitPosi)) && (bitVal == 1) )
      LPC_GPIO1->DATA |= (0x1<<bitPosi);
     else if ( (LPC_GPIO1->DATA & (0x1<<bitPosi)) && (bitVal == 0) )
      LPC_GPIO1->DATA &= ~(0x1<<bitPosi);     
   break;
   case PORT2:
     if ( !(LPC_GPIO2->DATA & (0x1<<bitPosi)) && (bitVal == 1) )
      LPC_GPIO2->DATA |= (0x1<<bitPosi);
     else if ( (LPC_GPIO2->DATA & (0x1<<bitPosi)) && (bitVal == 0) )
      LPC_GPIO2->DATA &= ~(0x1<<bitPosi);     
   break;
   case PORT3:
     if ( !(LPC_GPIO3->DATA & (0x1<<bitPosi)) && (bitVal == 1) )
      LPC_GPIO3->DATA |= (0x1<<bitPosi);
     else if ( (LPC_GPIO3->DATA & (0x1<<bitPosi)) && (bitVal == 0) )
      LPC_GPIO3->DATA &= ~(0x1<<bitPosi);     
   break;
   default:
     break;
  }
  return;
}

/*****************************************************************************
** Function name:      GPIOGetValue
**
** parameters:         port num, bit position, bit value
** Returned value:      None
** 
*****************************************************************************/
unsigned char GPIOGetValue( uint32_t portNum, uint32_t bitPosi)
{
   unsigned char status=0;
    /* if bitVal is 1, but GPIOx_DAT is not set, set DATA to 1; if bitVal is 0, but GPIOx_DAT is set, clear
   DATA to 0. All the other cases are ignored. On port3(bit 0 
   through 3 only), no error protection if bit value is out of range. */
   switch ( portNum )
   {
    case PORT0:
      if ( !(LPC_GPIO0->DATA & (0x1<<bitPosi)) )
       status = 0;
      else if ( (LPC_GPIO0->DATA & (0x1<<bitPosi)))
       status = 1;
    break;
     case PORT1:
      if ( !(LPC_GPIO1->DATA & (0x1<<bitPosi)) )
       status = 0;
      else if ( (LPC_GPIO1->DATA & (0x1<<bitPosi)))
       status = 1;
    break;
    case PORT2:
      if ( !(LPC_GPIO2->DATA & (0x1<<bitPosi)) )
       status = 0;
      else if ( (LPC_GPIO2->DATA & (0x1<<bitPosi)))
       status = 1;
    break;
    case PORT3:
      if ( !(LPC_GPIO3->DATA & (0x1<<bitPosi)) )
       status = 0;
      else if ( (LPC_GPIO3->DATA & (0x1<<bitPosi)))
       status = 1;
    break;
    default:
      break;
   }
   return status;
}





