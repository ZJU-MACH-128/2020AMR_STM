#include "param.h"
#include "stm32f4xx_flash.h"

Struct_SysParam SysParam;
Struct_RunParam RunParam;
//#define	Tyre_diameter_data	330
//#define	LCJ_data1			0
//#define	Encoder_data1		(96*512*2)
//#define	Ratio_data1			1
//#define	Ratio_Encoder		1

unsigned char InitSysParam(void)
{
	SysParam.wheelDiameter=330;
	SysParam.wheelEncoder=(96*512*2);
	SysParam.offsetRotate[0]=0x1f17;
	SysParam.offsetRotate[1]=0x2106;
	SysParam.offsetRotate[2]=0x209d;
	SysParam.offsetRotate[3]=0x2179;
}

void countRunParam(void)
{
    //RunParam.data_r_e1=(1/3.14*Ratio_Encoder/1000*10)/(0.1*Tyre_diameter_data)*Encoder_data1*Ratio_data1;
    //RunParam.data_r_e2=0.713565*Tyre_diameter_data*(LCJ_data1*0.01+1)/(Encoder_data1*Ratio_data1);
    RunParam.data_r_e1=(1/3.14*1/1000*10)/(0.1*SysParam.wheelDiameter)*SysParam.wheelEncoder*1;
    //RunParam.data_r_e2=0.713565*SysParam.wheelDiameter*(0*0.01+1)/(SysParam.wheelEncoder*1);
    //RunParam.data_r_e1=SysParam.wheelEncoder*10/(3.14*SysParam.wheelDiameter);
    RunParam.data_r_e2=(3.14*SysParam.wheelDiameter)/(SysParam.wheelEncoder*1);
}
unsigned char readSysParam(void)
{
	unsigned char ret = 0;
	Struct_SysFile SysFile;
	//初始化参数;
	InitSysParam();
	//从FLASH中读取参数;
	memcpy( (unsigned char *)&SysFile,(unsigned char *)PARAM_ADDR_OFFSET,sizeof(Struct_SysFile));
	if((SysFile.head[0] == 'Z')&&(SysFile.head[1] == 'J')&&(SysFile.head[2] == 'U')&&(SysFile.head[3] == 'M')&& \
		(SysFile.head[4] == 'S')&&(SysFile.head[5] == 'Y')&&(SysFile.head[6] == 'S')&&(SysFile.head[7] == 'F') )
	{
		int i;
		unsigned char check = 0;
		unsigned char *pParam = (unsigned char *)&SysFile.param;
		int len = sizeof(Struct_SysParam);
		for(i =0;i<len;i++)
		{
			check = check+pParam[i];
		}
		check = 0xff - check;
		if((SysFile.check[0] == check)&&(SysFile.check[1] == check)&&(SysFile.check[2] == check)&&(SysFile.check[3] == check)&& \
			(SysFile.check[4] == check)&&(SysFile.check[5] == check)&&(SysFile.check[6] == check)&&(SysFile.check[7] == check) )
		{
			memcpy( (unsigned char *)&SysParam,(unsigned char *)&SysFile.param,sizeof(Struct_SysParam));
			ret = 1;
		}
	}
	countRunParam();
	return ret;
}

unsigned char writeSysParam(void)
{
	Struct_SysFile SysFile;
	//复制参数到中间变量；
	SysFile.head[0] = 'Z';
	SysFile.head[1] = 'J';
	SysFile.head[2] = 'U';
	SysFile.head[3] = 'M';
	SysFile.head[4] = 'S';
	SysFile.head[5] = 'Y';
	SysFile.head[6] = 'S';
	SysFile.head[7] = 'F';
	memcpy( (unsigned char *)&SysFile.param,(unsigned char *)&SysParam,sizeof(Struct_SysParam));
	if(1)
	{
		int i;
		unsigned char check = 0;
		unsigned char *pParam = (unsigned char *)&SysParam;
		int len = sizeof(Struct_SysParam);
		for(i =0;i<len;i++)
		{
			check = check+pParam[i];
		}
		check = 0xff - check;
		
		SysFile.check[0] = check;
		SysFile.check[1] = check;
		SysFile.check[2] = check;
		SysFile.check[3] = check;
		SysFile.check[4] = check;
		SysFile.check[5] = check;
		SysFile.check[6] = check;
		SysFile.check[7] = check;
	}
	//写入FLASH
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
				  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

	FLASH_EraseSector(FLASH_Sector_9, VoltageRange_3);
	
	if(1)
	{
		int i;
		unsigned int den=PARAM_ADDR_OFFSET;
		unsigned int *pc = (unsigned int *)&SysFile;
		unsigned int *pd;
		for(i=0;i<((sizeof(Struct_SysFile)/4)+1);i++)
		{
			pd = &pc[i];
			FLASH_ProgramWord(den, *pd);
			den=den+4;
		}
	}
	countRunParam();
	return 1;
}
