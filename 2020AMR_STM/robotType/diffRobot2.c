#include <math.h>
#include "stm32f4xx.h"
#include "diffRobot2.h"
#include "myDef.h"
#include "baseRobot.h"
#include "dealMotec.h"

Struct_Robot_Size robotSize;
void paramDeal(void)
{
    robotSize.Encoder_data1 = 16384;		//���һȦ����������
    robotSize.Ratio_data1 = 22;				//���ٱȣ���̥һȦ���ת��Ȧ��
    robotSize.LCJ_data1 = 0x0001;			//��ֱ̥������
    robotSize.Tyre_diameter_data=200;		//��ֱ̥��������
	robotSize.Wheel_LR_data=460;			//��������֮��ࣺ����
	robotSize.Wheel_FB_data=460;			//ǰ������֮��ࣺ����		

	robotSize.data_r_e2=3.1415926*robotSize.Tyre_diameter_data*(robotSize.LCJ_data1*0.01+1)/(robotSize.Encoder_data1*robotSize.Ratio_data1); 	//һ�������������Ӧ�ľ��루mm��
    robotSize.data_r_e1=(1/3.1415926)/(0.1*robotSize.Tyre_diameter_data)*robotSize.Ratio_data1;	      					//��̥ǰ��1CM��Ҫ�ĵ��ת��ת����
	robotSize.data_r_e1=robotSize.data_r_e1*60;	//ת��Ϊ ת/��;
}

//�����
void RobotVectorAnalysis( void )	// ����ʸ������
{
    double Axis_VelExpTemp[4];
	
    robotDeal.ZVelAct = robotDeal.ZVelAct * 0.0174533;
	
	Axis_VelExpTemp[0] = robotDeal.XVelAct - robotDeal.ZVelAct * robotSize.Wheel_LR_data  / 20.0;//62=�������ľࣨCM)
	Axis_VelExpTemp[1] = robotDeal.XVelAct + robotDeal.ZVelAct * robotSize.Wheel_LR_data  / 20.0;

    robotDeal.MotoAXisRate[0] = (robotSize.data_r_e1) * Axis_VelExpTemp[0]; 						//ֱ�����˳ʱ����תΪ����
    robotDeal.MotoAXisRate[1] = (robotSize.data_r_e1) * Axis_VelExpTemp[1]; 						//15.17 = 1 /(33 * 3.14) * 96 * 512 * 4 /1000 *8����λת����
    robotDeal.MotoAXisRate[2] = robotDeal.MotoAXisTemp[0];   										//�������ٶ�(cm/s)תΪAB�������������(8ms������е�������)��
    robotDeal.MotoAXisRate[3] = robotDeal.MotoAXisTemp[1];
}

void RobotVectorInvers(void)  //����ʸ������
{
	signed short 	Vel_act[4];
    double 	Pos_Act_MM[4];
	
	double 	dxy_ave = 0;
	double 	dth = 0;
	
	double 	XAxis = 0;
	double 	YAxis = 0;
	double 	ZAxis = 0;

    Vel_act[0] = robotWheel.leftEnCoder - robotWheel.PosAct[0];		//�������ı���
    robotWheel.PosAct[0] = robotWheel.leftEnCoder;

    Vel_act[1] = robotWheel.rightEnCoder - robotWheel.PosAct[1];
    robotWheel.PosAct[1] = robotWheel.rightEnCoder;

    Pos_Act_MM[0] = (robotSize.data_r_e2)* Vel_act[0];  //�����߹��ľ��루mm��
    Pos_Act_MM[1] = (robotSize.data_r_e2)* Vel_act[1];

	dxy_ave = (Pos_Act_MM[1] + Pos_Act_MM[0]) / 2.0;		//����������ϵx����λ�ƣ�mm��
	dth = (Pos_Act_MM[1] - Pos_Act_MM[0]) / robotSize.Wheel_LR_data;	//ת���ĽǶȣ���ʱ��Ϊ��

	XAxis = cos(dth) * dxy_ave;
	YAxis = -sin(dth) * dxy_ave;
    ZAxis = dth * 572.9578;//572.9578=10*180/3.1415926���Ƕȵ�ʮ��

    robotDeal.XAxisSum += XAxis;
    robotDeal.YAxisSum += YAxis;
    robotDeal.ZAxisSum += ZAxis;
}
