#include "mycanopen.h"
#include "dealMotec.h"
#include "LinerBuf.h"
#include "motec.h"


//����CAN��������
//ʹ��SOD��Ӧ��ģʽ��
unsigned char configDeviceMotec(unsigned char id)
{
	unsigned char ret = 0;
	unsigned int getParam;
	//if(id == 1 )id	=1;
	//if(id == 3 )id	=1;
	//if(id == 4 )id	=1;
	///////////////////////////////////
//	ret = canOpenWriteSODAnser(id,0x6092,0,2,0,&getParam);
//	if(ret == 0 )return 0;
//	ret = canOpenWriteSODAnser(id,0x6093,0,2,0,&getParam);
//	if(ret == 0 )return 0;
//	
//	ret = canOpenWriteSODAnser(id,0x6060,0,2,4,&getParam);
//	if(ret == 0 )return 0;
//	ret = canOpenWriteSODAnser(id,0x6073,0,2,3000,&getParam);
//	if(ret == 0 )return 0;
//	ret = canOpenWriteSODAnser(id,0x6077,0,2,8000,&getParam);
//	if(ret == 0 )return 0;
	///////////////////////////////////
	//�����
	ret = canOpenWriteSODAnser(id,0x6075,0,2,6000,&getParam);
	if(ret == 0 )return 0;
	
	//��������ʱ��;
	ret = canOpenWriteSODAnser(id,0x1017,0,2,2500,&getParam);
	if(ret == 0 )return 0;
	
	//���ü��ٶȣ�
	ret = canOpenWriteSODAnser(id,0x6082,0,2,10,&getParam);
	if(ret == 0 )return 0;
	
	//���ü��ٶȣ�
	ret = canOpenWriteSODAnser(id,0x6083,0,2,100,&getParam);
	if(ret == 0 )return 0;
	
	//���÷�ת���ٶȣ�
	ret = canOpenWriteSODAnser(id,0x6084,0,2,5,&getParam);
	if(ret == 0 )return 0;
	
	//�������ʹ�ܣ�
	ret = canOpenWriteSODAnser(id,MOTEC_DEVICECTRL,0,2,6,&getParam);
	if(ret == 0 )return 0;
	
	//�������ʹ�ܣ�
	ret = canOpenWriteSODAnser(id,MOTEC_DEVICECTRL,0,2,7,&getParam);
	if(ret == 0 )return 0;
	
	//����ٶ�
	ret = canOpenWriteSODAnser(id,MOTEC_VEL_DEMAND_VALUE,0,2,0x00,&getParam);
	if(ret == 0 )return 0;
	
	//PDO���ղ���������
	/*ret = canOpenWriteSODAnser(id,0x1400,2,1,0xfe,&getParam);
	if(ret == 0 )return 0;
	ret = canOpenWriteSODAnser(id,0x1600,1,4,0x60400010,&getParam);
	if(ret == 0 )return 0;
	ret = canOpenWriteSODAnser(id,0x1600,2,4,0x60FF0010,&getParam);
	if(ret == 0 )return 0;
	ret = canOpenWriteSODAnser(id,0x1600,0,1,2,&getParam);
	if(ret == 0 )return 0;*/
	
	//PDO���Ͳ���������
	/*ret = canOpenWriteSODAnser(id,0x1800,2,1,0xff,&getParam);
	if(ret == 0 )return 0;
	ret = canOpenWriteSODAnser(id,0x1800,3,2,0x0a,&getParam);
	if(ret == 0 )return 0;
	ret = canOpenWriteSODAnser(id,0x1800,5,2,0x0a,&getParam);
	if(ret == 0 )return 0;
	
	ret = canOpenWriteSODAnser(id,0x1a00,1,4,0x60410010,&getParam);
	if(ret == 0 )return 0;
	ret = canOpenWriteSODAnser(id,0x1a00,2,4,0x606C0010,&getParam);
	if(ret == 0 )return 0;
	ret = canOpenWriteSODAnser(id,0x1a00,2,4,0x60630020,&getParam);
	if(ret == 0 )return 0;
	ret = canOpenWriteSODAnser(id,0x1a00,0,1,3,&getParam);
	if(ret == 0 )return 0;*/
	
	ret = canOpenWriteSODAnser(id,0x1800,2,1,0xff,&getParam);
	if(ret == 0 )return 0;
	ret = canOpenWriteSODAnser(id,0x1800,3,2,0x0a,&getParam);
	if(ret == 0 )return 0;
	ret = canOpenWriteSODAnser(id,0x1800,5,2,0x14,&getParam);
	if(ret == 0 )return 0;
	
	ret = canOpenWriteSODAnser(id,0x1a00,1,4,0x60410010,&getParam);
	if(ret == 0 )return 0;
	ret = canOpenWriteSODAnser(id,0x1a00,2,4,0x606C0010,&getParam);
	if(ret == 0 )return 0;
	ret = canOpenWriteSODAnser(id,0x1a00,3,4,0x60630020,&getParam);
	if(ret == 0 )return 0;
	ret = canOpenWriteSODAnser(id,0x1a00,0,1,3,&getParam);
	if(ret == 0 )return 0;
	
	
	ret = canOpenWriteSODAnser(id,0x1801,2,1,0xff,&getParam);
	if(ret == 0 )return 0;
	ret = canOpenWriteSODAnser(id,0x1801,3,2,0x0a,&getParam);
	if(ret == 0 )return 0;
	ret = canOpenWriteSODAnser(id,0x1801,5,2,0x03e8,&getParam);
	if(ret == 0 )return 0;
	
	ret = canOpenWriteSODAnser(id,0x1a01,1,4,0x200b0020,&getParam);
	if(ret == 0 )return 0;
	ret = canOpenWriteSODAnser(id,0x1a01,2,4,0x60780010,&getParam);
	if(ret == 0 )return 0;
	ret = canOpenWriteSODAnser(id,0x1a01,0,1,2,&getParam);
	if(ret == 0 )return 0;
}
