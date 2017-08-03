#ifndef DEF_IS_COMMON_H
	#define DEF_IS_COMMON_H
	
	/* ---------------------------------------------------------------------
	* Include MCU
	--------------------------------------------------------------------- */
	#include "efm32.h"
	//#include "config_main.h"
	#include <stdbool.h>
	
	#define __ramfunc __attribute__ ((long_call, section (".textrw")))
	/* ---------------------------------------------------------------------
	* Define
	--------------------------------------------------------------------- */
	#define SOFTWARE_VERSION 							{'t',17,3,27}

	#define HIGH 										1
	#define LOW  										0

	#define ON 											1
	#define OFF 										0

	#define YES 										1
	#define NO  										0
	
	#define DEF_COMMON_HFCLK_FREQUENCY                 	1200000
	#define DEF_COMMON_HFPERCLK_FREQUENCY              	1200000
	
	#define DEF_COMMON_POWER_SOURCE_DC      			1
	#define DEF_COMMON_POWER_SOURCE_CELL_BATTERY 		0

	#define DEF_COMMON_FOR_LOOP_TIME_1MS  				5
	#define DEF_POWER_AC_IN_DETECTION_TIME  DEF_COMMON_FOR_LOOP_TIME_1MS * 100
	
	#define DEF_COMMON_WDOG_FUNCTION
	#define DEF_COMMON_WDOG_DEFAULT 		            10
	#define DEF_COMMON_WDOG_ONOFF	 		            true
	#define DEF_COMMON_WDOG_RETRY_DEFAULT	 		    0
	/* ---------------------------------------------------------------------
	* Macro
	--------------------------------------------------------------------- */
	#define mCommon_BitBand( mAddr , mBit ) 			(0x42000000 + (mAddr - 0x40000000)* 32 + mBit * 4)
	#define mCommon_RWMemAddr( mAddr ) 					*((volatile unsigned long *) (mAddr))
	
	#define mCommon_DisableIrq() 						{ __asm( "CPSID I" ) ; }
	#define mCommon_EnableIrq()							{ __asm( "CPSIE I" ) ; }
	
	/* ---------------------------------------------------------------------
	* include
	--------------------------------------------------------------------- */	
	#include "cmu.h"
	#include "gpio.h"
	#include "i2c.h"
	#include "i2ccmd.h"
	#include "rtc.h"
	#include "aes.h"
	#include "acmp.h"
	//#include "em_chip.h"
	#include "adc.h"
	#include "msc.h"
	#include "power.h"

#endif
