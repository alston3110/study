#define DEF_IS_ADC_C
#include "common.h"

/* ---------------------------------------------------------------------
 * Define
--------------------------------------------------------------------- */
#define DEF_ADC_PMIC_VOLTAGE_MAX 		2900 //2.9V
#define DEF_ADC_PMIC_VOLTAGE_MIN 		2400 //2.4V

#define DEF_ADC_PMIC_RES1 				10 		//10K
#define DEF_ADC_PMIC_RES2 				10 		//10K
#define DEF_ADC_REFERENCE_VOLTAGE 		2500

/* ---------------------------------------------------------------------
 * Value / Data
--------------------------------------------------------------------- */
volatile uint32_t u32ADCPMICVoltage  = 0 ;
volatile bool     bADCVoltageGotFlag = NO ;

/* ---------------------------------------------------------------------
 * Function
--------------------------------------------------------------------- */
void ADC0_IRQHandler( void )
{
	uint32_t adData = ADC0->SINGLEDATA ;
	
	u32ADCPMICVoltage  = ( adData * DEF_ADC_REFERENCE_VOLTAGE / 4096 ) * ( ( DEF_ADC_PMIC_RES1 + DEF_ADC_PMIC_RES2 ) / DEF_ADC_PMIC_RES2 ) ;
	bADCVoltageGotFlag = YES ;
	ADC0->IFC          = ADC_IFC_SINGLE ;
}

void vADC_Init( void )
{
	CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_ADC0 ;
	ADC0->CTRL       |= ( ADC_CTRL_OVSRSEL_X4096 | ADC_CTRL_LPFMODE_RCFILT ) ;
	ADC0->SINGLECTRL |= ( ADC_SINGLECTRL_INPUTSEL_CH7 | ADC_SINGLECTRL_REF_2V5 | ADC_SINGLECTRL_AT_32CYCLES | ADC_SINGLECTRL_RES_DEFAULT ) ;
	ADC0->IEN         = ADC_IEN_SINGLE ;
	NVIC_EnableIRQ( ADC0_IRQn ) ;
}

bool bADC_PMICVoltageIsOK( void )
{
	bADCVoltageGotFlag = NO ;
	ADC0->CMD          = ADC_CMD_SINGLESTART ;
	
	while( bADCVoltageGotFlag != YES ) ;
	
	if( u32ADCPMICVoltage >= DEF_ADC_PMIC_VOLTAGE_MIN && u32ADCPMICVoltage <= DEF_ADC_PMIC_VOLTAGE_MAX )
	{
		return ( true ) ;
	}
	
	return ( false ) ;
}
