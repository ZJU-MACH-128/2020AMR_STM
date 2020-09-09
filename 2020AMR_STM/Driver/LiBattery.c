#include "stm32f4xx.h"
#include "LiBattery.h"
#include "mydef.h"
#include "baseRobot.h"
#include "timerDev.h"
///////////////////////////////////////////////////硬件///////////////////////////////////////////////////
//USART3
//TX:PB10
//RX:PB11
//初始化串口
void Usart3_Init(int bandrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	USART_DeInit(USART3);
	GPIO_PinAFConfig(GPIOB,  GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB,  GPIO_PinSource11, GPIO_AF_USART3);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate=bandrate;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART3,&USART_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel=USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART3,ENABLE);
}		  

//发送，接收中断
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收到数据
	{
		LiPowerRcvInt(USART_ReceiveData(USART3));
		USART_ClearITPendingBit(USART3,  USART_IT_RXNE);
	}
	else if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
	{
		USART_ClearITPendingBit(USART3,  USART_IT_TXE);
	}
}  

//目前用等待发送完成;
static unsigned char LiPowerSendData(unsigned char *buf,unsigned int len)
{
	int i=0;
	if(!sizeof(buf)) return ERROR_RETURN_VALUE;
	while(i!=len)
	{
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)!=SET);
		USART_SendData(USART3,buf[i]);
		i++;
	}
	return OK_RETURN_VALUE;
}
///////////////////////////////////////////////////////////////////////////////////////////////////

static Struct_Drv_Uart UartLiBattery;

//发送读取设备信息的请求数据
static unsigned char LiPowerTrigger(void)
{
	unsigned char cmd[]={0xDD ,0xA5 ,0x03 ,0x00 ,0xFF ,0xFD ,0x77};
	LiPowerSendData( cmd,7 );
	return OK_RETURN_VALUE;
}

//缓冲区

//查看接收缓冲区是否有数据
static int LiPoweravailable(void)
{
	return fifo8_status(&UartLiBattery.LiBatteryFIFO);
}

//串口接收到数据放入缓冲区
static unsigned char LiPowerRcvInt(unsigned char data)
{
	fifo8_putPut(&UartLiBattery.LiBatteryFIFO,data);
	return 1;
}

static unsigned char LiPowerRead(void)
{
	unsigned char getbuf;
	if(fifo8_status(&UartLiBattery.LiBatteryFIFO)!=0)
	{
		return fifo8_get(&UartLiBattery.LiBatteryFIFO);
	}
	return -1;
}

void __cycLiPowerDeal(void)
{
	unsigned char SonarData;
	//触发读取电池信息的请求
	if( (getSystemTick() - UartLiBattery.Tick )>=1000)
	{
		LiPowerTrigger();
		UartLiBattery.Tick = getSystemTick();
	}
	//电池串口号有数据收到；
	if (LiPoweravailable() > 0) 
	{
		// 从串口缓存读取数据
		SonarData = LiPowerRead();
		if(UartLiBattery.step == 0)
		{
			if(SonarData == 0xDD)	{ UartLiBattery.step = 1;}
			else					{ UartLiBattery.step = 0;}
		}
		else if(UartLiBattery.step == 1)
		{
			if(SonarData == 0x03)	{ UartLiBattery.step = 2;}
			else if(SonarData == 0xa5)	{ UartLiBattery.step = 2;}
			else					{ UartLiBattery.step = 0;}
		}
		else if(UartLiBattery.step == 2)
		{
			if(SonarData == 0x00)	{ UartLiBattery.step = 3;}
			else					{ UartLiBattery.step = 0;}
		}
		else if(UartLiBattery.step == 3)
		{
			UartLiBattery.dataIndex = 0;
			UartLiBattery.dataTotal = SonarData;
			UartLiBattery.step = 4;
		}
		else if(UartLiBattery.step == 4)
		{
			//数据信息
            UartLiBattery.dataBuffer[UartLiBattery.dataIndex] = SonarData;
            UartLiBattery.dataIndex++;
            if(UartLiBattery.dataIndex == UartLiBattery.dataTotal)
            {
                UartLiBattery.step = 5;
            }
		}
		else if(UartLiBattery.step == 5 )
		{
			//校验码高位
			UartLiBattery.step = 6;
		}
		else if(UartLiBattery.step == 6 )
		{
			//校验码地位
			UartLiBattery.step = 7;
		}
		else if(UartLiBattery.step == 7)
		{
			//收到一帧正确的数据；
			//拷贝串口数据到用户数据；
			LiBatteryFrameCallback((void *)UartLiBattery.dataBuffer);
			UartLiBattery.Tick = getSystemTick();
			UartLiBattery.step = 0;
		}
	}
}

static unsigned char LiBatteryFIFORcvData[1024];
void LiBatteryInit(void)
{
	fifo8_init(&UartLiBattery.LiBatteryFIFO,sizeof(LiBatteryFIFORcvData), LiBatteryFIFORcvData);
	Usart3_Init(9600);
	LiPowerTrigger();
	UartLiBattery.Tick = getSystemTick();
}
