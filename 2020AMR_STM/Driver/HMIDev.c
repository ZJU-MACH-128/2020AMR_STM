#include "stm32f4xx.h"
#include "mydef.h"
#include "HMIDev.h"
#include "timerDev.h"
#include "baseRobot.h"
///////////////////////////////////////////////////硬件///////////////////////////////////////////////////
//USART2
//TX:PD5
//RX:PD6
//初始化串口
void Usart2_Init(int bandrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_DeInit(USART2);
	GPIO_PinAFConfig(GPIOD,  GPIO_PinSource5,  GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD,  GPIO_PinSource6,  GPIO_AF_USART2);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	USART_InitStructure.USART_BaudRate=bandrate;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART2,&USART_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel=USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART2,ENABLE);
}

void USART2_IRQHandler(void)
{  
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		HMIDevRcvInt(USART_ReceiveData(USART2));
		USART_ClearITPendingBit(USART2,  USART_IT_RXNE);
	}
	else if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{
		USART_ClearITPendingBit(USART2,  USART_IT_TXE);
	}
}  
/*
unsigned char HMIDevSendData(unsigned char *buf,unsigned short len)
{
	int i=0;
	while(i!=len)
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)!=SET);
		USART_SendData(USART2,buf[i++]);
	}
}
*/
void UART2_DMA_Init(unsigned char *dmabuffer)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Stream6); 

    DMA_InitStructure.DMA_Channel             = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)(&(USART2->DR));
    DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t)dmabuffer;
    DMA_InitStructure.DMA_DIR                 = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize          = 0;               
    DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize  = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority            = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Disable;
	
    DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single;

    DMA_Init(DMA1_Stream6, &DMA_InitStructure);

    DMA_ITConfig(DMA1_Stream6, DMA_IT_TC, ENABLE);

    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel                   = DMA1_Stream6_IRQn;           
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1; 
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
               
	DMA_ClearFlag(DMA1_Stream6,DMA_FLAG_TCIF6);
    DMA_Cmd(DMA1_Stream6, DISABLE);
}

void DMA1_Stream6_IRQHandler(void)
{         
	if(DMA_GetITStatus(DMA1_Stream6,DMA_IT_TCIF6) != RESET)   
    {  
        DMA_ClearFlag(DMA1_Stream6,DMA_IT_TCIF6); 
		HMIDevSendOver();
    }  
}

unsigned char HMIDevEnableDMA(unsigned short len)
{
	DMA_SetCurrDataCounter(DMA1_Stream6,len); 
	DMA_Cmd(DMA1_Stream6,ENABLE); 
}

///////////////////////////////////////////////////////////////////////////////////////////////////
static Struct_Drv_Uart UartHMIDev;
static void HMIDevSendOver(void)
{
	UartHMIDev.DMABusy = 0;
}
int HMIDevSendData(unsigned char *data ,int len)
{
	if(UartHMIDev.DMABusy == 0 )
	{
		UartHMIDev.DMABusy = 1;
		memcpy( UartHMIDev.DMABuffer, data, len );
		HMIDevEnableDMA(len);
		return 0;
	}
	return 1;
}
static unsigned char HMIDevTrigger(void)
{	unsigned int BatteryVoltage;			//电池电压
	unsigned int BatteryCurrent;			//电池电流
	unsigned int BatteryRemainCapacity;		//电池剩余容量
	unsigned int BatteryPercentage;			//电池剩余容量百分比
	unsigned int BatteryStatus;				//电池状态
	unsigned int BatteryDevError;			//电池接收故障
	/*
	unsigned char cmd[1]={0xb3};
	HMIDevSendData( cmd, sizeof( cmd ) );
	*/
	unsigned char temp[128];
	int index = 0;
	int len = 0;
	
	memset( temp, 0, sizeof(temp));
	
	temp[index] = 0XEE;	index++;
	temp[index] = 0XB1;	index++;
	temp[index] = 0X12;	index++;
	
	temp[index] = 0X00;	index++;//界面号
	temp[index] = 0X04;	index++;
	
	//
	temp[index] = 0X00;	index++;//控件号
	temp[index] = 0X02;	index++;
	
	temp[index] = 0X00;	index++;//长度
	temp[index] = 0X00;	index++;
	
	sprintf(&temp[index],"%d%c",robotDeal.BatteryVoltage,0);
	len = strlen(&temp[index]);
	temp[index-1] = len;	
	index = index + len;
	
	//
	temp[index] = 0X00;	index++;//控件号
	temp[index] = 0X03;	index++;
	
	temp[index] = 0X00;	index++;//长度
	temp[index] = 0X00;	index++;
	
	sprintf(&temp[index],"%d%c",robotDeal.BatteryCurrent,0);
	len = strlen(&temp[index]);
	temp[index-1] = len;	
	index = index + len;
	//
	temp[index] = 0X00;	index++;//控件号
	temp[index] = 0X04;	index++;
	
	temp[index] = 0X00;	index++;//长度
	temp[index] = 0X00;	index++;

	sprintf(&temp[index],"%d%c",robotDeal.BatteryPercentage,0);
	len = strlen(&temp[index]);
	temp[index-1] = len;	
	index = index + len;	
	//
	temp[index] = 0X00;	index++;//控件号
	temp[index] = 0X05;	index++;
	
	temp[index] = 0X00;	index++;//长度
	temp[index] = 0X00;	index++;
	
	sprintf(&temp[index],"%d%c",robotDeal.BatteryStatus,0);
	len = strlen(&temp[index]);
	temp[index-1] = len;	
	index = index + len;
	
	//11
	temp[index] = 0X00;	index++;//控件号
	temp[index] = 0X0b;	index++;
	
	temp[index] = 0X00;	index++;//长度
	temp[index] = 0X00;	index++;
	
	sprintf(&temp[index],"%d  %d  %d  %d%c",	 robotDeal.rearWarningLevel[0],robotDeal.rearWarningLevel[1] \
												,robotDeal.rearWarningLevel[2],robotDeal.rearWarningLevel[3],0);
	len = strlen(&temp[index]);
	temp[index-1] = len;	
	index = index + len;
	
	temp[index] = 0X00;	index++;//控件号
	temp[index] = 0X0c;	index++;
	
	temp[index] = 0X00;	index++;//长度
	temp[index] = 0X00;	index++;
	sprintf(&temp[index],"%d%c",robotDeal.aheadWarningLevel,0);
	len = strlen(&temp[index]);
	temp[index-1] = len;	
	index = index + len;
	
	temp[index] = 0XFF;	index++;
	temp[index] = 0XFC;	index++;
	temp[index] = 0XFF;	index++;
	temp[index] = 0XFF;	index++;
	
	HMIDevSendData( temp, index );
	return OK_RETURN_VALUE;
}

static int HMIDevavailable(void)
{
	return fifo8_status(&UartHMIDev.LiBatteryFIFO);
}

static unsigned char HMIDevRcvInt(unsigned char data)
{
	fifo8_putPut(&UartHMIDev.LiBatteryFIFO,data);
	return 1;
}

static unsigned char HMIDevRead(void)
{
	return fifo8_get(&UartHMIDev.LiBatteryFIFO);
}

//Struct_Usr_RearRadar rearRadar;
unsigned char __cycHMIDevDeal(void)
{
	unsigned char SonarData;
	if( (getSystemTick() - UartHMIDev.Tick )>=500)
	{
		HMIDevTrigger();
		UartHMIDev.Tick = getSystemTick();
	}
	if (HMIDevavailable() > 0) 
	{
		SonarData = HMIDevRead();
		if(UartHMIDev.step == 0)
		{
			if(SonarData == 0xEE)	{ UartHMIDev.step = 1;}
			else					{ UartHMIDev.step = 0;}
		}
		else if(UartHMIDev.step == 1)
		{
			if(SonarData == 0xB1)	{ UartHMIDev.step = 2;}
			else					{ UartHMIDev.step = 0;}
		}
		else if(UartHMIDev.step == 2)
		{
			if(SonarData == 0x11)	
			{ 
				UartHMIDev.dataIndex++;
				UartHMIDev.dataTotal = 7;//按钮控件接收数据7 
				UartHMIDev.step = 3;
			}
			else					
			{ 
				UartHMIDev.step = 0;
			}
		}
		else if(UartHMIDev.step == 3)
		{
			//数据信息
            UartHMIDev.dataBuffer[UartHMIDev.dataIndex] = SonarData;
            UartHMIDev.dataIndex++;
            if(UartHMIDev.dataIndex == UartHMIDev.dataTotal)
            {
                UartHMIDev.step = 5;
            }
		}
		else if(UartHMIDev.step == 5)
		{
			if(SonarData == 0xff)	{ UartHMIDev.step = 6;}
			else					{ UartHMIDev.step = 0;}
		}
		else if(UartHMIDev.step == 6)
		{
			if(SonarData == 0xfc)	{ UartHMIDev.step = 6;}
			else					{ UartHMIDev.step = 0;}
		}
		else if(UartHMIDev.step == 7)
		{
			if(SonarData == 0xff)	{ UartHMIDev.step = 6;}
			else					{ UartHMIDev.step = 0;}
		}
		else if(UartHMIDev.step == 8)
		{
			if(SonarData == 0xff)	
			{
				//触发用户函数
			}
			UartHMIDev.step = 0;
		}
	}
}

static unsigned char HMIDevFIFORcvData[1024];
void HMIDevInit(void)
{	
	fifo8_init(&UartHMIDev.LiBatteryFIFO,sizeof(HMIDevFIFORcvData), HMIDevFIFORcvData);
	Usart2_Init(115200);
	UART2_DMA_Init(UartHMIDev.DMABuffer);
	UartHMIDev.Tick = getSystemTick();
}
