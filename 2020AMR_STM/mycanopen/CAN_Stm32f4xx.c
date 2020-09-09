#include "stm32f4xx.h"
#include "LinerBuf.h"
CanRxMsg RxMessage;
//#define	TEST_POWER
void CAN_Send_Msg(unsigned char *can_tx);
void canOpen_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	Init_Can_Stack();
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_CAN1,ENABLE);
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_CAN2,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	
	GPIO_PinAFConfig(GPIOD,  GPIO_PinSource0,  GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOD,  GPIO_PinSource1,  GPIO_AF_CAN1);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	CAN_DeInit(CAN1);

	#ifdef	TEST_POWER 
	CAN_InitStructure.CAN_Prescaler=12;
	#else
	CAN_InitStructure.CAN_Prescaler=6;
	#endif
	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1=CAN_BS1_7tq;
	CAN_InitStructure.CAN_BS2=CAN_BS2_6tq;
	CAN_InitStructure.CAN_TTCM=DISABLE;
	CAN_InitStructure.CAN_ABOM=DISABLE;
	CAN_InitStructure.CAN_AWUM=DISABLE;
	CAN_InitStructure.CAN_NART=DISABLE;
	CAN_InitStructure.CAN_RFLM=DISABLE;
	CAN_InitStructure.CAN_TXFP=DISABLE;
	CAN_Init( CAN1, &CAN_InitStructure);

	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterNumber=0;
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	NVIC_InitStructure.NVIC_IRQChannel=CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel=CAN1_TX_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	CAN_ITConfig(CAN1,  CAN_IT_FMP0,  ENABLE);
	CAN_ITConfig(CAN1,  CAN_IT_TME,  ENABLE);
	
	CAN_ClearITPendingBit(CAN1,  CAN_IT_FMP0);
	CAN_ClearITPendingBit(CAN1,  CAN_IT_TME);
	
	//canTest1();
	canTest();
	//canTest1();
	//canTest();
}
#ifdef	TEST_POWER
void CAN_Send(unsigned char *can_tx_buf)
{
	int i;
	CanTxMsg TxMessage;
	TxMessage.ExtId=(can_tx_buf[0]<<24)+(can_tx_buf[1]<<16)+(can_tx_buf[2]<<8)+can_tx_buf[3];	
	TxMessage.IDE=CAN_Id_Extended;
	TxMessage.RTR=CAN_RTR_DATA;
	TxMessage.DLC=can_tx_buf[4];
	for(i=0;i<8;i++)	
	{
		TxMessage.Data[i]=can_tx_buf[i+5];
	}
	CAN_Transmit(CAN1,&TxMessage);
}
#else
void CAN_Send(unsigned char *can_tx_buf)
{
	int i;
	CanTxMsg TxMessage;
	TxMessage.StdId=(can_tx_buf[0]<<24)+(can_tx_buf[1]<<16)+(can_tx_buf[2]<<8)+can_tx_buf[3];	
	TxMessage.IDE=CAN_Id_Standard;
	TxMessage.RTR=CAN_RTR_DATA;
	TxMessage.DLC=can_tx_buf[4];
	for(i=0;i<8;i++)	
	{
		TxMessage.Data[i]=can_tx_buf[i+5];
	}
	CAN_Transmit(CAN1,&TxMessage);
}
#endif
//#define	ARMDisableInt()	tsk_lock()
//#define	ARMEnableInt()	tsk_unlock()
void CAN_Send_Msg(unsigned char *can_tx)
{
	NmlAddDataToStackAlloc(&Snd_Can_Stack,can_tx);
	if ((CAN1->TSR&0x04000000) != 0)
	{
		if(NmlGetDataFromStackAlloc( &Snd_Can_Stack,can_tx)!=0)
		{
			CAN_Send(can_tx);
		}
	}
}

void CAN1_TX_IRQHandler(void)
{
	if( CAN_GetITStatus(CAN1, CAN_IT_TME )!= RESET)
	{
		u8 can_tx[13];
		if ((CAN1->TSR&0x04000000) != 0)
		{
			if(IntGetDataFromStackAlloc( &Snd_Can_Stack,can_tx)!=0)
			{
				CAN_Send(can_tx);
			}
		}
	}
	CAN_ClearITPendingBit(CAN1,  CAN_IT_TME);
}

void CAN1_RX0_IRQHandler(void)
{ 
	unsigned char canbuf[13];	
	CanRxMsg RxMessage;
	if( CAN_GetITStatus(CAN1, CAN_IT_FMP0 )!= RESET)
	{
		CAN_Receive(CAN1,  CAN_FIFO0,  &RxMessage);
		canbuf[0]=RxMessage.StdId>>24;
		canbuf[1]=RxMessage.StdId>>16;
		canbuf[2]=RxMessage.StdId>>8;
		canbuf[3]=RxMessage.StdId;
		canbuf[4]=RxMessage.DLC;
		canbuf[5]=RxMessage.Data[0];
		canbuf[6]=RxMessage.Data[1];
		canbuf[7]=RxMessage.Data[2];
		canbuf[8]=RxMessage.Data[3];
		canbuf[9]=RxMessage.Data[4];
		canbuf[10]=RxMessage.Data[5];
		canbuf[11]=RxMessage.Data[6];
		canbuf[12]=RxMessage.Data[7];
		if((RxMessage.StdId>=0x181)&&(RxMessage.StdId<=0x184))
			IntAddDataToStackAlloc( &Rcv_PDO14_Stack, canbuf);
		else if((RxMessage.StdId>=0x281)&&(RxMessage.StdId<=0x284))
			IntAddDataToStackAlloc( &Rcv_PDO14_Stack, canbuf);
		else if((RxMessage.StdId>=0x185)&&(RxMessage.StdId<=0x188))
			IntAddDataToStackAlloc( &Rcv_PDO58_Stack, canbuf);
		else if((RxMessage.StdId>0x580)&&(RxMessage.StdId<0x58a))
			IntAddDataToStackAlloc( &Rcv_SDO_Stack, canbuf);
	}
	CAN_ClearITPendingBit(CAN1,  CAN_IT_FMP0);
}
