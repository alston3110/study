#ifndef DEF_IS_I2CCMD_H
	#define DEF_IS_I2CCMD_H
	
	#ifdef DEF_IS_I2CCMD_C
		#define EXTERN 
	#else
		#define EXTERN extern
	#endif

	enum __I2CCMD__ {
		DEF_I2CCMD_LEN      ,
		DEF_I2CCMD_CMD      ,
		DEF_I2CCMD_DATA
	} ;

	#define DEF_I2CCMD_MASTER_SET_CLOCK     	0x80
	#define DEF_I2CCMD_MASTER_SET_ALARM     	0x81
	#define DEF_I2CCMD_MASTER_GET_CLOCK     	0x82
	#define DEF_I2CCMD_MASTER_GET_ALARM     	0x83
	
	#define DEF_I2CCMD_MASTER_RESET     		0x84
	
	#define DEF_I2CCMD_MASTER_SENT_AESDATA 		0x85
	#define DEF_I2CCMD_MASTER_GET_AESDATA  		0x86
	
	#define DEF_I2CCMD_MASTER_GET_VERSION   	0x87
	
	#define DEF_I2CCMD_MASTER_SET_POWER_CTL 	0x88
	#define DEF_I2CCMD_MASTER_GET_POWER_CTL 	0x89
	
	#define DEF_I2CCMD_MASTER_UPDATE_START	 	0x8A
	#define DEF_I2CCMD_MASTER_UPDATE_DATA	 	0x8B
	#define DEF_I2CCMD_MASTER_UPDATE_FINISH	 	0x8C
	#define DEF_I2CCMD_MASTER_GET_WHORUN	 	0x8D

	#define DEF_I2CCMD_MASTER_SET_WDOG		0x8E
	#define DEF_I2CCMD_MASTER_SET_WDOG_RETRY	0x8F
	#define DEF_I2CCMD_MASTER_GET_WDOG_STATUS	0x90
	#define DEF_I2CCMD_MASTER_GET_COUNT   		0x92
	
	#define DEF_I2CCMD_SLAVE_ACK				0xA0
	#define DEF_I2CCMD_SLAVE_NACK				0xA1
	#define DEF_I2CCMD_SLAVE_SENT_DATA			0xA2
	

	EXTERN void vI2CCmd_Process( void ) ;

	#undef EXTERN
#endif
