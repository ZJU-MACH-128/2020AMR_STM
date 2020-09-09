#ifndef	__DEALMOTEC_H__
#define	__DEALMOTEC_H__

#define	UNCONNECT_ERROR		0xff
#define	DEVICETYPE_NULL		0XFFFFFFFF
#define	DEVICETYPE_EXIST	41200
#define	STATUS_INIT		0
#define STATUS_CONFIG	1
#define	STATUS_OPERATE	2
#define	STATUS_ERR_DEAL	3
#define	TIMEOUT_RCV_INFOR	1000
typedef struct
{
	unsigned char status;
	short SetSpeedLeft;
	short SetSpeedRight;
	short SetSpeedLeftB;
	short SetSpeedRightB;
	short GetSpeedLeft;
	short GetSpeedRight;
	short GetSpeedLeftB;
	short GetSpeedRighBt;
	unsigned int leftRcvPdoCount;
	unsigned int rightRcvPdoCount;
	unsigned int leftBRcvPdoCount;
	unsigned int rightBRcvPdoCount;
	unsigned char leftUnConnect;
	unsigned char rightUnConnect;
	unsigned char leftBUnConnect;
	unsigned char rightBUnConnect;
	unsigned int leftAlarmCoder;
	unsigned int rightAlarmCoder;
	unsigned int leftBAlarmCoder;
	unsigned int rightBAlarmCoder;
	unsigned int leftStatusCoder;
	unsigned int rightStatusCoder;
	unsigned int leftBStatusCoder;
	unsigned int rightBStatusCoder;
	
	int leftEnCoder;
	int rightEnCoder;
	int leftBEnCoder;
	int rightBEnCoder;
	unsigned int leftCurrent;
	unsigned int rightCurrent;
	unsigned int leftBCurrent;
	unsigned int rightBCurrent;
	int PosAct[4];
	
}canopenWheel;
extern canopenWheel robotWheel;
#endif
