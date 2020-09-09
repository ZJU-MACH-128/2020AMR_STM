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
		//WarningLed_flag |=0x0004; //CAN����������
	}
	else
	{
		robotDeal.SysStatus &= (~0x4000);
		//WarningLed_flag &=(~0x0004); //CAN����������
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
	
	DataTemp[2] = 0x0A;	//������Ч
	DataTemp[3] = 0x00;	//��������
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

void Deal_WheelAxis(void)  // �˶��������
{    
	setSpeed(-robotDeal.MotoAXisRate[0],-robotDeal.MotoAXisRate[1],-robotDeal.MotoAXisRate[2],-robotDeal.MotoAXisRate[3],1);
}

void baseRobotDeal( void )
{
	//���и���ֵ��
	robotDeal.Lamp = GREEN_LAMP;
	robotDeal.XVelAct = robotDeal.RosXVel;
	robotDeal.YVelAct = robotDeal.RosYVel;
	robotDeal.ZVelAct = robotDeal.RosZVel;
	if( robotDeal.startButtonFlag == 1 )
	{
		robotDeal.startButtonFlag = 0;
		if( robotDeal.startButtonStatus == BUTTON_CLICK )
		{
			//����û�й��ϣ���������
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
	//ǰ�״����	���״����		������ݽ��չ���	Ros���ݽ��չ���
	/*if( robotDeal.aheadWarningErr || 	\
		robotDeal.rearWarningErr || 	\
		robotDeal.BatteryDevError ||	\
		robotDeal.RosDevError || 		\
	0)*/
	if( robotDeal.RobotError != 0 )
	{
		//�����˸��
		robotDeal.Lamp = RED_LAMP_FLASH;
		
		//�д���ֹͣ���У�
		robotDeal.XVelAct = 0;
		robotDeal.YVelAct = 0;
		robotDeal.ZVelAct = 0;
	}
	else
	{
		if( robotDeal.RobotStatus == ROBOT_STATUS_START )
		{
			//�̵���˸��
			robotDeal.Lamp = GREEN_LAMP_FLASH;
			
			//ǰ�����ϰ�����Ԥ��������
			if( robotDeal.aheadWarningLevel == ROBOT_REARRADAR_ENTRY )
			{
			}
			//���ϰ��￿��Ԥ������
			else if( robotDeal.aheadWarningLevel == ROBOT_REARRADAR_LIMITE )
			{
				//ǰ������
				if( robotDeal.XVelAct > 0 )
				{
					//if( robotDeal.XVelAct > ROBOT_SPEED_LIMITE )
					{
						//robotDeal.XVelAct = ROBOT_SPEED_LIMITE;
					}
				}
			}
			//���ϰ��￿������ֹͣ����
			else if( robotDeal.aheadWarningLevel == ROBOT_REARRADAR_EMGY )
			{
				//����ǰ��
				if( robotDeal.XVelAct > 0 )
				{
					robotDeal.XVelAct = 0;
				}
			}
		}
		else if( robotDeal.RobotStatus == ROBOT_STATUS_STOP )
		{
			//�Ƶ���
			robotDeal.Lamp = YELLOW_LAMP;
			
			//ֹͣ���У�
			robotDeal.XVelAct = 0;
			robotDeal.YVelAct = 0;
			robotDeal.ZVelAct = 0;
		}
	}
	SetStatusLamp( robotDeal.Lamp );
	//���ٶȽ�����ROBOT�ĸ������ӵ�ת��
    RobotVectorAnalysis(); 
}

//���յ�ROS���������͵���������Ϣʱ�Ļص�����
void RosDevFrameCallback( void *data )
{
	unsigned char *pdata = ( unsigned char *)data;
	unsigned char cmdType = pdata[1];
	if( cmdType == 0x01 )								//��������
	{
		robotDeal.RosSubCmd = pdata[2];
		if(robotDeal.RosSubCmd != 0 )
		{
			robotDeal.RosXVel = (double)(signed char)pdata[3];	//X���ٶ�	����/��
			robotDeal.RosYVel = (double)(signed char)pdata[4];	//Y���ٶ�	����/��
			robotDeal.RosZVel = (double)(signed char)pdata[5];	//���ٶ�	����/��
		}
	}
	else if( cmdType == 0x02 )							//������Ϣ����
	{
		sendFeedBackToRosDev();							//��ǰ��Ҫ����Ϣ���������ͻ�������	
	}
}

//���յ����״﷢�͵���������Ϣʱ�Ļص�����
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

//���յ���ط��͵���������Ϣʱ�Ļص�����
void LiBatteryFrameCallback( void *data )
{
	unsigned char *pdata = ( unsigned char *)data;
	robotDeal.BatteryVoltage = char2int( 0,0 ,pdata[0],pdata[1]);
	robotDeal.BatteryCurrent = char2int( 0,0 ,pdata[2],pdata[3]);
	robotDeal.BatteryRemainCapacity = char2int( 0,0, pdata[4],pdata[5] );
	robotDeal.BatteryPercentage = pdata[19];
	robotDeal.BatteryStatus = pdata[20];
	
}

//���յ���ť���͵���������Ϣʱ�Ļص�����
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

//���յ�ǰ�����͵���������Ϣʱ�Ļص�����
void AheadRadarFrameCallback( unsigned char status )
{
	robotDeal.aheadWarningLevel = status;
}
