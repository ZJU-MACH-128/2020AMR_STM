#include "math.h"
#include "baseRobot.h"
#include "stdlib.h"
#include "dealMotec.h"
#include "mydef.h"
#include "statusLamp.h"
#include "HMIDev.h"
#include "LiBattery.h"
#include "rearRadar.h"
#include "ROSDev.h"
#include "Button.h"
#include "AheadRadar.h"
#include "timerDev.h"
#include "baseRobot.h"
Struct_Robot_Deal robotDeal;

unsigned char robotWheelError(void)
{
	if(getRobotWheelError()!=0)
	{
		robotDeal.SysStatus |= 0x4000;
		//WarningLed_flag |=0x0004; //CAN驱动器故障
	}
	else
	{
		robotDeal.SysStatus &= (~0x4000);
		//WarningLed_flag &=(~0x0004); //CAN驱动器故障
	}
	return 0;
}

void sendFeedBackToRosDev(void)
{
    unsigned char 			DataTemp[17];
	short 					AxisTemp[4];

	AxisTemp[0] = (short)(100*robotDeal.XAxisSum);
	AxisTemp[1] = (short)(100*robotDeal.YAxisSum);
	AxisTemp[2] = (short)(100*robotDeal.ZAxisSum);
	
	robotDeal.XAxisSum = 0;
	robotDeal.YAxisSum = 0;
	robotDeal.ZAxisSum = 0;
	
	DataTemp[0] = 0xFE;
	DataTemp[1] = 0xEE;
	
	DataTemp[2] = 0x0A;	//数据有效
	DataTemp[3] = 0x00;	//数据类型
	DataTemp[4] = (AxisTemp[0] & 0xFF00)>>8;
	DataTemp[5] = AxisTemp[0] & 0x00FF;
	DataTemp[6] = (AxisTemp[1] & 0xFF00)>>8;
	DataTemp[7] = AxisTemp[1] & 0x00FF;
	DataTemp[8] = (AxisTemp[2] & 0xFF00)>>8;
	DataTemp[9] =  AxisTemp[2] & 0x00FF;
	DataTemp[10] = 0;//robotDeal.Base_Recevice_data_confirm;
	DataTemp[11] = 0;//(robotDeal.SysStatus& 0xFF00)>>8;
	DataTemp[12] = 0;//robotDeal.SysStatus & 0x00FF;
	
	DataTemp[13] =  0;//DataTemp[4]+DataTemp[5]+DataTemp[6]+DataTemp[7]+DataTemp[8]+ \
					DataTemp[9]+DataTemp[10]+DataTemp[11]+DataTemp[12];
	DataTemp[14] =  0;//~DataTemp[13];
	DataTemp[15] = 0xFC;
	DataTemp[16] = 0xFF;
	
	mainControlleSendData( DataTemp, sizeof(DataTemp) );
}

void Deal_WheelAxis(void)  // 运动电机控制
{    
	setSpeed(-robotDeal.MotoAXisRate[0],-robotDeal.MotoAXisRate[1],-robotDeal.MotoAXisRate[2],-robotDeal.MotoAXisRate[3],1);
}

void baseRobotDeal( void )
{
	//运行赋初值；
	robotDeal.Lamp = GREEN_LAMP;
	robotDeal.XVelAct = robotDeal.RosXVel;
	robotDeal.YVelAct = robotDeal.RosYVel;
	robotDeal.ZVelAct = robotDeal.RosZVel;
	if( robotDeal.startButtonFlag == 1 )
	{
		robotDeal.startButtonFlag = 0;
		if( robotDeal.startButtonStatus == BUTTON_CLICK )
		{
			//机器没有故障，才能启动
			if( robotDeal.RobotError == ROBOT_ERROR_NULL )
			{
				robotDeal.RobotStatus = ROBOT_STATUS_START;
			}
		}
	}	
	if( robotDeal.stopButtonFlag == 1 )
	{
		robotDeal.stopButtonFlag = 0;
		if( robotDeal.stopButtonStatus == BUTTON_CLICK )
		{
			if( 1 )
			{
				robotDeal.RobotStatus = ROBOT_STATUS_STOP;
			}
		}
	}
	//前雷达故障	后雷达故障		电池数据接收故障	Ros数据接收故障
	/*if( robotDeal.aheadWarningErr || 	\
		robotDeal.rearWarningErr || 	\
		robotDeal.BatteryDevError ||	\
		robotDeal.RosDevError || 		\
	0)*/
	if( robotDeal.RobotError != 0 )
	{
		//红灯闪烁；
		robotDeal.Lamp = RED_LAMP_FLASH;
		
		//有错误，停止运行；
		robotDeal.XVelAct = 0;
		robotDeal.YVelAct = 0;
		robotDeal.ZVelAct = 0;
	}
	else
	{
		if( robotDeal.RobotStatus == ROBOT_STATUS_START )
		{
			//绿灯闪烁；
			robotDeal.Lamp = GREEN_LAMP_FLASH;
			
			//前方有障碍物在预警区域内
			if( robotDeal.aheadWarningLevel == ROBOT_REARRADAR_ENTRY )
			{
			}
			//有障碍物靠近预警区域
			else if( robotDeal.aheadWarningLevel == ROBOT_REARRADAR_LIMITE )
			{
				//前行限速
				if( robotDeal.XVelAct > 0 )
				{
					//if( robotDeal.XVelAct > ROBOT_SPEED_LIMITE )
					{
						//robotDeal.XVelAct = ROBOT_SPEED_LIMITE;
					}
				}
			}
			//有障碍物靠近紧急停止区域
			else if( robotDeal.aheadWarningLevel == ROBOT_REARRADAR_EMGY )
			{
				//不能前行
				if( robotDeal.XVelAct > 0 )
				{
					robotDeal.XVelAct = 0;
				}
			}
		}
		else if( robotDeal.RobotStatus == ROBOT_STATUS_STOP )
		{
			//黄灯亮
			robotDeal.Lamp = YELLOW_LAMP;
			
			//停止运行；
			robotDeal.XVelAct = 0;
			robotDeal.YVelAct = 0;
			robotDeal.ZVelAct = 0;
		}
	}
	SetStatusLamp( robotDeal.Lamp );
	//线速度解析成ROBOT的各个轮子的转速
    RobotVectorAnalysis(); 
}

//接收到ROS控制器发送的完整桢信息时的回调函数
void RosDevFrameCallback( void *data )
{
	unsigned char *pdata = ( unsigned char *)data;
	unsigned char cmdType = pdata[1];
	if( cmdType == 0x01 )								//控制命令
	{
		robotDeal.RosSubCmd = pdata[2];
		if(robotDeal.RosSubCmd != 0 )
		{
			robotDeal.RosXVel = (double)(signed char)pdata[3];	//X向速度	厘米/秒
			robotDeal.RosYVel = (double)(signed char)pdata[4];	//Y向速度	厘米/秒
			robotDeal.RosZVel = (double)(signed char)pdata[5];	//角速度	弧度/秒
		}
	}
	else if( cmdType == 0x02 )							//反馈信息命令
	{
		sendFeedBackToRosDev();							//当前需要的信息拷贝到发送缓冲区；	
	}
}

//接收到后雷达发送的完整桢信息时的回调函数
void rearRadarFrameCallback( void *data )
{
	unsigned char *pdata = ( unsigned char *)data;
	robotDeal.rearWarningLevel[0] = pdata[4];
	robotDeal.rearWarningLevel[1] = pdata[5];
	robotDeal.rearWarningLevel[2] = pdata[6];
	robotDeal.rearWarningLevel[3] = pdata[7];
}
unsigned int char2int( unsigned char c1, unsigned char c2 ,unsigned char c3 ,unsigned char c4 )
{
	unsigned int toInt = 0;
	toInt = toInt+ c1;
	toInt = toInt << 8;
	toInt = toInt+ c2;
	toInt = toInt << 8;
	toInt = toInt+ c3;
	toInt = toInt << 8;
	toInt = toInt+ c4;
	return toInt;
}

//接收到电池发送的完整桢信息时的回调函数
void LiBatteryFrameCallback( void *data )
{
	unsigned char *pdata = ( unsigned char *)data;
	robotDeal.BatteryVoltage = char2int( 0,0 ,pdata[0],pdata[1]);
	robotDeal.BatteryCurrent = char2int( 0,0 ,pdata[2],pdata[3]);
	robotDeal.BatteryRemainCapacity = char2int( 0,0, pdata[4],pdata[5] );
	robotDeal.BatteryPercentage = pdata[19];
	robotDeal.BatteryStatus = pdata[20];
	
}

//接收到按钮发送的完整桢信息时的回调函数
void ButtonFrameCallback( unsigned char button, unsigned char status )
{
	if( button == 0 )
	{
		if( status == BUTTON_CLICK )
		{
			robotDeal.RobotError = robotDeal.RobotError | 0X00000001;
			robotDeal.RobotStatus = ROBOT_STATUS_STOP;
		}
		else
		{
			robotDeal.RobotError = robotDeal.RobotError & 0XFFFFFFFE;
		}
		robotDeal.emergencyStopStatus = status;
		robotDeal.emergencyStopFlag = 1;
	}
	else if( button == 1 )
	{
		robotDeal.startButtonStatus = BUTTON_CLICK;
		robotDeal.startButtonFlag = 1;
	}
	else if( button == 2 )
	{
		robotDeal.stopButtonStatus = BUTTON_CLICK;
		robotDeal.stopButtonFlag = 1;
	}
	
}

//接收到前方发送的完整桢信息时的回调函数
void AheadRadarFrameCallback( unsigned char status )
{
	robotDeal.aheadWarningLevel = status;
}
