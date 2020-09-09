#ifndef _LIBATTERY_H_
#define _LIBATTERY_H_
typedef struct
{
	int voltage;
	int current;
}Struct_Usr_LiBattery;

static unsigned char LiPowerRcvInt(unsigned char data);
void __cycLiPowerDeal(void);
void LiBatteryInit(void);
#endif
