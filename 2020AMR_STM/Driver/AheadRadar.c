#include "stm32f4xx.h"
#include "AheadRadar.h"
#include "timerDev.h"
#include "baseRobot.h"
//PA[5]	PA[4]	PA[3]	PA[2]	
SensorPBS Sensor_PBS;
void AheadRadarGpioInit(void)
{    
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_4 | GPIO_Pin_3 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	Sensor_PBS.Cur_Stax = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2);
	Sensor_PBS.Cur_Sta1 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5);
	Sensor_PBS.Cur_Sta2 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);
	Sensor_PBS.Cur_Sta3 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3);
}

#define	FILTER_TIMES	2
static void __timerPBS(void)
{	
	//故障					PA	2	故障时0
	//预警区域（外）		PA	5	进入时0
	//预警区域（中）		PA	4	进入时0
	//预警区域（内）		PA	3	进入时0
	//0灯亮起
	unsigned char sta;
	
	sta = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2);
	if(Sensor_PBS.Cur_Stax!=sta)
	{
		Sensor_PBS.Filterx++;
		if(Sensor_PBS.Filterx>=FILTER_TIMES)
		{
			Sensor_PBS.Filterx=0;
			Sensor_PBS.TriFlagx =1;
			Sensor_PBS.Cur_Stax=sta;
		}	
	}
	else
	{
		Sensor_PBS.Filterx=0;
	}
	
	sta = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3);
	if(Sensor_PBS.Cur_Sta1!=sta)
	{
		Sensor_PBS.Filter1++;
		if(Sensor_PBS.Filter1>=FILTER_TIMES)
		{
			Sensor_PBS.Filter1=0;
			Sensor_PBS.TriFlag1 =1;
			Sensor_PBS.Cur_Sta1=sta;
		}	
	}
	else
	{
		Sensor_PBS.Filter1=0;
	}
	sta = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);
	if(Sensor_PBS.Cur_Sta2!=sta)
	{
		Sensor_PBS.Filter2++;
		if(Sensor_PBS.Filter2>=FILTER_TIMES)
		{
			Sensor_PBS.Filter2=0;
			Sensor_PBS.TriFlag2 =1;
			Sensor_PBS.Cur_Sta2=sta;
		}	
	}
	else
	{
		Sensor_PBS.Filter2=0;
	}
	sta = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5);
	if(Sensor_PBS.Cur_Sta3!=sta)
	{
		Sensor_PBS.Filter3++;
		if(Sensor_PBS.Filter3>=FILTER_TIMES)
		{
			Sensor_PBS.Filter3=0;
			Sensor_PBS.TriFlag3 =1;
			Sensor_PBS.Cur_Sta3=sta;
		}	
	}
	else
	{
		Sensor_PBS.Filter3=0;
	}
	if( Sensor_PBS.Cur_Stax == 0 )
	{
		//有设备故障信号
		AheadRadarFrameCallback(0);
		return ;
	}
	else if( Sensor_PBS.Cur_Sta1 == 0 )
	{
		//
		AheadRadarFrameCallback(1);
		return ;
	}
	else if( Sensor_PBS.Cur_Sta2 == 0 )
	{
		AheadRadarFrameCallback(2);
		return ;
	}
	else if( Sensor_PBS.Cur_Sta3 == 0 )
	{
		AheadRadarFrameCallback(3);
		return ;
	}
	else
	{
		//安全区域
		AheadRadarFrameCallback(0xff);
		return ;
	}
}

void AheadRadarInit(void)
{
	AheadRadarGpioInit();
	setTimerCallback10( __timerPBS );
}
