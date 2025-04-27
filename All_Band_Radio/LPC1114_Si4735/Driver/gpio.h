#ifndef GPIO_H 
#define GPIO_H

#define PORT0      0
#define PORT1      1
#define PORT2      2
#define PORT3      3

void GPIOInit( void );
void GPIOSetDir( uint32_t portNum, uint32_t bitPosi, uint32_t dir );
void GPIOSetValue( uint32_t portNum, uint32_t bitPosi, uint32_t bitVal );
unsigned char GPIOGetValue( uint32_t portNum, uint32_t bitPosi);

#endif /* end __GPIO_H */
