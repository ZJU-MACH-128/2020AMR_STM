#ifndef	_STATUSLAMP_H_
#define	_STATUSLAMP_H_
#define		UNLIGHT				0
#define		RED_LAMP			1
#define		GREEN_LAMP			2
#define		YELLOW_LAMP			3
#define		RED_LAMP_FLASH		4
#define		GREEN_LAMP_FLASH	5
#define		YELLOW_LAMP_FLASH	6
typedef struct 
{
	__int64 Tick;
	unsigned int LightTime;
	unsigned int UnLightTime;
	unsigned char LightNum;
	unsigned char LightType;
	unsigned char LastLight;
	
	__int64 TickBoardLamp;
	unsigned char BoardLamp;
	unsigned int BoardFlashTime;
}Struct_User_StatusLamp;

void SetStatusLamp( unsigned char Light );
void commLampDeal(void);
void StatusLampInit(void);
void __cycStatusLampDeal(void);
#endif
