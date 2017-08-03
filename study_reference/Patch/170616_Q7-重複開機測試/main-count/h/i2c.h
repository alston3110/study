#ifndef DEF_IS_I2C_H
	#define DEF_IS_I2C_H
	
	#ifdef DEF_IS_I2C_C
		#define EXTERN 
	#else
		#define EXTERN extern
	#endif

	#define DEF_I2C_ADDR  					(0xc<<1)
	#define DEF_I2C_INT_ENABLE 				(I2C_IEN_ADDR|I2C_IEN_RXDATAV|I2C_IEN_SSTOP|I2C_IEN_CLTO)


	#define I2C_ARRAY_LENGTH 				20
	#define RECEIVE_CLOCK_TIME_LENGTH 		6
	#define RECEIVE_ALARM_TIME_LENGTH 		6
	#define RECEIVE_AES_LENGTH 				16
	#define RECEIVE_REQPWROFF_LENGTH 		1
	#define SEND_SENDPWRSTATE_LENGTH 		1


	#define mI2C_Enable() 					{ I2C0->IEN = DEF_I2C_INT_ENABLE ; }
	#define mI2C_Disable()					{ I2C0->IEN = _I2C_IEN_RESETVALUE ; }

	EXTERN void vI2C_Init( void ) ;
	EXTERN void vI2C_StartSend( void ) ;
	EXTERN void vI2C_TimeOut( void ) ;

	#undef EXTERN
#endif
