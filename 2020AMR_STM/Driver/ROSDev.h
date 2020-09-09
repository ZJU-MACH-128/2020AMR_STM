#ifndef _ROSDEV_H_
#define _ROSDEV_H_

typedef struct
{
	unsigned char cmd;
}Struct_Usr_mainController;
static unsigned char mainControllerRcvInt(unsigned char data);
static void mainControlleSendOver(void);
void MainControllerInit(void);
void __cycMainControllerDeal(void);
int mainControlleSendData(unsigned char *data ,int len);
#endif
