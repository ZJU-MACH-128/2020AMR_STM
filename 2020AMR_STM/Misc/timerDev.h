#ifndef	_TIMERDEV_H_
#define	_TIMERDEV_H_
typedef struct 
{
	__int64 Tick;
	unsigned int Times10MS;
	unsigned int TimesDelay;
	void (*p1) (void);
	void (*p2) (void);
	void (*p3) (void);
	void (*p4) (void);
	void (*p5) (void);
	void (*p1_) (void);
	void (*p2_) (void);
	void (*p3_) (void);
	void (*p4_) (void);
	void (*p5_) (void);
}Struct_User_Timer;

extern Struct_User_Timer timer;

void Delayms(unsigned int t);
__int64 getSystemTick(void);
int setTimerCallback( void (*p) (void) );
int setTimerCallback10( void (*p) (void) );
void timerDevInit(void);
#endif

