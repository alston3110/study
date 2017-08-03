#define DEF_IS_RTC_C
#include "common.h"

#define DEF_RTC_DEFAULT_CALENDAR 				{13,1,1,0,0,0}
#define DEF_RTC_BASE_YEAR 						1970
#define DEF_RTC_EVERY_MINUTE_SECOND             60
#define DEF_RTC_EVERY_HOUR_SECOND               3600
#define DEF_RTC_EVERY_DAY_SECOND                86400
#define DEF_RTC_ALARM_MAXIUM_DAY                180
#define DEF_RTC_ALARM_MAXIUM_DAY_SENCOND        (DEF_RTC_ALARM_MAXIUM_DAY*DEF_RTC_EVERY_DAY_SECOND)	

#define mRTC_IsLeap(mYear) 						(((mYear) % 4) == 0 && (((mYear) % 100) != 0 || ((mYear) % 400) == 0))

CALENDAR stcRTCCalendar = DEF_RTC_DEFAULT_CALENDAR ;
CALENDAR stcRTCAlarm    = DEF_RTC_DEFAULT_CALENDAR ;

volatile bool rtcOverFlowFlag=false;

uint8_t monthDay[]={31,28,31,30,31,30,31,31,30,31,30,31};

void RTC_IRQHandler( void )
{
	volatile uint32_t rtcIF = RTC->IF ;
	
	RTC->IFC = _RTC_IFC_MASK ;
	
	if ( ( rtcIF & RTC_IF_OF ) )
	{
		vRTC_UpdateCalendar( DEF_RTC_CMD_OVERFLOW ) ;
	}
	
	if ( ( rtcIF & RTC_IF_COMP1 ) && ( RTC->IEN & _RTC_IEN_COMP1_MASK ) )
	{
		if( bPowerSource == DEF_COMMON_POWER_SOURCE_DC )
		{
			vPower_SetPowerNeedOn( ) ;
		}
		RTC->IEN &= (~_RTC_IEN_COMP1_MASK ) ;
	}
}

void vRTC_Init( void )
{
	CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_LE ;
	CMU->OSCENCMD     |= CMU_OSCENCMD_LFXOEN ;
	
	while( !( CMU->STATUS & CMU_STATUS_LFXORDY ) ) ;
	
	CMU->LFCLKSEL  = CMU_LFCLKSEL_LFA_LFXO;

	CMU->LFAPRESC0 = CMU_LFAPRESC0_RTC_DIV32768 ;
	CMU->LFACLKEN0 = CMU_LFACLKEN0_RTC ;

	RTC->CNT       = 0 ;
	RTC->IFC       = RTC_IFC_COMP1 | RTC_IFC_OF ;
	RTC->IEN       = RTC_IEN_OF ;
	
	NVIC_ClearPendingIRQ( RTC_IRQn ) ;
	NVIC_EnableIRQ( RTC_IRQn ) ;
	RTC->CTRL      = RTC_CTRL_EN ;//|RTC_CTRL_DEBUGRUN;
}

void vRTC_CalendarCalculate( uint32_t cnt , CALENDAR* cal )
{
	uint32_t tempCnt=cnt+cal->sec+(cal->minute*DEF_RTC_EVERY_MINUTE_SECOND)+(cal->hour*DEF_RTC_EVERY_HOUR_SECOND);
	uint32_t sec=(tempCnt%DEF_RTC_EVERY_MINUTE_SECOND);
	uint32_t min=((tempCnt%DEF_RTC_EVERY_HOUR_SECOND)/DEF_RTC_EVERY_MINUTE_SECOND);
	uint32_t hour=((tempCnt%DEF_RTC_EVERY_DAY_SECOND)/DEF_RTC_EVERY_HOUR_SECOND);
	uint32_t day=(tempCnt/DEF_RTC_EVERY_DAY_SECOND);
	uint32_t tempDay=day+(*cal).day;
  
	cal->sec=sec;
	cal->minute=min;
	cal->hour=hour;
  
	for(;;)
	{
		volatile uint8_t month=cal->month;
		volatile uint8_t monthDayTemp=monthDay[month-1];
		if(month==2)
		{
			if(mRTC_IsLeap(DEF_RTC_BASE_YEAR+cal->year))
			{
				monthDayTemp=29;
			}
		}
		if(tempDay>monthDayTemp)
		{
			tempDay-=monthDayTemp;
			cal->month++;
			if(cal->month>12)
			{
				cal->year++;
				cal->month=1;
			}
		}
		else
		{
			cal->day=tempDay;
			break;
		}
	}
}

void vRTC_UpdateCalendar( uint32_t u32Cmd )
{
	switch( u32Cmd )
	{
		case DEF_RTC_CMD_OVERFLOW :
			vRTC_CalendarCalculate( 0x1000000 , &stcRTCCalendar ) ;
			break;
		case DEF_RTC_CMD_CLEAR    :
			RTC->CNT = 0 ;
			break;
		case DEF_RTC_CMD_UPDATE   :
			{
				uint32_t u32Cnt ;
				u32Cnt = RTC->CNT ;
				RTC->CNT = 0 ;
				vRTC_CalendarCalculate( u32Cnt , &stcRTCCalendar ) ;
			}
			break;
	} 
}

uint32_t u32RTC_CalculateCalendarDiff( CALENDAR* cal0 , CALENDAR* cal1 )
{ 
	int32_t year,month,day,hour,minute,sec;
	uint32_t totalSec=0;
	uint8_t tempMonthDay=0;
	uint8_t tempYear=0;
	uint8_t count , tempMonth ;
    
	year = cal0->year - cal1->year ;
	month=cal0->month-cal1->month;
	day=cal0->day-cal1->day;
	hour=cal0->hour-cal1->hour;
	minute=cal0->minute-cal1->minute;
	sec=cal0->sec-cal1->sec;
	if(sec<0)
	{
		sec+=60;
		minute--;
	}
	if(minute<0)
	{
		minute=minute+60;
		hour--;
	}
	if(hour<0)
	{
		hour=hour+24;
		day--;
	}
	if(day<0)
	{
		if(cal1->month==2)
		{
			if(mRTC_IsLeap(DEF_RTC_BASE_YEAR+cal1->year))
			{
				tempMonthDay=29;
			}
			else
			{
				tempMonthDay=28;
			}
		}
		else
		{
			tempMonthDay=monthDay[cal1->month-1];
		}
		day+=tempMonthDay;
		month--;
	}
	if(month<0)
	{
		month+=12;
		year--;
	}
	if(year<0)
	{
		return 0;
	}
	tempYear=cal1->year;
	for(count=0,tempMonth=cal1->month+1;count<month;count++)
	{
		if(tempMonth==2)
		{
			if(mRTC_IsLeap(DEF_RTC_BASE_YEAR+tempYear))
			{
				tempMonthDay=29;
			}
			else
			{
				tempMonthDay=28;
			}
		}
		else
		{
			tempMonthDay=monthDay[tempMonth-1];
		}
		day+=tempMonthDay;
		tempMonth++;
		if(tempMonth>12)
		{
			tempMonth=1;
			tempYear++;
		}
	}
	tempYear+=1;
	while(year!=0)
	{
		if(mRTC_IsLeap(DEF_RTC_BASE_YEAR+tempYear))
		{
			day+=366;
		}
		else
		{
			day+=365;
		}
		tempYear++;
		year--;
	}
	totalSec=(uint32_t)day*DEF_RTC_EVERY_DAY_SECOND+(hour*DEF_RTC_EVERY_HOUR_SECOND)+(minute*DEF_RTC_EVERY_MINUTE_SECOND)+sec;
	return totalSec;
}

void vRTC_CalculateAlarmCnt( void )
{
	uint32_t u32Cnt = 0 ;

	vRTC_UpdateCalendar( DEF_RTC_CMD_UPDATE ) ;
	u32Cnt = u32RTC_CalculateCalendarDiff( &stcRTCAlarm , &stcRTCCalendar ) ;
	if ( u32Cnt <= DEF_RTC_ALARM_MAXIUM_DAY_SENCOND )
	{
		RTC->COMP1 = u32Cnt ;
		RTC->IFC   = RTC_IFC_COMP1 ;
		RTC->IEN   = ( RTC->IEN | RTC_IEN_COMP1 ) ;
	}
	else
	{
		RTC->IEN  &= ( ~_RTC_IEN_COMP1_MASK ) ;
	}
}

