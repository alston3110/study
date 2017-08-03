#ifndef DEF_IS_GPIO_H
	#define DEF_IS_GPIO_H
	
	#ifdef DEF_IS_GPIO_C
		#define EXTERN 
	#else
		#define EXTERN extern
	#endif

	#define DEF_GPIO_MODE_DISABLED                      0x00000000UL    /* Mode DISABLED */
	#define DEF_GPIO_MODE_INPUT                         0x00000001UL    /* Mode INPUT */
	#define DEF_GPIO_MODE_INPUTPULL                     0x00000002UL    /* Mode INPUTPULL */
	#define DEF_GPIO_MODE_INPUTPULLFILTER               0x00000003UL    /* Mode INPUTPULLFILTER */
	#define DEF_GPIO_MODE_PUSHPULL                      0x00000004UL    /* Mode PUSHPULL */
	#define DEF_GPIO_MODE_PUSHPULLDRIVE                 0x00000005UL    /* Mode PUSHPULLDRIVE */
	#define DEF_GPIO_MODE_WIREDOR                       0x00000006UL    /* Mode WIREDOR */
	#define DEF_GPIO_MODE_WIREDORPULLDOWN               0x00000007UL    /* Mode WIREDORPULLDOWN */
	#define DEF_GPIO_MODE_WIREDAND                      0x00000008UL    /* Mode WIREDAND */
	#define DEF_GPIO_MODE_WIREDANDFILTER                0x00000009UL    /* Mode WIREDANDFILTER */
	#define DEF_GPIO_MODE_WIREDANDPULLUP                0x0000000AUL    /* Mode WIREDANDPULLUP */
	#define DEF_GPIO_MODE_WIREDANDPULLUPFILTER          0x0000000BUL    /* Mode WIREDANDPULLUPFILTER */
	#define DEF_GPIO_MODE_WIREDANDDRIVE                 0x0000000CUL    /* Mode WIREDANDDRIVE */
	#define DEF_GPIO_MODE_WIREDANDDRIVEFILTER           0x0000000DUL    /* Mode WIREDANDDRIVEFILTER */
	#define DEF_GPIO_MODE_WIREDANDDRIVEPULLUP           0x0000000EUL    /* Mode WIREDANDDRIVEPULLUP */
	#define DEF_GPIO_MODE_WIREDANDDRIVEPULLUPFILTER     0x0000000FUL    /* Mode WIREDANDDRIVEPULLUPFILTER */


	/*GPIO Setting*/
	#define DEF_GPIO_POWER_ON      						0x00  /* 0 ,  0 */
	#define DEF_GPIO_POWER_BUTTON  						0x21  /* 2 ,  1 */
	#define DEF_GPIO_PMIC_ON_OFF_REQ					0x1B  /* 1 , 11 */
	#define DEF_GPIO_I2C_SDA 							0x4C  /* 4 , 12 */
	#define DEF_GPIO_I2C_SCL 							0x4D  /* 4 , 13 */
	#define DEF_GPIO_POR 							0x52  /* 5 , 2 */
	//Unit:sec
//	#define POWER_BUTTON_POWER_OFF_DETECTION_TIME   1
//	#define POWER_BUTTON_POWER_ON_DETECTION_TIME    1
	
	#define mGPIO_GetPort( mPortBit ) 					((mPortBit>>4)&0x0F)
	#define mGPIO_GetPin( mPortBit ) 					(mPortBit&0x0F)


	EXTERN void vGPIO_SetMode( uint32_t u32PortBit , uint32_t u32Mode ) ;
	EXTERN void vGPIO_SetInterrupt( uint32_t u32PortBit , uint32_t u32Mode ) ;
	EXTERN uint32_t u32GPIO_Read( uint32_t u32PortBit ) ;
	EXTERN void vGPIO_Write( uint32_t u32PortBit , uint32_t u32HiLo ) ;
	EXTERN void vGPIO_InterruptDisOrEn( uint32_t u32PortBit , uint32_t u32Mode ) ;

	EXTERN void vGPIO_Init( void ) ;

	#undef EXTERN
#endif
