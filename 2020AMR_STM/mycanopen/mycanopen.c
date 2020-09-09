#include "mycanopen.h"
#include "LinerBuf.h"
unsigned int canOpenAnserCount = 0;
void canOpenSendNMT(unsigned char id,unsigned char param)
{
	unsigned char data[13];
	unsigned short cobid = 0x000;
	data[0]=0;
	data[1]=0;
	data[2]=0;
	data[3]=0;
	
	data[4]=8;
	data[5]=param;
	data[6]=id;
	data[7]=0;
	data[8]=0;
	data[9]=0;
	data[10]=0;
	data[11]=0;
	data[12]=0;
	CAN_Send_Msg(data);
}
void canOpenSendSYNC(void)
{
	unsigned char data[13];
	unsigned short cobid = 0x000;
	data[0]=0;
	data[1]=0;
	data[2]=0;
	data[3]=0x80;
	
	data[4]=0;
	data[5]=0;
	data[6]=0;
	data[7]=0;
	data[8]=0;
	data[9]=0;
	data[10]=0;
	data[11]=0;
	data[12]=0;
	CAN_Send_Msg(data);
}

void canOpenWritePDO(unsigned char id,unsigned short op1,unsigned short op2,unsigned int op3)
{
	unsigned char data[13];
	unsigned short cobid = 0x200;
	cobid = cobid+id;
	data[0]=0;
	data[1]=0;
	data[2]=(cobid>>8)&0xff;
	data[3]=cobid&0xff;
	
	data[4]=8;
	data[5]=op1&0xff;
	data[6]=(op1>>8)&0xff;
	data[7]=op2&0xff;
	data[8]=(op2>>8)&0xff;
	data[9]=op3&0xff;
	data[10]=(op3>>8)&0xff;
	data[11]=(op3>>16)&0xff;
	data[12]=(op3>>24)&0xff;
	CAN_Send_Msg(data);
}

void canOpenWritePDO2(unsigned char id)
{
	
	unsigned char data[13];
	unsigned short cobid = 0x300;
	cobid = cobid+id;
	data[0]=0;
	data[1]=0;
	data[2]=(cobid>>8)&0xff;
	data[3]=cobid&0xff;
	
	data[4]=8;
	data[5]=0x0;
	data[6]=0x3;
	data[7]=0xf0;
	data[8]=03;
	data[9]=20;
	data[10]=0;
	data[11]=20;
	data[12]=0;
	CAN_Send_Msg(data);
}
void canOpenWritePDO3(unsigned char id)
{
	
	unsigned char data[13];
	unsigned short cobid = 0x300;
	cobid = cobid+id;
	data[0]=0;
	data[1]=0;
	data[2]=(cobid>>8)&0xff;
	data[3]=cobid&0xff;
	
	data[4]=8;
	data[5]=0x2;
	data[6]=0x3;
	data[7]=0xf0;
	data[8]=03;
	data[9]=10;
	data[10]=0;
	data[11]=10;
	data[12]=0;
	CAN_Send_Msg(data);
}
void canOpenWritePDO1(unsigned char id,int sp)
{
	unsigned char data[13];
	unsigned short cobid = 0x200;
	cobid = cobid+id;
	data[0]=0;
	data[1]=0;
	data[2]=(cobid>>8)&0xff;
	data[3]=cobid&0xff;
	
	data[4]=4;
	data[5]=sp&0xff;
	data[6]=(sp>>8)&0xff;
	data[7]=(sp>>16)&0xff;
	data[8]=(sp>>24)&0xff;
	CAN_Send_Msg(data);
}
void canOpenReadSOD(unsigned char id,unsigned short index,unsigned char subindex,unsigned int len)
{
	unsigned char data[12];
	unsigned short cobid = 0x600;
	cobid = cobid+id;
	data[0]=0;
	data[1]=0;
	data[2]=(cobid>>8)&0xff;
	data[3]=cobid&0xff;
	
	data[4]=8;
	data[5]=0x40;
	data[6]=index&0xff;
	data[7]=(index>>8)&0xff;
	data[8]=subindex;
	data[9]=0;
	data[10]=0;
	data[11]=0;
	data[12]=0;
	CAN_Send_Msg(data);
}

void canTest()
{
	unsigned char data[13];
	data[0]=0x0c;
	data[1]=0x07;
	data[2]=0x9a;
	data[3]=0xa7;
	
	data[4]=8;
	data[5]=0x01;
	data[6]=0xff;
	data[7]=0xff;
	data[8]=0xff;
	data[9]=0xff;
	data[10]=0xff;
	data[11]=0xff;
	data[12]=0xff;
	CAN_Send_Msg(data);
}

void canTest1()
{
	unsigned char data[13];
	data[0]=0x0c;
	data[1]=0x07;
	data[2]=0x9a;
	data[3]=0xa7;
	
	data[4]=8;
	data[5]=0x00;
	data[6]=0xff;
	data[7]=0xff;
	data[8]=0xff;
	data[9]=0xff;
	data[10]=0xff;
	data[11]=0xff;
	data[12]=0xff;
	CAN_Send_Msg(data);
}

void canOpenWriteSOD(unsigned char id,unsigned short index,unsigned char subindex,unsigned int len,unsigned int param)
{
	unsigned char data[13];
	unsigned short cobid = 0x600;
	cobid = cobid+id;
	data[0]=0;
	data[1]=0;
	data[2]=(cobid>>8)&0xff;
	data[3]=cobid&0xff;
	
	data[4]=8;
	
	if(len == 4)
		data[5]=0x23;
	else if(len == 3)
		data[5]=0x27;
	else if(len == 2)
		data[5]=0x2b;
	else
		data[5]=0x2f;
	data[6]=index&0xff;
	data[7]=(index>>8)&0xff;
	data[8]=subindex;
	data[9]=param&0xff;
	data[10]=(param>>8)&0xff;
	data[11]=(param>>16)&0xff;
	data[12]=(param>>24)&0xff;
	CAN_Send_Msg(data);
	
}

void dealcanopencount(void)
{
	canOpenAnserCount++;
}
unsigned char canOpenWriteSODAnser(unsigned char id,unsigned short index,unsigned char subindex,unsigned int len,unsigned int param,unsigned int *getParam)
{
	unsigned char tempCanRcv[13];
	int trytimes = 3;
	while(trytimes)
	{
		trytimes--;
		canOpenWriteSOD(id,index,subindex,len,param);
		canOpenAnserCount=0;
		//Î´³¬Ê±£»
		while(canOpenAnserCount<5)
		{
			if(NmlGetDataFromStackAlloc( &Rcv_SDO_Stack,tempCanRcv)!=0)
			{
				unsigned int cobID;
				unsigned short indexRcv;
				unsigned short indexSubRcv;
				unsigned int getP=0;
				cobID = tempCanRcv[2];
				cobID = cobID<<8;
				cobID = cobID + tempCanRcv[3];
				
				indexRcv = tempCanRcv[7];
				indexRcv = indexRcv<<8;
				indexRcv = indexRcv + tempCanRcv[6];
				
				indexSubRcv = tempCanRcv[8];
				if(cobID == (0x580 + id ))
				{
					if(index == indexRcv)
					{
						getP = tempCanRcv[12];
						getP = getP<<8;
						getP = getP + tempCanRcv[11];
						getP = getP<<8;
						getP = getP + tempCanRcv[10];
						getP = getP<<8;
						getP = getP + tempCanRcv[9];
						*getParam = getP;
						return 1;
					}
				}
			}
		}
	}
	return 0;
}

unsigned char canOpenReadSODAnser(unsigned char id,unsigned short index,unsigned char subindex,unsigned int len,unsigned int *getParam)
{
	unsigned char tempCanRcv[13];
	canOpenReadSOD(id,index,subindex,len);
	canOpenAnserCount=0;
	//Î´³¬Ê±£»
	while(canOpenAnserCount<5)
	{
		if(NmlGetDataFromStackAlloc( &Rcv_SDO_Stack,tempCanRcv)!=0)
		{
			unsigned int cobID;
			unsigned short indexRcv;
			unsigned short indexSubRcv;
			unsigned int getP=0;
			cobID = tempCanRcv[2];
			cobID = cobID<<8;
			cobID = cobID + tempCanRcv[3];
			
			indexRcv = tempCanRcv[7];
			indexRcv = indexRcv<<8;
			indexRcv = indexRcv + tempCanRcv[6];
			
			indexSubRcv = tempCanRcv[8];
			if(cobID == (0x580 + id ))
			{
				if(index == indexRcv)
				{
					getP = tempCanRcv[12];
					getP = getP<<8;
					getP = getP + tempCanRcv[11];
					getP = getP<<8;
					getP = getP + tempCanRcv[10];
					getP = getP<<8;
					getP = getP + tempCanRcv[9];
					*getParam = getP;
					return 1;
				}
			}
		}
	}
	return 0;
}
