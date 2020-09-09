#ifndef _BUTTON_H_
#define _BUTTON_H_
typedef	struct
{
	unsigned int Cur_Sta1;
	unsigned int TriFlag1;
	unsigned int Filter1;
	unsigned int Cur_Sta2;
	unsigned int TriFlag2;
	unsigned int Filter2;
	unsigned int Cur_Sta3;
	unsigned int TriFlag3;
	unsigned int Filter3;
}SensorChk;

void ButtonInit(void);
#endif
