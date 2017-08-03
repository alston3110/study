#ifndef DEF_IS_ADC_H
	#define DEF_IS_ADC_H
	
	#ifdef DEF_IS_ADC_C
		#define EXTERN 
	#else
		#define EXTERN extern
	#endif

	EXTERN void vADC_Init( void ) ;
	EXTERN bool bADC_PMICVoltageIsOK( void ) ;

	#undef EXTERN
#endif
