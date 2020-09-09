#ifndef __BASEROBOT_H__
#define __BASEROBOT_H__
#define	BUTTON_CLICK			1				//按钮按下
#define	BUTTON_RELEASE			1				//按钮释放

#define	ROBOT_STATUS_STOP		0				//停止
#define	ROBOT_STATUS_START		1				//运行
#define	ROBOT_ERROR_NULL		0				//无故障

#define	ROBOT_AHEADDAR_NULL				0xff			//前雷达无预警
#define	ROBOT_AHEADDAR_ENTRY			3				//有障碍物进入预警区域
#define	ROBOT_AHEADDAR_LIMITE			2				//有障碍物进入限速区域
#define	ROBOT_AHEADDAR_EMGY				1				//有障碍物靠近紧急停止区域
#define	ROBOT_AHEADDAR_SPEED_LIMITE		20				//有障碍物进入限速区域限速

#define	ROBOT_REARRADAR_NULL			0xff			//有障碍物进入限速区域限速
#define	ROBOT_REARRADAR_ENTRY			3				//有障碍物进入预警区域
#define	ROBOT_REARRADAR_LIMITE			2				//有障碍物进入限速区域
#define	ROBOT_REARRADAR_EMGY			1				//有障碍物靠近紧急停止区域
#define	ROBOT_REARRADAR_SPEED_LIMITE	20				//有障碍物进入限速区域限速

typedef struct
{
    unsigned char aheadWarningLevel;		//前方雷达预警等级
    unsigned char aheadWarningErr;			//前方雷达接收故障

    unsigned char rearWarningLevel[4];			//后方雷达预警等级
    unsigned char rearWarningErr;			//后方雷达接收故障
	
	unsigned char emergencyStopStatus;		//急停按钮状态
	unsigned char emergencyStopFlag;		//急停按钮有变化
	
	
	unsigned char startButtonStatus;		//启动按钮状态
	unsigned char startButtonFlag;			//启动按钮有变化
	
	unsigned char stopButtonStatus;			//停止按钮状态
	unsigned char stopButtonFlag;			//停止按钮有变化
	
	unsigned int BatteryVoltage;			//电池电压
	unsigned int BatteryCurrent;			//电池电流
	unsigned int BatteryRemainCapacity;		//电池剩余容量
	unsigned int BatteryPercentage;			//电池剩余容量百分比
	unsigned int BatteryStatus;				//电池状态
	unsigned int BatteryDevError;			//电池接收故障
	
	double RosXVel;							//接收到Ros的X向速度	厘米/秒
	double RosYVel;							//接收到Ros的Y向速度	厘米/秒
	double RosZVel;							//接收到Ros的角速度		弧度/秒
    unsigned char RosDevError;				//接收Ros数据故障
	
	double XVelAct;						//实际通过状态处理之后X向速度	厘米/秒
	double YVelAct;						//实际通过状态处理之后Y向速度	厘米/秒
	double ZVelAct;						//实际通过状态处理之后的角速度	弧度/秒
	
	unsigned char Lamp;					//实际通过状态处理之后指示灯的状态；
	
	double MotoAXisRate[4];				//轴的转速；
	double MotoAXisTemp[4];				//临时变量；
	
	double XAxisSum;
	double YAxisSum;
	double ZAxisSum;
		
	unsigned short 	SysStatus;
	unsigned char 	Base_Recevice_data_confirm;
	unsigned char 	RosSubCmd;
	unsigned char 	Feedback;
	
	
	unsigned char 	RobotStatus;		//机器状态	0//STOP		1//START
	unsigned char 	RobotError;			//机器故障，按位；
}Struct_Robot_Deal;

extern Struct_Robot_Deal robotDeal;

void AheadRadarFrameCallback( unsigned char status );
void ButtonFrameCallback( unsigned char button, unsigned char status );
void LiBatteryFrameCallback( void *data );
void rearRadarFrameCallback( void *data );
void RosDevFrameCallback( void *data );
#endif
