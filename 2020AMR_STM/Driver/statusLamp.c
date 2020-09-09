#include "stm32f4xx.h"
#include "statusLamp.h"
#include "timerDev.h"

///////////////////////////////////////////////////硬件///////////////////////////////////////////////////
void GPIO_StatusLamp_Init(void)
{
	//通信指示灯 CPU指示灯
    GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//状态报警指示灯LED1	LED2	LED3
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}
///////////////////////////////////////////////////////////////////////////////////////////////////

Struct_User_StatusLamp statusLamp = {0, 500, 1000, 1, 0};

void SetStatusLamp( unsigned char Light )
{
	if(statusLamp.LastLight != Light )
	{
		statusLamp.LastLight = Light;
		statusLamp.Tick = getSystemTick();
	}
	if( Light == 0 )
	{
		statusLamp.LightType = 0;
		statusLamp.LightNum = 0;
	}
	else if( (Light == 1) ||(Light == 2) ||(Light == 3) )
	{
		statusLamp.LightType = 0;
		statusLamp.LightNum = Light;
	}
	else
	{
		statusLamp.LightType = 1;
		statusLamp.LightNum = Light - 3;
	}
}

void __cycStatusLampDeal(void)
{
	int offsetTime = getSystemTick() - statusLamp.TickBoardLamp ;
	//板级运行指示灯；
	
	statusLamp.BoardFlashTime = 500;
	if( offsetTime >= statusLamp.BoardFlashTime)
	{
		statusLamp.TickBoardLamp = getSystemTick();
		if( statusLamp.BoardLamp != 0 )
		{
			GPIO_ToggleBits(GPIOC, GPIO_Pin_9);
			statusLamp.BoardLamp = 0;
		}
		else
		{
			GPIO_ToggleBits(GPIOC, GPIO_Pin_9);
			statusLamp.BoardLamp = 1;
		}
	}
	
	//球星指示灯；
	
	offsetTime = getSystemTick() - statusLamp.Tick ;
	if( statusLamp.LightType == 0 )
	{
		if(statusLamp.LightNum == 1 )
		{
			GPIO_ResetBits(GPIOD, GPIO_Pin_11);
			GPIO_SetBits(GPIOD, GPIO_Pin_10);
			GPIO_SetBits(GPIOD, GPIO_Pin_9);
		}
		else if(statusLamp.LightNum == 2 )
		{
			GPIO_SetBits(GPIOD, GPIO_Pin_11);
			GPIO_ResetBits(GPIOD, GPIO_Pin_10);
			GPIO_SetBits(GPIOD, GPIO_Pin_9);
		}
		else if(statusLamp.LightNum == 3 )
		{
			GPIO_SetBits(GPIOD, GPIO_Pin_11);
			GPIO_SetBits(GPIOD, GPIO_Pin_10);
			GPIO_ResetBits(GPIOD, GPIO_Pin_9);
		}
		else
		{
			//全灭
			GPIO_SetBits(GPIOD, GPIO_Pin_11);
			GPIO_SetBits(GPIOD, GPIO_Pin_10);
			GPIO_SetBits(GPIOD, GPIO_Pin_9);
		}
	}
	else
	{
		if( offsetTime < statusLamp.LightTime)
		{
			if(statusLamp.LightNum == 1 )
			{
				GPIO_ResetBits(GPIOD, GPIO_Pin_11);
				GPIO_SetBits(GPIOD, GPIO_Pin_10);
				GPIO_SetBits(GPIOD, GPIO_Pin_9);
			}
			else if(statusLamp.LightNum == 2 )
			{
				GPIO_SetBits(GPIOD, GPIO_Pin_11);
				GPIO_ResetBits(GPIOD, GPIO_Pin_10);
				GPIO_SetBits(GPIOD, GPIO_Pin_9);
			}
			else if(statusLamp.LightNum == 3 )
			{
				GPIO_SetBits(GPIOD, GPIO_Pin_11);
				GPIO_SetBits(GPIOD, GPIO_Pin_10);
				GPIO_ResetBits(GPIOD, GPIO_Pin_9);
			}
			else
			{
				//全灭
				GPIO_SetBits(GPIOD, GPIO_Pin_11);
				GPIO_SetBits(GPIOD, GPIO_Pin_10);
				GPIO_SetBits(GPIOD, GPIO_Pin_9);
			}
		}
		else
		{
			//全灭
			GPIO_SetBits(GPIOD, GPIO_Pin_11);
			GPIO_SetBits(GPIOD, GPIO_Pin_10);
			GPIO_SetBits(GPIOD, GPIO_Pin_9);
			if( offsetTime >= (statusLamp.LightTime + statusLamp.UnLightTime) )
			{
				statusLamp.Tick = getSystemTick();
			}
		}
	}
}


void commLampDeal(void)
{
	GPIO_ToggleBits(GPIOC, GPIO_Pin_8);
}

void StatusLampInit(void)
{	
	statusLamp.Tick = 0;
	statusLamp.LightTime = 500;
	statusLamp.UnLightTime = 500;
	statusLamp.LightNum = 0;
	statusLamp.LightType = 0;
	statusLamp.LastLight = 0;
	
	statusLamp.TickBoardLamp = 0;
	statusLamp.BoardLamp = 0;
	statusLamp.BoardFlashTime = 500;
	GPIO_StatusLamp_Init();
}
