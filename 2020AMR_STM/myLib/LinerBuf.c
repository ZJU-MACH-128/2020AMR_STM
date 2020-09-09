#include "LinerBuf.h"
#include "string.h"
#define	ARMDisableInt()	__disable_irq()
#define	ARMEnableInt()	__enable_irq()

//��ʼ���ڴ棺����addr��ʼ����Ϊlen*datalen��Ϊ������
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

//���ж�д��������д�������ΪDataLen���ֽ�
unsigned char NmlAddDataToStackAlloc( STRUCT_AllocStack *whichstack, unsigned char *databuf)	
{
	unsigned char *ptin;
	if(whichstack->StackInIndex==whichstack->StackOutIndex)	//���뻺����������==��ȡ��������������ȣ�����
	{
		ARMDisableInt();
		whichstack->StackInIndex=0;
		whichstack->StackOutIndex=0;
		ARMEnableInt();
	}
	if(whichstack->StackInIndex>=(whichstack->StackLen-1))	//����������������
	{
		ARMDisableInt();
		whichstack->isFull=1;
		ARMEnableInt();
		return 0;
	}
	else													//������д�뻺����
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
	if(whichstack->StackInIndex==whichstack->StackOutIndex)	//���뻺����������==��ȡ��������������ȣ�����
	{
		whichstack->StackInIndex=0;
		whichstack->StackOutIndex=0;
	}
	if(whichstack->StackInIndex>=(whichstack->StackLen-1))	//����������������
	{
		whichstack->isFull=1;
		return 0;
	}
	else													//������д�뻺����
	{
		ptin = &whichstack->Strdata[whichstack->StackInIndex*whichstack->DataLen];
		memcpy((unsigned char *)ptin,(unsigned char *)databuf,whichstack->DataLen);
		whichstack->StackInIndex++;
		return 1;
	}
}

//���ж϶�������������DataLen���ֽڵ����ݵ�data
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
