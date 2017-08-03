#define DEF_IS_I2C_C
#include "common.h"

/* ---------------------------------------------------------------------
 * extern
--------------------------------------------------------------------- */
extern volatile uint8_t  u8I2CFlashState ;
extern volatile uint32_t u32I2CFlashTimeOut ;

/* ---------------------------------------------------------------------
 * Value
--------------------------------------------------------------------- */
volatile uint8_t i2cCltoCount = 0;

uint8_t u8I2CRecvData[32] = {0} ;
volatile uint8_t u8I2CRecvIndex     = 0 ;
volatile uint8_t u8I2CRecvCheckSum  = 0 ;
volatile uint8_t u8I2CRecvState     = 0 ;
volatile uint32_t u32I2CRecvTimeOut = 0 ;

uint8_t u8I2CSentData[32] = {0} ;
volatile uint8_t  u8I2CSentIndex     = 0 ;
volatile uint8_t  u8I2CSentCmdLen    = 0 ;
volatile uint8_t  u8I2CSentState     = 0 ;
volatile uint32_t u32I2CSentTimeOut  = 0 ;


uint32_t u32I2CErrorTimes[12] = { 0 } ;
/* ---------------------------------------------------------------------
 * Function
--------------------------------------------------------------------- */
void vI2C_RecvClear( void )
{
	u8I2CRecvIndex    = 0 ;
	u8I2CRecvCheckSum = 0 ;
	u8I2CRecvState    = 0 ;
	u32I2CRecvTimeOut = 0 ;
}

void vI2C_SentClear( void )
{
	u32I2CSentTimeOut = 0 ;
	u8I2CSentIndex    = 0 ;
	u8I2CSentCmdLen   = 0 ;
	u8I2CSentState    = 0 ;
}

void I2C0_IRQHandler( void )
{
	uint32_t u32I2CState = I2C0->IF ;
	int      iError      = 0 ;
	uint8_t  u8GetData ;
	
	if ( u32I2CState & I2C_IF_ADDR )
	{
		I2C0->IFC = I2C_IFC_ADDR ;
		u8GetData = I2C0->RXDATA ;
		if ( u8GetData & 0x01 )
		{
			if ( u8I2CSentState == 0x02 )
			{
				// Error
				u32I2CErrorTimes[4] ++ ;
				I2C0->IEN &= ~(I2C_IEN_TXC) ;
				I2C0->CMD = I2C_CMD_CLEARTX | I2C_CMD_ABORT ;
				vI2C_SentClear( ) ;
				I2C0->CMD = I2C_CMD_NACK ;
				return ;
			}
			else
			{
				u8I2CSentState = 0x01 ;
			}
		}
		else
		{
			if ( u8I2CSentState )
			{
				// error
				u32I2CErrorTimes[8] ++ ;
				I2C0->IEN &= ~(I2C_IEN_TXC) ;
				vI2C_SentClear( ) ;
			}
			
			if ( u8I2CRecvState & 0x04 )
			{
				u8I2CRecvState |= 0x80 ;
				u32I2CErrorTimes[0] ++ ;
			}
			else
			{
				vI2C_RecvClear( ) ;
				u8I2CRecvState    = 0x01 ;
			}
		}
		I2C0->CMD = I2C_CMD_ACK  ;
	}
	else if ( u32I2CState & I2C_IF_RXDATAV )
	{	
		u8GetData = I2C0->RXDATA ;
		if ( u8I2CRecvState & 0x80 )
		{
			I2C0->CMD = I2C_CMD_NACK ;
		}
		else
		{
			u32I2CRecvTimeOut = 0 ;
			u8I2CRecvData[u8I2CRecvIndex] = u8GetData ;
			u8I2CRecvState |= 0x02 ;
			if ( u8I2CRecvIndex <= 32 )
			{
				if ( ( u8I2CRecvIndex + 1 ) >= u8I2CRecvData[DEF_I2CCMD_LEN] )
				{
					if ( u8I2CRecvCheckSum != u8I2CRecvData[u8I2CRecvIndex] )
					{
						iError = 1 ;
						u32I2CErrorTimes[1] ++ ;
					}
					else
					{
						u8I2CRecvState |= 0x04 ;
					}
				}
				else
				{
					u8I2CRecvCheckSum += u8I2CRecvData[u8I2CRecvIndex] ;
				}
				
				u8I2CRecvIndex ++ ;
			}
			else
			{
				iError = 1 ;
				u32I2CErrorTimes[2] ++ ;
			}
			
			if ( iError )
			{
				I2C0->CMD = I2C_CMD_NACK ;
				vI2C_RecvClear( ) ;
			}
			else
			{
				I2C0->CMD = I2C_CMD_ACK ;
			}
		}
	}

	if( u32I2CState & I2C_IF_TXC )
	{
		I2C0->IFC = I2C_IF_TXC ;
		u32I2CSentTimeOut = 0 ;
		if ( u8I2CSentCmdLen && ( u8I2CSentCmdLen <= 32 ) )
		{
			if ( u8I2CSentIndex < u8I2CSentCmdLen )
			{
				I2C0->TXDATA = u8I2CSentData[u8I2CSentIndex] ;
				u8I2CSentIndex ++ ;
			}
			else
			{
				u8I2CSentIndex    = 0 ;
				u8I2CSentCmdLen   = 0 ;
				u8I2CSentState    = 0 ;
				I2C0->CMD = I2C_CMD_CLEARTX | I2C_CMD_ABORT ;
				I2C0->IEN &= (~I2C_IEN_TXC) ;
			}
		}
	}
	
	if ( u32I2CState & I2C_IF_SSTOP )
	{
		I2C0->IFC = I2C_IFC_SSTOP ;
	}
	
	if ( u32I2CState & I2C_IF_CLTO )
	{
		I2C0->IFC = I2C_IFC_CLTO ;
		i2cCltoCount += 1 ;
		if( i2cCltoCount > 10 )
		{
			I2C0->CTRL &= (~I2C_CTRL_EN) ;
			
			I2C0->CMD = I2C_CMD_CLEARTX | I2C_CMD_ABORT ;
			I2C0->IEN &= (~I2C_IEN_TXC) ;
			
			u8I2CSentIndex  = 0 ;
			u8I2CSentCmdLen = 0 ;
			vI2C_RecvClear( ) ;
			vI2C_SentClear( ) ;
			I2C0->CTRL |= I2C_CTRL_EN ;
			i2cCltoCount = 0;
			u32I2CErrorTimes[3] ++ ;
		}
	}
	else
	{
		i2cCltoCount = 0 ;
	}
}

void vI2C_Init( void )
{
	CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_I2C0 ;
	vGPIO_SetMode( DEF_GPIO_I2C_SDA , DEF_GPIO_MODE_WIREDANDFILTER ) ;
	vGPIO_SetMode( DEF_GPIO_I2C_SCL , DEF_GPIO_MODE_WIREDANDFILTER ) ;
  
	I2C0->CTRL      = I2C_CTRL_EN |I2C_CTRL_SLAVE | I2C_CTRL_CLTO_1024PPC ;
	I2C0->SADDR     = DEF_I2C_ADDR ;
	I2C0->SADDRMASK = 0xFE ;
	I2C0->ROUTE     = ( I2C_ROUTE_LOCATION_LOC6 | I2C_ROUTE_SDAPEN | I2C_ROUTE_SCLPEN ) ;
  
	I2C0->IFC       = _I2C_IFC_MASK ;
	I2C0->IEN       = DEF_I2C_INT_ENABLE ;
	NVIC_EnableIRQ( I2C0_IRQn ) ;
}

void vI2C_StartSend( void )
{
	if ( u8I2CSentState == 0x01 )
	{
		I2C0->IEN     |= I2C_IEN_TXC ;
		I2C0->TXDATA   = u8I2CSentData[0] ;
		u8I2CSentIndex = 1 ;
		u8I2CSentState = 0x02 ;
	}
}

void vI2C_TimeOut( void )
{
	if ( u8I2CRecvState )
	{
		u32I2CRecvTimeOut ++ ;
		
		if ( u32I2CRecvTimeOut >= 0xFF )
		{
			u32I2CErrorTimes[5] ++ ;
			I2C0->IEN &= ~DEF_I2C_INT_ENABLE ;
			vI2C_RecvClear( ) ;
			I2C0->IEN |= DEF_I2C_INT_ENABLE ;
			I2C0->CMD = I2C_CMD_CLEARTX | I2C_CMD_ABORT ;
		}
	}
	
	if ( u8I2CSentIndex )
	{
		u32I2CSentTimeOut ++ ;
		if ( u32I2CSentTimeOut >= 0xFF )
		{
			u32I2CErrorTimes[6] ++ ;
			I2C0->IEN &= ~(DEF_I2C_INT_ENABLE|I2C_IEN_TXC) ;
			vI2C_SentClear( ) ;
			I2C0->IEN |= DEF_I2C_INT_ENABLE ;
			I2C0->CMD = I2C_CMD_CLEARTX | I2C_CMD_ABORT ;
		}
	}
	
	if ( u8I2CFlashState )
	{
		u32I2CFlashTimeOut ++ ;
		if ( u32I2CFlashTimeOut >= 0xFF )
		{
			u32I2CErrorTimes[7] ++ ;
			u8I2CFlashState = 0 ;
			u32I2CFlashTimeOut = 0 ;
			
			I2C0->IEN &= ~(DEF_I2C_INT_ENABLE|I2C_IEN_TXC) ;
			vI2C_RecvClear( ) ;
			vI2C_SentClear( ) ;
			I2C0->IEN |= DEF_I2C_INT_ENABLE ;
			I2C0->CMD = I2C_CMD_CLEARTX | I2C_CMD_ABORT ;
		}
	}
}
