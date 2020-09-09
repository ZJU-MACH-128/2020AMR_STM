#include "mydef.h"
#include "stdlib.h"
#include "statusLamp.h"
#include "HMIDev.h"
#include "LiBattery.h"
#include "rearRadar.h"
#include "ROSDev.h"
#include "Button.h"
#include "AheadRadar.h"
#include "timerDev.h"
#include "diffRobot2.h"
int main(void)
{
                                                                                                                                                                                                                                                              	paramDeal();
	timerDevInit();
	Delayms( 1000 );
	StatusLampInit();	
	HMIDevInit();
	ButtonInit();
	AheadRadarInit();
	SonarPartInit();
	canOpen_Init();
	LiBatteryInit();  
	MainControllerInit();	
    
	while(1)
    {
		__cycStatusLampDeal();
		__cycSonarDeal();
		__cycLiPowerDeal();
		__cycMainControllerDeal();
		__cycHMIDevDeal(); 
		baseRobotDeal();		

		Deal_WheelAxis();
		dealRobotWheel();
    }
}
