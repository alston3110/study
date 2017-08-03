#ifndef DEF_IS_MSC_H
	#define DEF_IS_MSC_H
	
	#ifdef DEF_IS_MSC_C
		#define EXTERN 
	#else
		#define EXTERN extern
	#endif

	EXTERN void vMSC_Init( void ) ;
	EXTERN void vMSC_Release( void ) ;
	EXTERN __ramfunc bool MSCWrite(uint8_t*,uint32_t,uint32_t*);
	EXTERN __ramfunc bool MSCErase(uint32_t);

	#undef EXTERN 
#endif
