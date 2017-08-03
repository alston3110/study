#define DEF_IS_MAIN_C
#include "common.h"

/* ---------------------------------------------------------------------
 * Function
--------------------------------------------------------------------- */
void vMain_Init( void )
{
	vCMU_Init( ) ;
	vGPIO_Init( ) ;
	vACMP_Init( ) ;
	vRTC_Init( ) ;
	vI2C_Init( ) ;
	vAES_Init( ) ;
	vADC_Init( ) ;
	vPower_DefaultInit( ) ;
	mCMU_BackupClkEN( ) ;
}

void vMain_SleepClose( void )
{
	mACMP_DisableCR2032( ) ;
	vPower_OffControl( ) ;
	vPower_DefaultInit( ) ;
	mCMU_SetClkENForEM2( ) ;
}

void vMain_WakeupDCIn( void )
{
	mACMP_EnableCR2032( ) ;
	mCMU_RestoreClkEN( ) ;
}

void vMain_EnterEM2( void )
{
	/* Enter Cortex-M3 deep sleep mode */
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk ;
	__WFI( ) ;
}

void main( void )
{
	vMain_Init( ) ;	
	
	while( 1 )
	{
		if( bPowerSource == DEF_COMMON_POWER_SOURCE_DC )
		{
			vI2CCmd_Process( ) ;
			vPower_Process( ) ;
		}
		else
		{
			vMain_SleepClose( ) ;
			vMain_EnterEM2( ) ;
			if ( bPowerSource )
			{
				vMain_WakeupDCIn( ) ;
			}
		}
	}
}
