#include "stm32f4xx.h"
#include "ROSDev.h"
#include "mydef.h"
#include "baseRobot.h"
#include "timerDev.h"

void DMA_Use_USART5_Tx_Init(void);
///////////////////////////////////////////////////硬件///////////////////////////////////////////////////
//USART5
//TX:PC12
//RX:PD2
//初始化串口
void UART5_Init(int bandrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
	USART_DeInit(UART5);
	GPIO_PinAFConfig(GPIOC,  GPIO_PinSource12,  GPIO_AF_UART5);
	GPIO_PinAFConfig(GPIOD,  GPIO_PinSource2, GPIO_AF_UART5);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOD,&GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = bandrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART5, &USART_InitStructure);
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
    USART_Cmd(UART5, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);  
}

void UART5_IRQHandler(void)
{
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) //接收到数据
	{
		mainControllerRcvInt(USART_ReceiveData(UART5));
		USART_ClearITPendingBit(UART5,  USART_IT_RXNE);
	}
	else if(USART_GetITStatus(UART5, USART_IT_TXE) != RESET)
	{
		USART_ClearITPendingBit(UART5,  USART_IT_TXE);
	}
}

void UART5_DMA_Init(unsigned char *dmabuffer)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Stream7); 

    DMA_InitStructure.DMA_Channel             = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)(&(UART5->DR));
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

    DMA_Init(DMA1_Stream7, &DMA_InitStructure);

    DMA_ITConfig(DMA1_Stream7, DMA_IT_TC, ENABLE);

    USART_DMACmd(UART5, USART_DMAReq_Tx, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel                   = DMA1_Stream7_IRQn;           
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1; 
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
               
	DMA_ClearFlag(DMA1_Stream7,DMA_FLAG_TCIF7);
    DMA_Cmd(DMA1_Stream7, DISABLE);
}

void DMA1_Stream7_IRQHandler(void)
{         
	if(DMA_GetITStatus(DMA1_Stream7,DMA_IT_TCIF7) != RESET)   
    {  
        DMA_ClearFlag(DMA1_Stream7,DMA_IT_TCIF7); 
		mainControlleSendOver();
    }  
}

unsigned char mainControlleEnableDMA(unsigned short len)
{
	DMA_SetCurrDataCounter(DMA1_Stream7,len); 
	DMA_Cmd(DMA1_Stream7,ENABLE); 
}

///////////////////////////////////////////////////////////////////////////////////////////////////

static Struct_Drv_Uart UartMainController;
static void mainControlleSendOver(void)
{
	UartMainController.DMABusy = 0;
}

int mainControlleSendData(unsigned char *data ,int len)
{
	if(UartMainController.DMABusy == 0 )
	{
		UartMainController.DMABusy = 1;
		memcpy( UartMainController.DMABuffer, data, len );
		mainControlleEnableDMA(len);
		
		return 0;
	}
	return 1;
}
// 触发超声探头不断发送数据
static unsigned char mainControllerTrigger(void)
{
	unsigned char cmd[1]={0xb3};
	//rearRadarSendData(cmd,sizeof(cmd));
	//rearRadarSendData(cmd,sizeof(cmd));
	return OK_RETURN_VALUE;
}

static int mainControlleravailable(void)
{
	return fifo8_status(&UartMainController.LiBatteryFIFO);
}

static unsigned char mainControllerRcvInt(unsigned char data)
{
	fifo8_putPut(&UartMainController.LiBatteryFIFO,data);
	return 1;
}

static unsigned char rearRadarRead(void)
{
	return fifo8_get(&UartMainController.LiBatteryFIFO);
}

Struct_Usr_mainController mainController;
void __cycMainControllerDeal(void)
{
	unsigned char SonarData;
	if( (getSystemTick() - UartMainController.Tick )>=1000)
	{
		//rearRadarTrigger();
		UartMainController.Tick = getSystemTick();
	}
	//超声波有数据收到；
	while(1)
	{
		if (mainControlleravailable() > 0) 
		{
			// 从串口缓存读取数据
			SonarData = rearRadarRead();
			//起始桢1
			if(UartMainController.step == 0)
			{
				if(SonarData == 0xFE)	{ UartMainController.step = 1;}
				else					{ UartMainController.step = 0;}
			}
			//起始桢2
			else if(UartMainController.step == 1)
			{
				if(SonarData == 0xEE)	{ UartMainController.step = 2;}
				else					{ UartMainController.step = 0;}
			}
			else if(UartMainController.step == 2)
			{
				//数据字节数；
				UartMainController.dataIndex = 0;
				UartMainController.dataTotal = SonarData;
				UartMainController.dataBuffer[0] = SonarData;
				UartMainController.step = 4;
			}
			else if(UartMainController.step ==3)
			{
				//主命令；
				//UartMainController.type = SonarData;
				//UartMainController.dataBuffer[0] = SonarData;
				UartMainController.step = 4;
			}
			else if(UartMainController.step == 4)
			{
				//数据信息
				UartMainController.dataBuffer[UartMainController.dataIndex + 1] = SonarData;
				UartMainController.dataIndex++;
				if(UartMainController.dataIndex == UartMainController.dataTotal)
				{
					UartMainController.step = 5;
				}
			}
			//校验和高位，暂时不计算
			else if(UartMainController.step == 5)
			{
				UartMainController.step = 6;
			}
			//校验和低位，暂时不计算
			else if(UartMainController.step == 6)
			{
				UartMainController.step = 7;
			}
			//结束帧1
			else if(UartMainController.step == 7)
			{
				if(SonarData == 0xFC) {UartMainController.step = 0x08;}
				else                  {UartMainController.step = 0x00;}
			}
			//结束帧2
			else if(UartMainController.step == 8)
			{
				//收到一帧正确的数据；
				if(SonarData == 0xFF) 
				{
					//拷贝串口数据到用户数据；
					RosDevFrameCallback((void *)UartMainController.dataBuffer);
					UartMainController.step = 0;
					break;
				}
				else
				{
					UartMainController.step = 0;
				}
			}
		}
		else
		{
			break;
		}
	}
}

static unsigned char mainControllerFIFORcvData[1024];
void MainControllerInit(void)
{	
	fifo8_init(&UartMainController.LiBatteryFIFO,sizeof(mainControllerFIFORcvData), mainControllerFIFORcvData);
	UART5_Init(9600);
	UART5_DMA_Init(UartMainController.DMABuffer);
	//rearRadarTrigger();
	UartMainController.Tick = getSystemTick();
}
