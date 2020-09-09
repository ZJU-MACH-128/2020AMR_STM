#ifndef __USART4SONAR_H__
#define __USART4SONAR_H__

//用户数据接口
typedef struct
{
	unsigned char updateFlag;
	unsigned short distance[8];
}Struct_Usr_RearRadar;

static unsigned char rearRadarRcvInt(unsigned char data);
void SonarPartInit(void);
void __cycSonarDeal(void);
#endif
