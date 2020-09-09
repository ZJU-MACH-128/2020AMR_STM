#ifndef __BASEROBOT_H__
#define __BASEROBOT_H__
#define	BUTTON_CLICK			1				//��ť����
#define	BUTTON_RELEASE			1				//��ť�ͷ�

#define	ROBOT_STATUS_STOP		0				//ֹͣ
#define	ROBOT_STATUS_START		1				//����
#define	ROBOT_ERROR_NULL		0				//�޹���

#define	ROBOT_AHEADDAR_NULL				0xff			//ǰ�״���Ԥ��
#define	ROBOT_AHEADDAR_ENTRY			3				//���ϰ������Ԥ������
#define	ROBOT_AHEADDAR_LIMITE			2				//���ϰ��������������
#define	ROBOT_AHEADDAR_EMGY				1				//���ϰ��￿������ֹͣ����
#define	ROBOT_AHEADDAR_SPEED_LIMITE		20				//���ϰ������������������

#define	ROBOT_REARRADAR_NULL			0xff			//���ϰ������������������
#define	ROBOT_REARRADAR_ENTRY			3				//���ϰ������Ԥ������
#define	ROBOT_REARRADAR_LIMITE			2				//���ϰ��������������
#define	ROBOT_REARRADAR_EMGY			1				//���ϰ��￿������ֹͣ����
#define	ROBOT_REARRADAR_SPEED_LIMITE	20				//���ϰ������������������

typedef struct
{
    unsigned char aheadWarningLevel;		//ǰ���״�Ԥ���ȼ�
    unsigned char aheadWarningErr;			//ǰ���״���չ���

    unsigned char rearWarningLevel[4];			//���״�Ԥ���ȼ�
    unsigned char rearWarningErr;			//���״���չ���
	
	unsigned char emergencyStopStatus;		//��ͣ��ť״̬
	unsigned char emergencyStopFlag;		//��ͣ��ť�б仯
	
	
	unsigned char startButtonStatus;		//������ť״̬
	unsigned char startButtonFlag;			//������ť�б仯
	
	unsigned char stopButtonStatus;			//ֹͣ��ť״̬
	unsigned char stopButtonFlag;			//ֹͣ��ť�б仯
	
	unsigned int BatteryVoltage;			//��ص�ѹ
	unsigned int BatteryCurrent;			//��ص���
	unsigned int BatteryRemainCapacity;		//���ʣ������
	unsigned int BatteryPercentage;			//���ʣ�������ٷֱ�
	unsigned int BatteryStatus;				//���״̬
	unsigned int BatteryDevError;			//��ؽ��չ���
	
	double RosXVel;							//���յ�Ros��X���ٶ�	����/��
	double RosYVel;							//���յ�Ros��Y���ٶ�	����/��
	double RosZVel;							//���յ�Ros�Ľ��ٶ�		����/��
    unsigned char RosDevError;				//����Ros���ݹ���
	
	double XVelAct;						//ʵ��ͨ��״̬����֮��X���ٶ�	����/��
	double YVelAct;						//ʵ��ͨ��״̬����֮��Y���ٶ�	����/��
	double ZVelAct;						//ʵ��ͨ��״̬����֮��Ľ��ٶ�	����/��
	
	unsigned char Lamp;					//ʵ��ͨ��״̬����֮��ָʾ�Ƶ�״̬��
	
	double MotoAXisRate[4];				//���ת�٣�
	double MotoAXisTemp[4];				//��ʱ������
	
	double XAxisSum;
	double YAxisSum;
	double ZAxisSum;
		
	unsigned short 	SysStatus;
	unsigned char 	Base_Recevice_data_confirm;
	unsigned char 	RosSubCmd;
	unsigned char 	Feedback;
	
	
	unsigned char 	RobotStatus;		//����״̬	0//STOP		1//START
	unsigned char 	RobotError;			//�������ϣ���λ��
}Struct_Robot_Deal;

extern Struct_Robot_Deal robotDeal;

void AheadRadarFrameCallback( unsigned char status );
void ButtonFrameCallback( unsigned char button, unsigned char status );
void LiBatteryFrameCallback( void *data );
void rearRadarFrameCallback( void *data );
void RosDevFrameCallback( void *data );
#endif
