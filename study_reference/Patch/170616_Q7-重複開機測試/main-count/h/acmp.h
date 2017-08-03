#ifndef DEF_IS_ACMP_H
	#define DEF_IS_ACMP_H
	
	#ifdef DEF_IS_ACMP_C
		#define EXTERN 
	#else
		#define EXTERN extern
	#endif


	#define mACMP_GetDCINStatus()   (ACMP0->STATUS & ACMP_STATUS_ACMPOUT)
	#define mACMP_GetCR2032Status() (ACMP1->STATUS & ACMP_STATUS_ACMPOUT)
	#define mACMP_DisableCR2032() 	{ ACMP1->CTRL = (ACMP1->CTRL & _ACMP_CTRL_EN_MASK) ; }
	#define mACMP_EnableCR2032() 	{ ACMP1->CTRL = (ACMP1->CTRL & _ACMP_CTRL_EN_MASK)|ACMP_CTRL_EN ; }

	EXTERN void vACMP_Init( void ) ;
	
	#undef EXTERN
#endif
