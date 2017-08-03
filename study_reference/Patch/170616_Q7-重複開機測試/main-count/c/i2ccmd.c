#define DEF_IS_I2CCMD_C
#include "common.h"

/* ---------------------------------------------------------------------
 * Extern
--------------------------------------------------------------------- */
extern CALENDAR stcRTCCalendar ;
extern CALENDAR stcRTCAlarm    ;

extern uint8_t u8I2CRecvData[32] ;
extern volatile uint8_t u8I2CRecvState ;

extern uint8_t u8I2CSentData[32] ;
extern volatile uint8_t  u8I2CSentCmdLen ;

/* ---------------------------------------------------------------------
 * Value
--------------------------------------------------------------------- */
uint32_t u32I2CPlainData[4] = { 0 } ;

volatile uint32_t u32I2CFlashAddress  = 0x2000 ;
volatile uint32_t u32I2CFlashCheckSum = 0 ;
volatile uint32_t u32I2CFlashOriginCheckSum = 0 ;
volatile uint32_t u32I2CFlashSize     = 0 ;
volatile uint8_t  u8I2CFlashState     = 0 ;
volatile uint32_t u32I2CFlashTimeOut  = 0 ;

/* ---------------------------------------------------------------------
 * Const
--------------------------------------------------------------------- */
uint8_t const u8I2CCmdMaxLen[] = {
	3 +  6 , // DEF_I2CCMD_MASTER_SET_CLOCK
	3 +  6 , // DEF_I2CCMD_MASTER_SET_ALARM
	3      , // DEF_I2CCMD_MASTER_GET_CLOCK
	3      , // DEF_I2CCMD_MASTER_GET_ALARM
	3      , // DEF_I2CCMD_MASTER_RESET
	3 + 16 , // DEF_I2CCMD_MASTER_SENT_AESDATA
	3      , // DEF_I2CCMD_MASTER_GET_AESDATA
	3      , // DEF_I2CCMD_MASTER_GET_VERSION
	3 + 1  , // DEF_I2CCMD_MASTER_SET_POWER_CTL
	3      , // DEF_I2CCMD_MASTER_GET_POWER_CTL
	3      , // DEF_I2CCMD_MASTER_UPDATE_START
	3 + 16 , // DEF_I2CCMD_MASTER_UPDATE_DATA
	3 + 12 , // DEF_I2CCMD_MASTER_UPDATE_FINISH
	3      , // DEF_I2CCMD_MASTER_GET_WHORUN
	3 + 2  , // DEF_I2CCMD_MASTER_SET_WDOG
	3 + 1  , // DEF_I2CCMD_MASTER_SET_WDOG_RETRY
	3 	   , // DEF_I2CCMD_MASTER_GET_WDOG_STATUS
	3      , // DEF_I2CCMD_MASTER_GET_COUNT
} ;

/* ---------------------------------------------------------------------
 * Function
--------------------------------------------------------------------- */
int iI2CCmd_SentAck( void )
{
	u8I2CSentData[DEF_I2CCMD_LEN]  = 3 ;
	u8I2CSentData[DEF_I2CCMD_CMD]  = DEF_I2CCMD_SLAVE_ACK ;
	u8I2CSentData[DEF_I2CCMD_DATA] = u8I2CSentData[DEF_I2CCMD_LEN] + u8I2CSentData[DEF_I2CCMD_CMD] ;
	
	return ( 3 ) ;
}

int iI2CCmd_SentNAck( void )
{
	u8I2CSentData[DEF_I2CCMD_LEN]  = 3 ;
	u8I2CSentData[DEF_I2CCMD_CMD]  = DEF_I2CCMD_SLAVE_NACK ;
	u8I2CSentData[DEF_I2CCMD_DATA] = u8I2CSentData[DEF_I2CCMD_LEN] + u8I2CSentData[DEF_I2CCMD_CMD] ;
	
	return ( 3 ) ;
}

int iI2CCmd_SetData( uint8_t u8Len )
{
	int iLoop , iMaxLoop ;
	uint8_t u8CheckSum ;
	
	iMaxLoop = 2 + u8Len ;
	
	u8I2CSentData[DEF_I2CCMD_LEN] = 3 + u8Len ;
	u8I2CSentData[DEF_I2CCMD_CMD] = DEF_I2CCMD_SLAVE_SENT_DATA ;
	
	u8CheckSum = u8I2CSentData[DEF_I2CCMD_LEN] + u8I2CSentData[DEF_I2CCMD_CMD] ;
	
	for ( iLoop = DEF_I2CCMD_DATA ; iLoop < DEF_I2CCMD_DATA + u8Len ; iLoop ++ )
	{
		u8CheckSum += u8I2CSentData[iLoop] ;
	}
	
	u8I2CSentData[iMaxLoop] = u8CheckSum ;

	return ( 3 + u8Len ) ;
}

int iI2CCmd_CheckCmdIsOK( void )
{
	if ( u8I2CRecvData[DEF_I2CCMD_LEN] == u8I2CCmdMaxLen[(u8I2CRecvData[DEF_I2CCMD_CMD]&0x0F)] )
	{
		return ( 1 ) ;
	}
	return ( 0 ) ;
}

int iI2CCmd_SentCount( void )
{
	uint8_t u8McuCount[4] = { 0 } ;
	uint8_t u8Loop ;
	int     iPutIndex  ;
	
	u32Count = u32Count + 1;
	u8McuCount[3] = u32Count >> 24;
	u8McuCount[2] = (u32Count >> 16) & 0xff;
	u8McuCount[1] = (u32Count >> 8) & 0xff;
	u8McuCount[0] = u32Count & 0xff;

	iPutIndex = DEF_I2CCMD_DATA ;
	
	for( u8Loop = 0 ; u8Loop < 4 ; u8Loop++ )
	{
		u8I2CSentData[iPutIndex++] = u8McuCount[u8Loop] ;
	}
	
	return ( iI2CCmd_SetData( 4 ) ) ;
}

int iI2CCmd_GetCalendar( CALENDAR* pCal )
{
	uint8_t *pu8Adr    = (uint8_t *)pCal ;
	int     iIndex     = 0 ;
	
	for ( iIndex = 0 ; iIndex < sizeof(CALENDAR) ; iIndex ++ )
	{
		pu8Adr[iIndex] = u8I2CRecvData[DEF_I2CCMD_DATA+iIndex] ;
	}
	return ( iI2CCmd_SentAck( ) ) ;
}

int iI2CCmd_SendCalendar( CALENDAR* pCal )
{
	uint8_t *pu8Adr    = (uint8_t *)pCal ;
	int     iIndex     ;
	int     iPutIndex  ;
	
	iPutIndex = DEF_I2CCMD_DATA ;
	for ( iIndex = 0 ; iIndex < sizeof(CALENDAR) ; iIndex ++ )
	{
		u8I2CSentData[iPutIndex++] = pu8Adr[iIndex] ;
	}
	
	if ( mACMP_GetCR2032Status( ) )
	{
		u8I2CSentData[iPutIndex++] = 1 ;
	}
	else
	{
		u8I2CSentData[iPutIndex++] = 0 ;
	}
	
	return ( iI2CCmd_SetData( sizeof(CALENDAR) + 1 ) ) ;
}

int iI2CCmd_GetAESData( void )
{
	uint32_t u32InData[4] ;
	uint8_t *pInData = (uint8_t *)&u32InData[0] ;
	int iLoop ;
					
	for ( iLoop = 0 ; iLoop < 4*4 ; iLoop ++ )
	{
		pInData[iLoop] = u8I2CRecvData[DEF_I2CCMD_DATA+iLoop] ;
	}
					
	vAES_128EnDeCrypt( 0 , (uint32_t*)(u8AESKeyData) , (uint32_t*)u32InData , u32I2CPlainData ) ;

	return ( iI2CCmd_SentAck( ) ) ;
}

int iI2CCmd_SendAESData( void )
{
	uint8_t *pu8Adr ;
	int     iIndex     = 0 ;
	int     iPutIndex  ;
	
	pu8Adr = (uint8_t *)u32I2CPlainData ;
	iPutIndex = DEF_I2CCMD_DATA ;
	for( iIndex = 0 ; iIndex < RECEIVE_AES_LENGTH ; iIndex++ )
	{
		u8I2CSentData[iPutIndex++] = pu8Adr[iIndex] ;
	}
	
	return ( iI2CCmd_SetData( RECEIVE_AES_LENGTH ) ) ;
}

int iI2CCmd_SentVersion( void )
{
	uint8_t u8Version[4] = SOFTWARE_VERSION ;
	uint8_t u8Loop ;
	int     iPutIndex  ;
	
	iPutIndex = DEF_I2CCMD_DATA ;
	
	for( u8Loop = 0 ; u8Loop < 4 ; u8Loop++ )
	{
		u8I2CSentData[iPutIndex++] = u8Version[u8Loop] ;
	}
	
	return ( iI2CCmd_SetData( 4 ) ) ;
}

int iI2CCmd_SentPowerState( void )
{
	if( bPowerFunction )
	{
		u8I2CSentData[DEF_I2CCMD_DATA] = 1 ;
	}
	else
	{
		u8I2CSentData[DEF_I2CCMD_DATA] = 0 ;
	}

	return ( iI2CCmd_SetData( 1 ) ) ;
}

int iI2CCmd_GetUpdateStart( void )
{
	if ( (uint32_t)&iI2CCmd_GetUpdateStart >= 0x2000 )
	{
		u32I2CFlashAddress = 0x400   ;
		u8I2CSentData[DEF_I2CCMD_DATA] = 0 ;
	}
	else
	{
		u32I2CFlashAddress = 0x2000 ;
		u8I2CSentData[DEF_I2CCMD_DATA] = 1 ;
	}
	
	u8I2CFlashState           = 1 ;
	u32I2CFlashCheckSum       = 0 ;
	u32I2CFlashSize           = 0 ;
	u32I2CFlashOriginCheckSum = 0 ;

	vMSC_Init( ) ;
	
	return ( iI2CCmd_SetData( 1 ) ) ;
}

int iI2CCmd_GetUpdateData( void )
{
	if ( u8I2CFlashState )
	{
		uint32_t u32InData[4] ;
		uint8_t DeBinData[16] ;
		uint8_t *pInData = (uint8_t *)&u32InData[0] ;
		int iLoop ;
						
		u8I2CFlashState = 2 ;
						
		if( ( u32I2CFlashAddress & 0x1FF ) == 0 )
		{
			MSCErase( u32I2CFlashAddress ) ;
		}
						
		for ( iLoop = 0 ; iLoop < 4*4 ; iLoop ++ )
		{
			pInData[iLoop] = u8I2CRecvData[DEF_I2CCMD_DATA+iLoop] ;
			u32I2CFlashCheckSum += u8I2CRecvData[DEF_I2CCMD_DATA+iLoop] ;
		}
						
		vAES_128EnDeCrypt( 0 , (uint32_t*)(u8AESKeyData) , (uint32_t*)pInData , (uint32_t*)&DeBinData[0] ) ;
		MSCWrite( DeBinData , 16 , &u32I2CFlashAddress ) ;
		for ( iLoop = 0 ; iLoop < 4*4 ; iLoop ++ )
		{
			u32I2CFlashOriginCheckSum += DeBinData[iLoop] ;
		}
		u8I2CFlashState = 3 ;
		u8I2CSentData[DEF_I2CCMD_DATA] = 1 ;
		u32I2CFlashSize += 16 ;
	}
	else
	{
		u8I2CSentData[DEF_I2CCMD_DATA] = 0 ;
	}
	
	u32I2CFlashTimeOut = 0 ;
	
	return ( iI2CCmd_SetData( 1 ) ) ;
}

int iI2CCmd_GetUpdateFinish( void )
{
	int iOK = 0 ;
	
	if ( u8I2CFlashState == 3 )
	{
		uint32_t u32GetCheckSum ;
		uint32_t u32GetOriginCheckSum ;
		uint32_t u32GetFileSize ;
		uint8_t *pu8Adr ;
		int iLoop ;
		uint32_t u32WriteAdr = 0x200 ;
		uint8_t u8WriteData[16] ;
		uint32_t *pAdr = (uint32_t *)&u8WriteData[0] ;
						
		pu8Adr = (uint8_t *)&u32GetCheckSum ;
		for ( iLoop = 0 ; iLoop < 4 ; iLoop ++ )
		{
			pu8Adr[iLoop] = u8I2CRecvData[DEF_I2CCMD_DATA+iLoop] ;
		}
		
		pu8Adr = (uint8_t *)&u32GetOriginCheckSum ;
		for ( iLoop = 4 ; iLoop < 8 ; iLoop ++ )
		{
			pu8Adr[iLoop-4] = u8I2CRecvData[DEF_I2CCMD_DATA+iLoop] ;
		}
		
		pu8Adr = (uint8_t *)&u32GetFileSize ;
		for ( iLoop = 8 ; iLoop < 12 ; iLoop ++ )
		{
			pu8Adr[iLoop-8] = u8I2CRecvData[DEF_I2CCMD_DATA+iLoop] ;
		}
		
		if ( u32I2CFlashSize == u32GetFileSize )
		{
			if ( u32GetCheckSum == u32I2CFlashCheckSum )
			{
				if ( u32GetOriginCheckSum == u32I2CFlashOriginCheckSum )
				{
					if ( (uint32_t)&iI2CCmd_GetUpdateFinish >= 0x2000 )
					{
						*pAdr = 0x400   ;
					}
					else
					{
						*pAdr = 0x2000 ;
					}

					MSCErase( u32WriteAdr ) ;
					MSCWrite( u8WriteData , 4 , &u32WriteAdr ) ;
			
							
					//vMSC_Release( ) ;
					//SCB->AIRCR = 0x05FA0004;
					iOK = 1 ;
				}
			}
		}
	}
	
	u8I2CFlashState    = 0 ;
	u32I2CFlashTimeOut = 0 ;
	if ( iOK )
	{
		return ( iI2CCmd_SentAck( ) ) ;
	}
	return ( iI2CCmd_SentNAck( ) ) ;
}

int iI2CCmd_GetWhoRun( void )
{
	if ( (uint32_t)&iI2CCmd_GetWhoRun >= 0x2000 )
	{
		u8I2CSentData[DEF_I2CCMD_DATA] = 1 ;
	}
	else
	{
		u8I2CSentData[DEF_I2CCMD_DATA] = 0 ;
	}
	
	return ( iI2CCmd_SetData( 1 ) ) ;
}

#ifdef DEF_COMMON_WDOG_FUNCTION
int iI2CCmd_GetWdog_Status( void )
{
	uint8_t *pu8Adr ;
	int     iIndex     = 0 ;

	pu8Adr = (uint8_t *)&u32PowerWdogSetSec ;
	for( iIndex = 0 ; iIndex < 2 ; iIndex++ )
	{
		u8I2CSentData[DEF_I2CCMD_DATA+iIndex] = pu8Adr[1 - iIndex] ;
	}

	u8I2CSentData[DEF_I2CCMD_DATA+iIndex] = (uint8_t) u32PowerWdogSetRetryCount;
	iIndex++;
	u8I2CSentData[DEF_I2CCMD_DATA+iIndex] = (uint8_t) u32PowerWdogRetryCount;
	iIndex++;
	return ( iI2CCmd_SetData( iIndex ) ) ;
}

#endif

void vI2CCmd_Process( void )
{
	vI2C_TimeOut( ) ;
	
	if ( u8I2CRecvState & 0x04 )
	{
		I2C0->IEN &= ~DEF_I2C_INT_ENABLE ;
		u8I2CSentCmdLen = 0 ;
		if ( iI2CCmd_CheckCmdIsOK( ) )
		{
			switch( u8I2CRecvData[DEF_I2CCMD_CMD] )
			{
				case DEF_I2CCMD_MASTER_SET_CLOCK     	:
					u8I2CSentCmdLen = iI2CCmd_GetCalendar( &stcRTCCalendar ) ;
					vRTC_UpdateCalendar( DEF_RTC_CMD_CLEAR ) ;
					break ;
				case DEF_I2CCMD_MASTER_SET_ALARM     	:
					u8I2CSentCmdLen = iI2CCmd_GetCalendar( &stcRTCAlarm ) ;
					vRTC_CalculateAlarmCnt( ) ;
					break ;
				case DEF_I2CCMD_MASTER_GET_CLOCK     	:
					vRTC_UpdateCalendar( DEF_RTC_CMD_UPDATE ) ;
					u8I2CSentCmdLen = iI2CCmd_SendCalendar( &stcRTCCalendar ) ;
					break ;
				case DEF_I2CCMD_MASTER_GET_ALARM     	:
					u8I2CSentCmdLen = iI2CCmd_SendCalendar( &stcRTCAlarm ) ;
					break ;
				case DEF_I2CCMD_MASTER_RESET     		:
					SCB->AIRCR = 0x05FA0004 ;
					break ;
				case DEF_I2CCMD_MASTER_SENT_AESDATA 	:
					u8I2CSentCmdLen = iI2CCmd_GetAESData( ) ;
					break ;
				case DEF_I2CCMD_MASTER_GET_AESDATA  	:
					u8I2CSentCmdLen = iI2CCmd_SendAESData( ) ;
					break ;
				case DEF_I2CCMD_MASTER_GET_VERSION   	:
					u8I2CSentCmdLen = iI2CCmd_SentVersion( ) ;
					break ;
				case DEF_I2CCMD_MASTER_SET_POWER_CTL 	:
					vPower_SetAction( u8I2CRecvData[DEF_I2CCMD_DATA] ) ;
					u8I2CSentCmdLen = iI2CCmd_SentAck( ) ;
					break ;
				case DEF_I2CCMD_MASTER_GET_POWER_CTL 	:
					u8I2CSentCmdLen = iI2CCmd_SentPowerState( ) ;
					break ;
				case DEF_I2CCMD_MASTER_UPDATE_START	 	:
					u8I2CSentCmdLen = iI2CCmd_GetUpdateStart( ) ;
					break ;
				case DEF_I2CCMD_MASTER_UPDATE_DATA		:
					u8I2CSentCmdLen = iI2CCmd_GetUpdateData( ) ;
					break ;
				case DEF_I2CCMD_MASTER_UPDATE_FINISH	:
					u8I2CSentCmdLen = iI2CCmd_GetUpdateFinish( ) ;
					break ;
				case DEF_I2CCMD_MASTER_GET_WHORUN       :
					u8I2CSentCmdLen = iI2CCmd_GetWhoRun( ) ;
					break ;	
            #ifdef DEF_COMMON_WDOG_FUNCTION
				case DEF_I2CCMD_MASTER_SET_WDOG          :
					vPower_SetWdog_Time(u8I2CRecvData[DEF_I2CCMD_DATA], u8I2CRecvData[DEF_I2CCMD_DATA + 1]) ;
					u8I2CSentCmdLen = iI2CCmd_SentAck( ) ;
					break ;
				case DEF_I2CCMD_MASTER_SET_WDOG_RETRY    :
						vPower_SetWdog_Times(u8I2CRecvData[DEF_I2CCMD_DATA]) ;
						u8I2CSentCmdLen = iI2CCmd_SentAck( ) ;
					break ;
				case DEF_I2CCMD_MASTER_GET_WDOG_STATUS    :
					u8I2CSentCmdLen = iI2CCmd_GetWdog_Status( ) ;
					break ;
            #endif
				case DEF_I2CCMD_MASTER_GET_COUNT   	:
					u8I2CSentCmdLen = iI2CCmd_SentCount( ) ;
					break ;	
				default:
					u8I2CSentCmdLen = iI2CCmd_SentNAck( ) ;
					break ;			
			}
		}
		else
		{
			u8I2CSentCmdLen = iI2CCmd_SentNAck( ) ;
		}
		vI2C_RecvClear( ) ;
		I2C0->IEN |= DEF_I2C_INT_ENABLE ;
	}
	if ( u8I2CSentCmdLen )
	{
		vI2C_StartSend( ) ;
	}
}
