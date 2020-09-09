#include "stm32f4xx.h"
#include "rearRadar.h"
#include "mydef.h"
#include "baseRobot.h"
#include "timerDev.h"
///////////////////////////////////////////////////硬件///////////////////////////////////////////////////
//USART1
//TX:PA9
//RX:PA10
//初始化串口
void Usart1_Init(int bandrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	USART_DeInit(USART1);
	GPIO_PinAFConfig(GPIOA,  GPIO_PinSource9,  GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,  GPIO_PinSource10, GPIO_AF_USART1);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate=bandrate;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART1,&USART_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART1,ENABLE);
}		  

//发送，接收中断
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //接收到数据
	{
		rearRadarRcvInt(USART_ReceiveData(USART1));
		USART_ClearITPendingBit(USART1,  USART_IT_RXNE);
	}
	else if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{
		USART_ClearITPendingBit(USART1,  USART_IT_TXE);
	}
}  

//目前用等待发送完成;
static unsigned char rearRadarSendData(unsigned char *buf,unsigned int len)
{
	int i=0;
	if(!sizeof(buf)) return ERROR_RETURN_VALUE;
	while(i!=len)
	{
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)!=SET);
		USART_SendData(USART1,buf[i]);
		i++;
	}
	return OK_RETURN_VALUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
static Struct_Drv_Uart UartRearRadar;

// 触发超声探头不断发送数据
static unsigned char rearRadarTrigger(void)
{
	unsigned char cmd[1]={0xb3};
	rearRadarSendData(cmd,sizeof(cmd));
	rearRadarSendData(cmd,sizeof(cmd));
	return OK_RETURN_VALUE;
}

static int rearRadaravailable(void)
{
	return fifo8_status(&UartRearRadar.LiBatteryFIFO);
}

static unsigned char rearRadarRcvInt(unsigned char data)
{
	fifo8_putPut(&UartRearRadar.LiBatteryFIFO,data);
	return 1;
}

static unsigned char rearRadarRead(void)
{
	return fifo8_get(&UartRearRadar.LiBatteryFIFO);
}

Struct_Usr_RearRadar rearRadar;
void __cycSonarDeal(void)
{
	unsigned char SonarData;
	//触发激活超声波模块；
	if( (getSystemTick() - UartRearRadar.Tick )>=1000)
	{
		//rearRadarTrigger();
		UartRearRadar.Tick = getSystemTick();
	}
	//超声波有数据收到；
	if (rearRadaravailable() > 0) 
	{
		// 从串口缓存读取数据
		SonarData = rearRadarRead();
		if(UartRearRadar.step == 0)
		{
			if(SonarData == 0x55)	{ UartRearRadar.step = 1;}
			else					{ UartRearRadar.step = 0;}
		}
		else if(UartRearRadar.step == 1)
		{
			if(SonarData == 0x54)	{ UartRearRadar.step = 2;}
			else					{ UartRearRadar.step = 0;}
		}
		else if(UartRearRadar.step == 2)
		{
			if(SonarData == 0x8)	
			{ 
				UartRearRadar.dataIndex = 0;
				UartRearRadar.dataTotal = 0x8;
				UartRearRadar.step = 3;
			}
			else					
			{ 
				UartRearRadar.step = 0;
			}
		}
		else if(UartRearRadar.step == 3)
		{
			//数据信息
            UartRearRadar.dataBuffer[UartRearRadar.dataIndex] = SonarData;
            UartRearRadar.dataIndex++;
            if(UartRearRadar.dataIndex == UartRearRadar.dataTotal)
            {
                UartRearRadar.step = 4;
            }
		}
		else if(UartRearRadar.step == 4)
		{
			if(SonarData == 0xff)	{ UartRearRadar.step = 5;}
			else					{ UartRearRadar.step = 0;}
		}
		else if(UartRearRadar.step == 5)
		{
			UartRearRadar.step = 6;
		}
		else if(UartRearRadar.step == 6)
		{
			UartRearRadar.step = 7;
		}
		else if(UartRearRadar.step == 7)
		{
			//收到一帧正确的数据；
			//拷贝串口数据到用户数据；
			rearRadarFrameCallback((void *)UartRearRadar.dataBuffer);
			UartRearRadar.Tick = getSystemTick();
			UartRearRadar.step = 0;
		}
	}
}

static unsigned char rearRadarFIFORcvData[1024];
void SonarPartInit(void)
{	
	fifo8_init(&UartRearRadar.LiBatteryFIFO,sizeof(rearRadarFIFORcvData), rearRadarFIFORcvData);
	Usart1_Init(115200);
	rearRadarTrigger();
	UartRearRadar.Tick = getSystemTick();
}
