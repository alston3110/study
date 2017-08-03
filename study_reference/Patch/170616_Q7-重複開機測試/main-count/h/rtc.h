#ifndef DEF_IS_RTC_H
	#define DEF_IS_RTC_H
	
	#ifdef DEF_IS_RTC_C
		#define EXTERN 
	#else
		#define EXTERN extern
	#endif

	typedef struct __CALENDAR__ {
		uint8_t year   ;
		uint8_t month  ;
		uint8_t day    ;
		uint8_t hour   ;
		uint8_t minute ;
		uint8_t sec    ;
	} CALENDAR ;

	enum __RTC_ACTION__ {
		DEF_RTC_CMD_OVERFLOW ,
		DEF_RTC_CMD_CLEAR    ,
		DEF_RTC_CMD_UPDATE
	} ;


	#define mRTC_GetCount() 				(RTC->CNT)

	EXTERN void vRTC_Init( void ) ;
	
	EXTERN void vRTC_UpdateCalendar( uint32_t u32Cmd ) ;
	EXTERN void vRTC_CalculateAlarmCnt( void ) ;
	
	#undef EXTERN
#endif
