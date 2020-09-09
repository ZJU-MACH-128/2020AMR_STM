#include "LinerBuf.h"
#include "string.h"
#define	ARMDisableInt()	__disable_irq()
#define	ARMEnableInt()	__enable_irq()

//初始化内存：分配addr开始长度为len*datalen做为缓冲区
void InintStackAlloc( STRUCT_AllocStack *whichstack, unsigned char *addr,unsigned int len,unsigned int datalen)		
{
	unsigned char *p;
	p=(unsigned char *)addr;
	whichstack->Strdata=p;
	whichstack->DataLen=datalen;
	whichstack->StackLen=len;
	whichstack->StackInIndex=0;
	whichstack->StackOutIndex=0;
	whichstack->isFull=0;
}
void CleanStackAlloc( STRUCT_AllocStack *whichstack)
{
	whichstack->StackInIndex=0;
	whichstack->StackOutIndex=0;
	whichstack->isFull=0;
}

//非中断写缓冲区：写入的数据为DataLen个字节
unsigned char NmlAddDataToStackAlloc( STRUCT_AllocStack *whichstack, unsigned char *databuf)	
{
	unsigned char *ptin;
	if(whichstack->StackInIndex==whichstack->StackOutIndex)	//存入缓冲区的数据==读取缓冲区的数据相等？清零
	{
		ARMDisableInt();
		whichstack->StackInIndex=0;
		whichstack->StackOutIndex=0;
		ARMEnableInt();
	}
	if(whichstack->StackInIndex>=(whichstack->StackLen-1))	//缓冲区已满？报错
	{
		ARMDisableInt();
		whichstack->isFull=1;
		ARMEnableInt();
		return 0;
	}
	else													//将数据写入缓冲区
	{
		ARMDisableInt();
		ptin = &whichstack->Strdata[whichstack->StackInIndex*whichstack->DataLen];
		memcpy((unsigned char *)ptin,(unsigned char *)databuf,whichstack->DataLen);
		whichstack->StackInIndex++;
		ARMEnableInt();
		return 1;
	}
}


unsigned char IntAddDataToStackAlloc( STRUCT_AllocStack *whichstack, unsigned char *databuf)	
{
	unsigned char *ptin;
	if(whichstack->StackInIndex==whichstack->StackOutIndex)	//存入缓冲区的数据==读取缓冲区的数据相等？清零
	{
		whichstack->StackInIndex=0;
		whichstack->StackOutIndex=0;
	}
	if(whichstack->StackInIndex>=(whichstack->StackLen-1))	//缓冲区已满？报错
	{
		whichstack->isFull=1;
		return 0;
	}
	else													//将数据写入缓冲区
	{
		ptin = &whichstack->Strdata[whichstack->StackInIndex*whichstack->DataLen];
		memcpy((unsigned char *)ptin,(unsigned char *)databuf,whichstack->DataLen);
		whichstack->StackInIndex++;
		return 1;
	}
}

//非中断读缓冲区：读出DataLen个字节的数据到data
unsigned char NmlGetDataFromStackAlloc( STRUCT_AllocStack *whichstack,unsigned char  * data)	
{
	unsigned char *ptout;
	if(whichstack->StackOutIndex>whichstack->StackInIndex)		
	{
		ARMDisableInt();
		whichstack->isFull=1;
		ARMEnableInt();
		return 0;
	}
	else if(whichstack->StackOutIndex==whichstack->StackInIndex)
	{
		return 0;
	}
	else
	{
		ARMDisableInt();
		ptout = &whichstack->Strdata[whichstack->StackOutIndex*whichstack->DataLen];
		memcpy((unsigned char *)data,(unsigned char *)ptout,whichstack->DataLen);
		whichstack->StackOutIndex++;
		ARMEnableInt();
		return 1;
	}
}

unsigned char IntGetDataFromStackAlloc( STRUCT_AllocStack *whichstack,  unsigned char  *data)	
{
	unsigned char *ptout;
	if(whichstack->StackOutIndex>whichstack->StackInIndex)		
	{
		whichstack->isFull=1;
		return 0;
	}
	else if(whichstack->StackOutIndex==whichstack->StackInIndex)
	{
		return 0;
	}
	else
	{
		ptout = &whichstack->Strdata[whichstack->StackOutIndex*whichstack->DataLen];
		memcpy((unsigned char *)data,(unsigned char *)ptout,whichstack->DataLen);
		whichstack->StackOutIndex++;
		return 1;
	}
}

unsigned char buf1[TOTAL_BUF*BTYE_PER_BUF];
unsigned char buf2[TOTAL_BUF*BTYE_PER_BUF];
unsigned char buf3[TOTAL_BUF*BTYE_PER_BUF];
unsigned char buf4[TOTAL_BUF*BTYE_PER_BUF];
unsigned char buf5[TOTAL_BUF*BTYE_PER_BUF];

STRUCT_AllocStack Snd_Can_Stack;
STRUCT_AllocStack Rcv_SDO_Stack;
STRUCT_AllocStack Rcv_PDO14_Stack;
STRUCT_AllocStack Rcv_PDO58_Stack;
STRUCT_AllocStack Rcv_UART_Stack;

void Init_Can_Stack(void)
{
	 InintStackAlloc(&Snd_Can_Stack,(unsigned char *)&buf1[0],TOTAL_BUF,BTYE_PER_BUF);
	 InintStackAlloc(&Rcv_SDO_Stack,(unsigned char *)&buf2[0],TOTAL_BUF,BTYE_PER_BUF);
	 InintStackAlloc(&Rcv_PDO14_Stack,(unsigned char *)&buf3[0],TOTAL_BUF,BTYE_PER_BUF);
	 InintStackAlloc(&Rcv_PDO58_Stack,(unsigned char *)&buf5[0],TOTAL_BUF,BTYE_PER_BUF);
	 InintStackAlloc(&Rcv_UART_Stack,(unsigned char *)&buf4[0],TOTAL_BUF,BTYE_PER_BUF);
}
