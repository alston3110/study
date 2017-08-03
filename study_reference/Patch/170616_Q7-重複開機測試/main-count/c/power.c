#define DEF_IS_POWER_C
#include "common.h"

/* ---------------------------------------------------------------------
 * Define
--------------------------------------------------------------------- */
enum __POWER_STATE__ {
	PROC_POWER_IDLE   ,
	PROC_POWER_ON     ,
	PROC_POWER_OFF    ,
	PROC_POWER_BUTTON ,
	PROC_POWER_RESET  ,
} ;

//cycle => 5300 => 1Sec
#define DEF_POWER_BUTTON_POWER_OFF_DETECTION_TIME   5300
#define DEF_POWER_BUTTON_POWER_ON_DETECTION_TIME    5300


/* ---------------------------------------------------------------------
 * Value
--------------------------------------------------------------------- */
uint32_t u32PowerProcState     = 0 ;
uint32_t u32PowerProcCount     = 0 ;
uint32_t u32PowerProcNextState = 0 ;
uint32_t u32PowerRTCCount      = 0 ;
uint32_t u32Count ;

bool     bPowerCanOnFlag       = false ;
bool     bPowerStatus          = OFF   ;
bool     bPowerSource          = DEF_COMMON_POWER_SOURCE_CELL_BATTERY ;
bool     bPowerFunction        = OFF ;

bool     bPowerButtonGetStore  = 0     ;
bool     bPowerButtonGet       = 0     ;
bool     bPowerButtonEvent     = 0     ;

#ifdef DEF_COMMON_WDOG_FUNCTION
	bool     bPowerWdogOnFlag      = DEF_COMMON_WDOG_ONOFF ;
	uint32_t u32PowerWdogSetSec    = DEF_COMMON_WDOG_DEFAULT ;
	uint32_t u32PowerWdogCount     = 0 ;

	uint32_t u32PowerWdogSetRetryCount = DEF_COMMON_WDOG_RETRY_DEFAULT;
	uint32_t u32PowerWdogRetryCount = 0;
#endif

/* ---------------------------------------------------------------------
 * Function
--------------------------------------------------------------------- */
void vPower_DefaultInit( void )
{
	u32PowerProcState     = 0 ;
	u32PowerProcCount     = 0 ;
	u32PowerProcNextState = 0 ;
	bPowerButtonGetStore  = (!u32GPIO_Read( DEF_GPIO_POWER_BUTTON )) ;
	bPowerButtonEvent     = 0 ;
}

void vPower_SetPowerNeedOn( void )
{
	bPowerCanOnFlag = true ;
}

void vPower_SetAction( uint32_t u32Act )
{
	switch( u32Act )
	{
		case 0 :
			//power off
			u32PowerProcNextState = PROC_POWER_OFF ;
			break ;
		case 1 :
			//do reset
			u32PowerProcNextState = PROC_POWER_RESET ;
			break ;
		case 2 :
			bPowerFunction = ON  ;
			break ;
		case 3 :
			bPowerFunction = OFF ;
			break ;
	}
}

#ifdef DEF_COMMON_WDOG_FUNCTION
void vPower_SetWdog_Time( uint32_t u32hsec, uint32_t u32lsec )
{
	uint32_t u32Act = (u32hsec << 8) + u32lsec;
	if(u32Act == 0){
		bPowerWdogOnFlag = false;
		u32PowerWdogCount = 0;
		u32PowerWdogSetSec = u32Act;

		u32PowerWdogRetryCount = 0;
	}else{
		u32PowerWdogCount = 0;
		u32PowerWdogSetSec = u32Act;
		bPowerWdogOnFlag = true;
	}
}

void vPower_SetWdog_Times( uint32_t u32count )
{
	u32PowerWdogSetRetryCount = u32count;
}

#endif

void vPower_OffControl( void )
{
	vGPIO_Write( DEF_GPIO_POR , LOW ) ;
	vGPIO_Write( DEF_GPIO_POWER_ON , LOW ) ;
	vGPIO_Write( DEF_GPIO_PMIC_ON_OFF_REQ , LOW ) ;
	mI2C_Disable( ) ;
	bPowerStatus    = OFF ;
	bPowerCanOnFlag = false ;
}

void vPower_Idle( void )
{
	/*
	 * Check if need power on
	 */
	if ( bPowerCanOnFlag )
	{
		mCommon_DisableIrq( ) ;
		bPowerCanOnFlag = false ;
		mCommon_EnableIrq( ) ;
		/*
		 * Is power on or off now
		 * If power off , need to open power
		 */
		if ( bPowerStatus == OFF )
		{
			#ifdef DEF_COMMON_WDOG_FUNCTION
				bPowerWdogOnFlag = DEF_COMMON_WDOG_ONOFF;
				u32PowerWdogCount = 0;
				u32PowerWdogSetSec = DEF_COMMON_WDOG_DEFAULT;
				u32PowerWdogSetRetryCount = DEF_COMMON_WDOG_RETRY_DEFAULT;
				u32PowerWdogRetryCount = 0;
			#endif
			u32PowerProcState   = PROC_POWER_ON ;
			u32PowerProcCount   = 0 ;
			return ;
		}
	}
	
	if ( bPowerButtonEvent )
	{
		u32PowerProcState   = PROC_POWER_BUTTON ;
		u32PowerProcCount   = 0 ;
		bPowerButtonEvent   = false ;
		return ;
	}

	if ( u32PowerProcNextState )
	{
		u32PowerProcState     = u32PowerProcNextState ;
		u32PowerProcCount     = 0 ;
		u32PowerProcNextState = 0 ;
		return ;
	}

	#ifdef DEF_COMMON_WDOG_FUNCTION
	if ( bPowerStatus )
	{
		u32PowerWdogCount ++ ;
		if ((u32PowerWdogCount > ( u32PowerWdogSetSec * 5300 )) && bPowerWdogOnFlag)
		{
			u32PowerWdogRetryCount ++;
			if(u32PowerWdogRetryCount > u32PowerWdogSetRetryCount){
				u32PowerProcState     = PROC_POWER_OFF ;
			}else{
				u32PowerProcState     = PROC_POWER_RESET ;
			}
			u32PowerProcCount     = 0 ;
			u32PowerProcNextState = 0 ;
			u32PowerWdogCount = 0 ;
			return ;
		}
	}
	else
	{
		bPowerWdogOnFlag = false;
		u32PowerWdogCount = 0;
		u32PowerWdogSetSec = DEF_COMMON_WDOG_DEFAULT;
		u32PowerWdogSetRetryCount = DEF_COMMON_WDOG_RETRY_DEFAULT;
		u32PowerWdogRetryCount = 0;
	}

	#endif

}

void vPower_On( void )
{
	u32PowerProcCount ++ ;
	
	if ( u32PowerProcCount == ( DEF_COMMON_FOR_LOOP_TIME_1MS * 10 ) )
	{
		vGPIO_Write( DEF_GPIO_POWER_ON , HIGH ) ;		//step 1 - PowerOn
	}
	if ( u32PowerProcCount == ( DEF_COMMON_FOR_LOOP_TIME_1MS * 28 ) )
	{
		vGPIO_Write( DEF_GPIO_PMIC_ON_OFF_REQ , HIGH ) ;	//step 2 - delay 18msec (HW request)
	}	
	if ( u32PowerProcCount == ( DEF_COMMON_FOR_LOOP_TIME_1MS * 600 ) )
	{
		vGPIO_Write( DEF_GPIO_POR , HIGH ) ;			//step 3 - POR delay 600msec (HW request)
	}
	
	if ( u32PowerProcCount == ( DEF_COMMON_FOR_LOOP_TIME_1MS * 700 ) )
	{
		if( bADC_PMICVoltageIsOK( ) )				//step 4 - Detect PMIC
		{
			mI2C_Enable( ) ;
			bPowerStatus = ON ;
			u32PowerProcState   = PROC_POWER_IDLE ;
			u32PowerProcCount   = 0 ;
			return ;
		}
		vGPIO_Write( DEF_GPIO_POWER_ON , LOW ) ;
		vGPIO_Write( DEF_GPIO_PMIC_ON_OFF_REQ , LOW ) ;
	}
	
	if ( u32PowerProcCount >= ( DEF_COMMON_FOR_LOOP_TIME_1MS * 750 ) )
	{
		u32PowerProcState   = PROC_POWER_OFF ;
		u32PowerProcCount   = 0 ;
		return ;
	}
}

void vPower_Off( void )
{
	#ifdef DEF_COMMON_WDOG_FUNCTION
		bPowerWdogOnFlag = false;
		u32PowerWdogCount = 0;
		u32PowerWdogSetSec = DEF_COMMON_WDOG_DEFAULT;
		u32PowerWdogSetRetryCount = DEF_COMMON_WDOG_RETRY_DEFAULT;
		u32PowerWdogRetryCount = 0;
	#endif
	vPower_OffControl( ) ;
	vPower_DefaultInit( ) ;
}

void vPower_Button( void )
{
	uint32_t u32RTCNowCount ;
	uint32_t u32RTCDiffCount ;
	
	if ( !bPowerButtonGet )
	{
		if ( bPowerStatus == OFF )
		{
			vGPIO_Write( DEF_GPIO_PMIC_ON_OFF_REQ , LOW ) ;
		}
		else
		{
			vGPIO_Write( DEF_GPIO_PMIC_ON_OFF_REQ , HIGH ) ;
		}
		u32PowerProcState   = PROC_POWER_IDLE ;
		u32PowerProcCount   = 0 ;
		return ;
	}
/*	
	u32RTCNowCount = mRTC_GetCount( ) ;
	
	if ( u32PowerProcCount == 0 )
	{
		vGPIO_Write( DEF_GPIO_PMIC_ON_OFF_REQ , LOW ) ;
		u32PowerRTCCount = u32RTCNowCount ;
	}
*/	
	u32PowerProcCount ++ ;
/*	
	if ( u32RTCNowCount >= u32PowerRTCCount )
	{
		u32RTCDiffCount = u32RTCNowCount - u32PowerRTCCount ;
	}
	else
	{
		u32RTCDiffCount = ( 0x1000000 - u32PowerRTCCount ) + u32RTCNowCount ;
	}
*/
//use cycle times 
	if ( bPowerStatus == OFF )
	{
		if ( u32PowerProcCount >= DEF_POWER_BUTTON_POWER_ON_DETECTION_TIME )
		{
			#ifdef DEF_COMMON_WDOG_FUNCTION
				bPowerWdogOnFlag = DEF_COMMON_WDOG_ONOFF;
				u32PowerWdogCount = 0;
				u32PowerWdogSetSec = DEF_COMMON_WDOG_DEFAULT;
				u32PowerWdogSetRetryCount = DEF_COMMON_WDOG_RETRY_DEFAULT;
				u32PowerWdogRetryCount = 0;
			#endif
			u32PowerProcState   = PROC_POWER_ON ;
			u32PowerProcCount   = 0 ;
		}
	}
	else
	{
		if ( u32PowerProcCount >= DEF_POWER_BUTTON_POWER_OFF_DETECTION_TIME )
		{
		#ifdef DEF_COMMON_WDOG_FUNCTION
			bPowerWdogOnFlag = false;
			u32PowerWdogCount = 0;
			u32PowerWdogSetSec = DEF_COMMON_WDOG_DEFAULT;
			u32PowerWdogSetRetryCount = DEF_COMMON_WDOG_RETRY_DEFAULT;
			u32PowerWdogRetryCount = 0;
		#endif
			u32PowerProcState   = PROC_POWER_OFF ;
			u32PowerProcCount   = 0 ;
		}
	}
}

void vPower_Reset( void )
{
	if ( u32PowerProcCount == 0 )
	{
		vPower_OffControl( ) ;
	}
	
	u32PowerProcCount ++ ;
	#ifdef DEF_COMMON_WDOG_FUNCTION
		u32PowerWdogSetSec = DEF_COMMON_WDOG_DEFAULT;
	#endif
	
	if ( u32PowerProcCount == ( DEF_COMMON_FOR_LOOP_TIME_1MS * 1000 ) )
	{
		u32PowerProcState   = PROC_POWER_ON ;
		u32PowerProcCount   = 0 ;
	}
}

void vPower_Process( void )
{
	if( bPowerSource != DEF_COMMON_POWER_SOURCE_DC )
	{
		return ;
	}

	bPowerButtonGet = (!u32GPIO_Read( DEF_GPIO_POWER_BUTTON )) ;
	if ( bPowerButtonGetStore != bPowerButtonGet )
	{
		bPowerButtonGetStore = bPowerButtonGet ;
		bPowerButtonEvent = true ;
	}
	
	switch( u32PowerProcState )
	{
		case PROC_POWER_IDLE   : vPower_Idle( ) ;   break ;
		case PROC_POWER_ON     : vPower_On( ) ;     break ;
		case PROC_POWER_OFF    : vPower_Off( ) ;    break ;
		case PROC_POWER_BUTTON : vPower_Button( ) ; break ;
		case PROC_POWER_RESET  : vPower_Reset( ) ;  break ;
		default :
			u32PowerProcState = PROC_POWER_IDLE ;
			u32PowerProcCount = 0 ;
			break ;
	}
}
