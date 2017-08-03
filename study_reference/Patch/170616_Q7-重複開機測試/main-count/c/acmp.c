#define DEF_IS_ACMP_C
#include "common.h"

/* ---------------------------------------------------------------------
 * Define
--------------------------------------------------------------------- */
#define DEF_ACMP_DC_IN_VOLTAGE           3.1 //3.1V
#define DEF_ACMP_CR2032_MINMUM_VOLTAGE   2.4 //2.4V

/* ---------------------------------------------------------------------
 * Macro
--------------------------------------------------------------------- */
#define mACMP_CalCR2032VDDLevel( mDCINVoltage , mCR2032MinVoltage ) ((uint8_t)(((float)mCR2032MinVoltage*63.0)/(float)mDCINVoltage))

/* ---------------------------------------------------------------------
 * Function
--------------------------------------------------------------------- */
/* ---------------------------------------------------------------------
 * ACMP Interrupt Handler for DC IN
--------------------------------------------------------------------- */
void ACMP0_IRQHandler( void )
{
	ACMP0->IFC = ACMP_IFC_EDGE ;
	if( ACMP0->STATUS & ACMP_STATUS_ACMPOUT )
	{
		bPowerSource = DEF_COMMON_POWER_SOURCE_DC ;
		vPower_SetPowerNeedOn( ) ;
	}
	else
	{
		bPowerSource = DEF_COMMON_POWER_SOURCE_CELL_BATTERY ;
	}
}

/* ---------------------------------------------------------------------
 * ACMP Initial
 * ACMP0 is DC IN
 * ACMP1 is Cell Battery
--------------------------------------------------------------------- */
void vACMP_Init( void )
{
	CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_ACMP1 | CMU_HFPERCLKEN0_ACMP0 ;
  
	ACMP0->INPUTSEL = ( ACMP_INPUTSEL_NEGSEL_2V5 | 
						ACMP_INPUTSEL_POSSEL_CH0 | 
						ACMP_INPUTSEL_LPREF
						) ;

	ACMP0->CTRL     = ( ACMP_CTRL_BIASPROG_DEFAULT   |
						ACMP_CTRL_HYSTSEL_HYST7      |
						ACMP_CTRL_HALFBIAS           |
						ACMP_CTRL_WARMTIME_256CYCLES |
						ACMP_CTRL_IFALL_ENABLED      |
						ACMP_CTRL_IRISE_ENABLED      |
						ACMP_CTRL_EN
						) ;
  
	ACMP0->IEN |= ACMP_IEN_EDGE ;

	while( !( ACMP0->STATUS & ACMP_STATUS_ACMPACT ) ) ;
	NVIC_EnableIRQ( ACMP0_IRQn ) ;
  

	ACMP1->INPUTSEL = ( ACMP_INPUTSEL_NEGSEL_VDD | 
						( mACMP_CalCR2032VDDLevel( DEF_ACMP_DC_IN_VOLTAGE , DEF_ACMP_CR2032_MINMUM_VOLTAGE ) << _ACMP_INPUTSEL_VDDLEVEL_SHIFT ) |
						ACMP_INPUTSEL_POSSEL_CH6 |
						ACMP_INPUTSEL_LPREF
						) ;

	ACMP1->CTRL     = (
						ACMP_CTRL_BIASPROG_DEFAULT   |
						ACMP_CTRL_HYSTSEL_HYST7      |
						ACMP_CTRL_HALFBIAS           |
						ACMP_CTRL_WARMTIME_256CYCLES |
						ACMP_CTRL_EN
						) ;

	while( !( ACMP1->STATUS & ACMP_STATUS_ACMPACT ) ) ;

	vGPIO_Write( DEF_GPIO_PMIC_ON_OFF_REQ , LOW ) ;
	vGPIO_SetMode( DEF_GPIO_PMIC_ON_OFF_REQ , DEF_GPIO_MODE_WIREDAND ) ;

	if( ACMP0->STATUS & ACMP_STATUS_ACMPOUT )
	{
		bPowerSource = DEF_COMMON_POWER_SOURCE_DC ;
		vPower_SetPowerNeedOn( ) ;
	}
	else
	{
		bPowerSource = DEF_COMMON_POWER_SOURCE_CELL_BATTERY ;
	}

}
