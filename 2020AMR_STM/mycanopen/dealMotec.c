#include "math.h"
#include "stdlib.h"
#include "mycanopen.h"
#include "dealMotec.h"
#include "LinerBuf.h"
#include "motec.h"
//#define	FOR_4_WHEEL

typedef struct  
{
	int zeroCount;
	int unEnableFlag;
	int rcvSpeed;
	int sendSpeed;
	int speedBuf[10];
	int index;
}strController ;
short addSpeed(short cS,short lS);
strController controller = {0,0,};
void zeroTimer(void)
{
	if(controller.zeroCount == 1 )
	{
		controller.unEnableFlag = 1;
	}
	if(controller.zeroCount!=0)
	{
		controller.zeroCount--;
	}
}


canopenWheel robotWheel = {STATUS_INIT,0,0,0,0,0,0};

unsigned int canOpenSendTimer = 0;
short intLeftLast = 0,intRightLast = 0,intLeftBLast = 0,intRightBLast = 0;

extern unsigned int canOpenAnserCount;
void canOpenCount(void)
{
	canOpenSendTimer++;
	canOpenAnserCount++;
	if(robotWheel.leftRcvPdoCount != 0)
		robotWheel.leftRcvPdoCount--;
	if(robotWheel.rightRcvPdoCount != 0)
		robotWheel.rightRcvPdoCount--;
	if(robotWheel.leftBRcvPdoCount != 0)
		robotWheel.leftBRcvPdoCount--;
	if(robotWheel.rightBRcvPdoCount != 0)
		robotWheel.rightBRcvPdoCount--;
}

#define	MAX_SPEED	13400
void setSpeed(double sp1,double sp2, double sp3, double sp4, int flag)
{
	if(sp1>MAX_SPEED)
		robotWheel.SetSpeedLeft = MAX_SPEED;
	else if(sp1<-MAX_SPEED)
		robotWheel.SetSpeedLeft = -MAX_SPEED;
	else
		robotWheel.SetSpeedLeft = (short) sp1;
		
	if(sp2>MAX_SPEED)
		robotWheel.SetSpeedRight = MAX_SPEED;
	else if(sp2<-MAX_SPEED)
		robotWheel.SetSpeedRight = -MAX_SPEED;
	else
		robotWheel.SetSpeedRight = (short) sp2;
	
	if(sp3>MAX_SPEED)
		robotWheel.SetSpeedLeftB = MAX_SPEED;
	else if(sp3<-MAX_SPEED)
		robotWheel.SetSpeedLeftB = -MAX_SPEED;
	else
		robotWheel.SetSpeedLeftB = (short) sp3;
	
	if(sp4>MAX_SPEED)
		robotWheel.SetSpeedRightB = MAX_SPEED;
	else if(sp4<-MAX_SPEED)
		robotWheel.SetSpeedRightB = -MAX_SPEED;
	else
		robotWheel.SetSpeedRightB = (short) sp4;
	
	
		//robotWheel.SetSpeedLeftB = robotWheel.SetSpeedLeft;
		//robotWheel.SetSpeedRightB = robotWheel.SetSpeedRight;
}

unsigned char tempCanData[BTYE_PER_BUF];
void dealRobotWheel(void)
{
	unsigned int cobID;
	//unsigned index;
	if(robotWheel.status != STATUS_INIT)
	{
		if(NmlGetDataFromStackAlloc( &Rcv_PDO14_Stack,tempCanData)!=0)
		{
			cobID = tempCanData[2];
			cobID = cobID<<8;
			cobID = cobID + tempCanData[3];
			
			//index = tempCanData[6];
			//index = index<<8;
			//index = index + tempCanData[5];
			
			if(cobID == 0x181)
			{
				unsigned int encoder = 0;
				unsigned short status = 0;
				
				status = tempCanData[6];
				status = status<<8;
				status = status + tempCanData[5];
				robotWheel.leftStatusCoder = status;
				
				encoder = tempCanData[12];
				encoder = encoder<<8;
				encoder = encoder + tempCanData[11];
				encoder = encoder<<8;
				encoder = encoder + tempCanData[10];
				encoder = encoder<<8;
				encoder = encoder + tempCanData[9];
				
				robotWheel.leftEnCoder=-encoder;
				robotWheel.leftRcvPdoCount =TIMEOUT_RCV_INFOR;
				RobotVectorInvers();
			}
			else if(cobID == 0x281)
			{
				unsigned int status = 0;
				unsigned short current = 0;
				
				current = tempCanData[10];
				current = current<<8;
				current = current + tempCanData[9];
				robotWheel.leftCurrent = current;
				
				status = tempCanData[8];
				status = status<<8;
				status = status + tempCanData[7];
				status = status<<8;
				status = status + tempCanData[6];
				status = status<<8;
				status = status + tempCanData[5];
				
				robotWheel.leftAlarmCoder=status;
				robotWheel.leftRcvPdoCount =TIMEOUT_RCV_INFOR;
			}
			else if(cobID == 0x182)
			{
				unsigned int encoder = 0;
				unsigned short status = 0;
				
				status = tempCanData[6];
				status = status<<8;
				status = status + tempCanData[5];
				robotWheel.rightStatusCoder = status;
				
				encoder = tempCanData[12];
				encoder = encoder<<8;
				encoder = encoder + tempCanData[11];
				encoder = encoder<<8;
				encoder = encoder + tempCanData[10];
				encoder = encoder<<8;
				encoder = encoder + tempCanData[9];
				
				robotWheel.rightEnCoder=encoder;
				robotWheel.rightRcvPdoCount =TIMEOUT_RCV_INFOR;
				RobotVectorInvers();
			}
			else if(cobID == 0x282)
			{
				unsigned int status = 0;
				unsigned short current = 0;
				
				current = tempCanData[10];
				current = current<<8;
				current = current + tempCanData[9];
				robotWheel.rightCurrent = current;
				
				status = tempCanData[8];
				status = status<<8;
				status = status + tempCanData[7];
				status = status<<8;
				status = status + tempCanData[6];
				status = status<<8;
				status = status + tempCanData[5];
				
				robotWheel.rightAlarmCoder=status;
				robotWheel.rightRcvPdoCount =TIMEOUT_RCV_INFOR;
			}
			#ifdef	FOR_4_WHEEL
			else if(cobID == 0x183)
			{
				unsigned int encoder = 0;
				unsigned short status = 0;
				
				status = tempCanData[6];
				status = status<<8;
				status = status + tempCanData[5];
				robotWheel.leftBStatusCoder = status;
				
				encoder = tempCanData[12];
				encoder = encoder<<8;
				encoder = encoder + tempCanData[11];
				encoder = encoder<<8;
				encoder = encoder + tempCanData[10];
				encoder = encoder<<8;
				encoder = encoder + tempCanData[9];
				
				robotWheel.leftBEnCoder=-encoder;
				robotWheel.leftBRcvPdoCount =TIMEOUT_RCV_INFOR;
				RobotVectorInvers();
			}
			else if(cobID == 0x283)
			{
				unsigned int status = 0;
				unsigned short current = 0;
				
				current = tempCanData[10];
				current = current<<8;
				current = current + tempCanData[9];
				robotWheel.leftBCurrent = current;
				
				status = tempCanData[8];
				status = status<<8;
				status = status + tempCanData[7];
				status = status<<8;
				status = status + tempCanData[6];
				status = status<<8;
				status = status + tempCanData[5];
				
				robotWheel.leftBAlarmCoder=status;
				robotWheel.leftBRcvPdoCount =TIMEOUT_RCV_INFOR;
			}
			else if(cobID == 0x184)
			{
				unsigned int encoder = 0;
				unsigned short status = 0;
				
				status = tempCanData[6];
				status = status<<8;
				status = status + tempCanData[5];
				robotWheel.rightBStatusCoder = status;
				
				encoder = tempCanData[12];
				encoder = encoder<<8;
				encoder = encoder + tempCanData[11];
				encoder = encoder<<8;
				encoder = encoder + tempCanData[10];
				encoder = encoder<<8;
				encoder = encoder + tempCanData[9];
				
				robotWheel.rightBEnCoder=encoder;
				robotWheel.rightBRcvPdoCount =TIMEOUT_RCV_INFOR;
				RobotVectorInvers();
			}
			else if(cobID == 0x284)
			{
				unsigned int status = 0;
				unsigned short current = 0;
				
				current = tempCanData[10];
				current = current<<8;
				current = current + tempCanData[9];
				robotWheel.rightBCurrent = current;
				
				status = tempCanData[8];
				status = status<<8;
				status = status + tempCanData[7];
				status = status<<8;
				status = status + tempCanData[6];
				status = status<<8;
				status = status + tempCanData[5];
				
				robotWheel.rightBAlarmCoder=status;
				robotWheel.rightBRcvPdoCount =TIMEOUT_RCV_INFOR;
			}
			#endif
			else if((cobID == 0x701)||(cobID == 0x702)||(cobID == 0x703)||(cobID == 0x704))//CANOPEN心跳帧
			{
				unsigned char id = cobID - 0x700;
				unsigned char status = tempCanData[5];
				//预操作模式
				if(status == 0x7f)
				{;}
				else if(status ==0x05)
				{;}
			}
		}
	}
	//robotWheel.status = STATUS_OPERATE;
	switch (robotWheel.status)
	{
		//初始模式，初始化驱动器
		case STATUS_INIT:
		{
			robotWheel.leftUnConnect=UNCONNECT_ERROR;
			robotWheel.rightUnConnect=UNCONNECT_ERROR;
			if(canOpenSendTimer >500)
			{
				canOpenSendTimer = 0;
				//取消电机使能，
				canOpenWriteSOD(0x01,MOTEC_VEL_DEMAND_VALUE,0,2,0x00);
				canOpenWriteSOD(0x02,MOTEC_VEL_DEMAND_VALUE,0,2,0x00);
				//配置驱动器1,
				if(configDeviceMotec(0x01)==1)
				{
					//成功后配置驱动器2
					if(configDeviceMotec(0x02)==1)
					{
						#ifdef	FOR_4_WHEEL
						//成功后配置驱动器3
						if(configDeviceMotec(0x03)==1)
						{
							//成功后配置驱动器4
							if(configDeviceMotec(0x04)==1)
							#endif
							{
								canOpenSendNMT(0,MOTEC_SET_OPERATE);//进入可操作模式；
								//配置完成后，电机进入可运行模式
								robotWheel.status = STATUS_OPERATE;
								robotWheel.leftUnConnect=0;
								robotWheel.rightUnConnect=0;
								robotWheel.leftBUnConnect=0;
								robotWheel.rightBUnConnect=0;
								robotWheel.leftRcvPdoCount = TIMEOUT_RCV_INFOR;
								robotWheel.rightRcvPdoCount = TIMEOUT_RCV_INFOR;
								robotWheel.leftBRcvPdoCount = TIMEOUT_RCV_INFOR;
								robotWheel.rightBRcvPdoCount = TIMEOUT_RCV_INFOR;
								canOpenSendTimer = 0;
							}
							#ifdef	FOR_4_WHEEL
							else
							{
								return;
							}
						}
						else
						{
							return;
						}
							#endif
					}
					else 
					{
						return ;
					}
				}
				else
				{
					return;
				}
			}
		}
		break;
		case STATUS_ERR_DEAL:
		{
			if(canOpenSendTimer >= 500 )
			{
				canOpenWritePDO(0x1,CTRL_RUN,0,0);
				canOpenWritePDO(0x2,CTRL_RUN,0,0);
				#ifdef	FOR_4_WHEEL
				canOpenWritePDO(0x3,CTRL_RUN,0,0);
				canOpenWritePDO(0x4,CTRL_RUN,0,0);
				#endif
				robotWheel.status = STATUS_OPERATE;
			}
		}
		break;
		case STATUS_OPERATE:
		{
			//接收信号超时；
			/*if((robotWheel.leftRcvPdoCount == 0)||(robotWheel.rightRcvPdoCount==0))
			{
				//重新配置驱动器；
				robotWheel.status = STATUS_INIT;
				if((robotWheel.leftRcvPdoCount == 0)||(robotWheel.rightRcvPdoCount == 0))
				{
					canOpenWriteSOD(0x1,MOTEC_VEL_DEMAND_VALUE,0,2,0x00);
					canOpenWriteSOD(0x2,MOTEC_VEL_DEMAND_VALUE,0,2,0x00);
					#ifdef	FOR_4_WHEEL
					canOpenWriteSOD(0x3,MOTEC_VEL_DEMAND_VALUE,0,2,0x00);
					canOpenWriteSOD(0x4,MOTEC_VEL_DEMAND_VALUE,0,2,0x00);
					#endif
				}
			}
			else*/
			{
				short intLeft,intRight,intLeftB,intRightB;
				intLeft = robotWheel.SetSpeedLeft;
				intRight = robotWheel.SetSpeedRight;
				intLeftB = robotWheel.SetSpeedLeftB;
				intRightB = robotWheel.SetSpeedRightB;
				if(canOpenSendTimer >= 100 )
				{
					canOpenSendTimer=0;
					/*if( ((robotWheel.leftStatusCoder&0x50) ==0x50)||((robotWheel.rightStatusCoder&0x50) ==0x50))
					{
						canOpenWritePDO(0x2,0x86,0,0);
						canOpenWritePDO(0x1,0x86,0,0);
						#ifdef	FOR_4_WHEEL
						canOpenWritePDO(0x4,0x86,0,0);
						canOpenWritePDO(0x3,0x86,0,0);
						#endif
						robotWheel.status = STATUS_ERR_DEAL;
						canOpenSendTimer=0;
					}
					//有报警
					else if(((robotWheel.leftStatusCoder & STATUS_ALARM)!=0)||((robotWheel.rightStatusCoder & STATUS_ALARM)!=0))
					{
						#ifdef	FOR_4_WHEEL
						canOpenWritePDO(0x4,0x80,0,0);
						canOpenWritePDO(0x3,0x80,0,0);
						#endif
						canOpenWritePDO(0x2,0x80,0,0);
						canOpenWritePDO(0x1,0x80,0,0);
					}
					else */
					{
						//#ifdef	FOR_4_WHEEL
						//canOpenWritePDO(0x4,CTRL_RUN,intRightB,0);
						//canOpenWritePDO(0x3,CTRL_RUN,-intLeftB,0);
						//#endif
						//加减速
						
						#ifdef	FOR_4_WHEEL
						intLeft = addSpeed(intLeft,intLeftLast);
						intRight = addSpeed(intRight,intRightLast);
						intLeftB = addSpeed(intLeftB,intLeftBLast);
						intRightB = addSpeed(intRightB,intRightBLast);
						intLeftLast = intLeft;
						intRightLast = intRight;
						intLeftBLast = intLeftB;
						intRightBLast = intRightB;
						//canOpenWritePDO(0x1,-intLeft*100,intRight*100,0);
						canOpenWritePDO(0x1,CTRL_RUN,-intLeft,0);
						canOpenWritePDO(0x2,CTRL_RUN,intRight,0);
						canOpenWritePDO(0x3,CTRL_RUN,-intLeftB,0);
						canOpenWritePDO(0x4,CTRL_RUN,intRightB,0);
						//canOpenWritePDO(0x3,-intLeft,intRight,0);
						//canOpenWritePDO(0x4,-intLeft,intRight,0);
						#else
						canOpenWritePDO(0x1,CTRL_RUN,intLeft,0);
						canOpenWritePDO(0x2,CTRL_RUN,-intRight,0);
						canOpenWritePDO(0x3,CTRL_RUN,intLeftB,0);
						canOpenWritePDO(0x4,CTRL_RUN,-intRightB,0);
						#endif
					}
				}
			}
		}
		break;
	}
}

short addSpeed(short cS,short lS)
{
	short tempCS = cS;
	short tempLS = lS;
	unsigned short absTempCS = 0;
	unsigned short absTempLS = 0;
	if((tempCS * tempLS) < 0 )
	{
		tempCS = 0;
	}
	absTempCS = abs(tempCS);
	absTempLS = abs(tempLS);
	if(absTempCS > absTempLS)
	{
		if((absTempCS - absTempLS) > 5 )
		{
			if(tempCS > 0)
			{
				return (tempLS + 5);
			}
			else if(tempCS < 0)
			{
				return (tempLS - 5);
			}
			else
			{
				return tempCS;
			}
		}
		else
		{
			return tempCS;
		}
	}
	else if(absTempCS < absTempLS)
	{
		if((absTempLS - absTempCS) > 5 )
		{
			if(tempLS > 0)
			{
				return (tempLS - 5);
			}
			else if(tempLS < 0)
			{
				return (tempLS + 5);
			}
			else
			{
				return tempCS;
			}
		}
		else
		{
			return tempCS;
		}
	}
	else
	{
		return tempCS;
	}
}
unsigned char getRobotWheelError(void)
{
	#if	0
	if((robotWheel.leftUnConnect==UNCONNECT_ERROR)||(robotWheel.rightUnConnect==UNCONNECT_ERROR))
	{
		return 1;
	}
	if((robotWheel.leftAlarmCoder!=0)||(robotWheel.rightAlarmCoder!=0))
	{
		return 2;
	}
	#ifdef	FOR_4_WHEEL
	if((robotWheel.leftBUnConnect==UNCONNECT_ERROR)||(robotWheel.rightBUnConnect==UNCONNECT_ERROR))
	{
		return 1;
	}
	if((robotWheel.leftBAlarmCoder!=0)||(robotWheel.rightBAlarmCoder!=0))
	{
		return 2;
	}
	#endif
	#endif
	return 0;
}
