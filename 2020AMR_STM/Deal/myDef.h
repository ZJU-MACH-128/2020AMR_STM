#ifndef _MYDEF_H
#define _MYDEF_H

#include "ringbuf.h"

#define ERROR_RETURN_VALUE       ((unsigned char) -1)
#define WARRNING_RETURN_VALUE    ((unsigned char)  1)
#define OK_RETURN_VALUE          ((unsigned char)  0)
	
typedef struct
{
	__int64 Tick;
	struct FIFO8 LiBatteryFIFO;
	unsigned int step;
	unsigned int dataTotal;
	unsigned int dataIndex;
	unsigned int type;
	unsigned int check;
	unsigned char dataBuffer[256];
	unsigned char DMABuffer[256];
	unsigned char DMADataSize;
	unsigned char DMABusy;
}Struct_Drv_Uart;

	
typedef struct
{
    unsigned int Encoder_data1;		//���һȦ����������
    unsigned int Ratio_data1;		//���ٱȣ���̥һȦ���ת��Ȧ��
    unsigned int LCJ_data1;			//��ֱ̥������
    unsigned int Tyre_diameter_data;//��ֱ̥��������
	unsigned int Wheel_LR_data;		//��������֮��ࣺ����
	unsigned int Wheel_FB_data;		//ǰ������֮��ࣺ����	
	double data_r_e1;
	double data_r_e2;
}Struct_Robot_Size;

extern Struct_Robot_Size robotSize;

#endif
