#include "stm32f4xx.h"
#include "timerDev.h"

///////////////////////////////////////////////////硬件///////////////////////////////////////////////////
void TIM6_init( void )
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure ;
    NVIC_InitTypeDef NVIC_InitStructure; 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6 , ENABLE);                     
    //**************************************************************************
    //     定时器2设置： 720分频，1000us中断一次，向上计数
    //**************************************************************************
    TIM_TimeBaseStructure.TIM_Period = 999;
    TIM_TimeBaseStructure.TIM_Prescaler = 82-1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM6, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM6_DAC_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM6,TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
		timer.Tick++;
		if(timer.TimesDelay != 0 )
		{
			timer.TimesDelay--;
		}
		canOpenCount();
		zeroTimer();
		if( timer.p1 != 0 )		timer.p1();
		if( timer.p2 != 0 )		timer.p2();
		if( timer.p3 != 0 )		timer.p3();
		timer.Times10MS++;
		if(timer.Times10MS>=10)
		{
			timer.Times10MS = 0;
			if( timer.p1_ != 0 )		timer.p1_();
			if( timer.p2_ != 0 )		timer.p2_();
			if( timer.p3_ != 0 )		timer.p3_();
		}
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////

Struct_User_Timer timer;

void Delayms(unsigned int t)
{
	timer.TimesDelay=t;
	while(timer.TimesDelay!=0);
}

__int64 getSystemTick(void)
{
	return timer.Tick;
}

int setTimerCallback( void (*p) (void) )
{
	if(timer.p1 == 0 )	{timer.p1 = p ; return 1;}
	if(timer.p2 == 0 )	{timer.p2 = p ; return 1;}
	if(timer.p3 == 0 )	{timer.p3 = p ; return 1;}
}

int setTimerCallback10( void (*p) (void) )
{
	if(timer.p1_ == 0 )	{timer.p1_ = p ; return 1;}
	if(timer.p2_ == 0 )	{timer.p2_ = p ; return 1;}
	if(timer.p3_ == 0 )	{timer.p3_ = p ; return 1;}
}

void timerDevInit(void)
{	
	timer.Tick = 0;
	timer.Times10MS = 0;
	timer.TimesDelay = 0;
	
	timer.p1 = 0;
	timer.p2 = 0;
	timer.p3 = 0;
	timer.p4 = 0;
	timer.p5 = 0;
	
	timer.p1_ = 0;
	timer.p2_ = 0;
	timer.p3_ = 0;
	timer.p4_ = 0;
	timer.p5_ = 0;
	TIM6_init();
}
