#ifndef DEF_IS_POWER_H
	#define DEF_IS_POWER_H
	
	#ifdef DEF_IS_POWER_C
		#define EXTERN 
	#else
		#define EXTERN extern
		
		EXTERN bool     bPowerSource   ;
		EXTERN bool     bPowerFunction ;
	#endif
	
	EXTERN void vPower_DefaultInit( void ) ;
	EXTERN void vPower_SetPowerNeedOn( void ) ;
	EXTERN void vPower_SetAction( uint32_t u32Act ) ;
	#ifdef DEF_COMMON_WDOG_FUNCTION
		EXTERN void vPower_SetWdog_Time( uint32_t u32hsec, uint32_t u32lsec ) ;
		EXTERN void vPower_SetWdog_Times( uint32_t u32times ) ;
		EXTERN uint32_t u32PowerWdogSetSec;
		EXTERN uint32_t u32PowerWdogSetRetryCount;
		EXTERN uint32_t u32PowerWdogRetryCount;
		EXTERN bool bPowerWdogOnFlag;
	#endif
	EXTERN void vPower_OffControl( void ) ;
	EXTERN void vPower_Process( void ) ;
	EXTERN uint32_t u32Count ;
	
	#undef EXTERN
#endif
