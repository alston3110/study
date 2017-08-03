#ifndef DEF_IS_AES_H
	#define DEF_IS_AES_H
	
	#ifdef DEF_IS_AES_C
		#define EXTERN 
	#else
		#define EXTERN extern
		
		EXTERN uint8_t const u8AESKeyData[] ;
	#endif

	EXTERN void vAES_Init( void ) ;
	EXTERN void vAES_128EnDeCrypt( uint8_t encrypt, uint32_t *inkey, uint32_t *indata, uint32_t *outdata ) ;

	#undef EXTERN
#endif
