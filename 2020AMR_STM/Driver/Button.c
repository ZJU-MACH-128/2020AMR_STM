#include "stm32f4xx.h"
#include "Button.h"
#include "timerDev.h"
#include "baseRobot.h"
//PG1[¼±Í£] 	PG0[Æô¶¯]	PF15[Í£Ö¹]
SensorChk Sensor_Start;
void ButtontGpioInit(void)
{    
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	
	Sensor_Start.Cur_Sta1=GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_1);
	Sensor_Start.Cur_Sta2=GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_0);
	Sensor_Start.Cur_Sta3=GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_15);
}
#define	FILTER_TIMES	10
static void __timerButton(void)
{	
	
	//¼±Í£°´ÏÂ¡¾0¡¿
	//Æô¶¯Í£Ö¹°´ÏÂ¡¾1¡¿
	unsigned char sta;
	sta = GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_1);
	//if(Sensor_Start.Cur_Sta1!=sta)
	{
		Sensor_Start.Filter1++;
		if(Sensor_Start.Filter1>=FILTER_TIMES)
		{
			Sensor_Start.Filter1=0;
			Sensor_Start.TriFlag1 =1;
			Sensor_Start.Cur_Sta1=sta;
			if( Sensor_Start.Cur_Sta1 == 0 )
			{
				ButtonFrameCallback(0, 1 );
			}
			else
			{
				ButtonFrameCallback(0, 0 );
			}
		}	
	}
	//else
	//{
	//	Sensor_Start.Filter1=0;
	//}
	
	sta = GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_0);
	if(Sensor_Start.Cur_Sta2!=sta)
	{
		Sensor_Start.Filter2++;
		if(Sensor_Start.Filter2>=FILTER_TIMES)
		{
			Sensor_Start.Filter2=0;
			Sensor_Start.TriFlag2 =1;
			Sensor_Start.Cur_Sta2=sta;
			if( Sensor_Start.Cur_Sta2 == 1 )
			{
				ButtonFrameCallback(1, 1 );
			}
			else
			{
				;//ButtonFrameCallback(1, 0 );
			}
		}	
	}
	else
	{
		Sensor_Start.Filter2=0;
	}
	sta = GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_15);
	if(Sensor_Start.Cur_Sta3!=sta)
	{
		Sensor_Start.Filter3++;
		if(Sensor_Start.Filter3>=FILTER_TIMES)
		{
			Sensor_Start.Filter3=0;
			Sensor_Start.TriFlag3 =1;
			Sensor_Start.Cur_Sta3=sta;
			if( Sensor_Start.Cur_Sta3 == 1 )
			{
				ButtonFrameCallback(2, 1 );
			}
			else
			{
				;//ButtonFrameCallback(2, 0 );
			}
		}	
	}
	else
	{
		Sensor_Start.Filter3=0;
	}
}

void ButtonInit(void)
{
	ButtontGpioInit();
	setTimerCallback10( __timerButton );
}
