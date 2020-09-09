#ifndef	__GPIO_DRV__
#define	__GPIO_DRV__

void InitPinAllMode(void);
void pinMode(unsigned char pin, unsigned int mode);
void digitalWrite(unsigned char pin, unsigned char value);
	
#endif