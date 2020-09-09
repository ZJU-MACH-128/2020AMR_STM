#include <math.h>
#include "stm32f4xx.h"
#include "diffRobot2.h"
#include "myDef.h"
#include "baseRobot.h"
#include "dealMotec.h"

Struct_Robot_Size robotSize;
void paramDeal(void)
{
    robotSize.Encoder_data1 = 16384;		//电机一圈编码器数；
    robotSize.Ratio_data1 = 22;				//减速比：轮胎一圈电机转的圈数
    robotSize.LCJ_data1 = 0x0001;			//轮胎直径补偿
    robotSize.Tyre_diameter_data=200;		//轮胎直径：毫米
	robotSize.Wheel_LR_data=460;			//左右轮子之间距：毫米
	robotSize.Wheel_FB_data=460;			//前后轮子之间距：毫米		

	robotSize.data_r_e2=3.1415926*robotSize.Tyre_diameter_data*(robotSize.LCJ_data1*0.01+1)/(robotSize.Encoder_data1*robotSize.Ratio_data1); 	//一个编码器脉冲对应的距离（mm）
    robotSize.data_r_e1=(1/3.1415926)/(0.1*robotSize.Tyre_diameter_data)*robotSize.Ratio_data1;	      					//轮胎前行1CM需要的电机转动转数；
	robotSize.data_r_e1=robotSize.data_r_e1*60;	//转化为 转/分;
}

//差分轮
void RobotVectorAnalysis( void )	// 底盘矢量解析
{
    double Axis_VelExpTemp[4];
	
    robotDeal.ZVelAct = robotDeal.ZVelAct * 0.0174533;
	
	Axis_VelExpTemp[0] = robotDeal.XVelAct - robotDeal.ZVelAct * robotSize.Wheel_LR_data  / 20.0;//62=两轮中心距（CM)
	Axis_VelExpTemp[1] = robotDeal.XVelAct + robotDeal.ZVelAct * robotSize.Wheel_LR_data  / 20.0;

    robotDeal.MotoAXisRate[0] = (robotSize.data_r_e1) * Axis_VelExpTemp[0]; 						//直流电机顺时针旋转为正。
    robotDeal.MotoAXisRate[1] = (robotSize.data_r_e1) * Axis_VelExpTemp[1]; 						//15.17 = 1 /(33 * 3.14) * 96 * 512 * 4 /1000 *8；单位转换，
    robotDeal.MotoAXisRate[2] = robotDeal.MotoAXisTemp[0];   										//由期望速度(cm/s)转为AB相编码器的量纲(8ms电机运行的脉冲数)；
    robotDeal.MotoAXisRate[3] = robotDeal.MotoAXisTemp[1];
}

void RobotVectorInvers(void)  //底盘矢量反解
{
	signed short 	Vel_act[4];
    double 	Pos_Act_MM[4];
	
	double 	dxy_ave = 0;
	double 	dth = 0;
	
	double 	XAxis = 0;
	double 	YAxis = 0;
	double 	ZAxis = 0;

    Vel_act[0] = robotWheel.leftEnCoder - robotWheel.PosAct[0];		//脉冲数改变量
    robotWheel.PosAct[0] = robotWheel.leftEnCoder;

    Vel_act[1] = robotWheel.rightEnCoder - robotWheel.PosAct[1];
    robotWheel.PosAct[1] = robotWheel.rightEnCoder;

    Pos_Act_MM[0] = (robotSize.data_r_e2)* Vel_act[0];  //轮子走过的距离（mm）
    Pos_Act_MM[1] = (robotSize.data_r_e2)* Vel_act[1];

	dxy_ave = (Pos_Act_MM[1] + Pos_Act_MM[0]) / 2.0;		//机器人坐标系x方向位移（mm）
	dth = (Pos_Act_MM[1] - Pos_Act_MM[0]) / robotSize.Wheel_LR_data;	//转过的角度，逆时针为正

	XAxis = cos(dth) * dxy_ave;
	YAxis = -sin(dth) * dxy_ave;
    ZAxis = dth * 572.9578;//572.9578=10*180/3.1415926，角度的十倍

    robotDeal.XAxisSum += XAxis;
    robotDeal.YAxisSum += YAxis;
    robotDeal.ZAxisSum += ZAxis;
}
