#ifndef DEF_IS_CMU_H
	#define DEF_IS_CMU_H
	
	#ifdef DEF_IS_CMU_C
		#define EXTERN 
	#else
		#define EXTERN extern
	#endif

	EXTERN volatile uint32_t u32CMUBackupCoreCLKEN ;
	EXTERN volatile uint32_t u32CMUBackupPerhCLkEN ;

	#define mCMU_BackupClkEN() { \
		u32CMUBackupCoreCLKEN = CMU->HFCORECLKEN0 ; \
		u32CMUBackupPerhCLkEN = CMU->HFPERCLKEN0  ; \
	}

	#define mCMU_SetClkENForEM2() { \
		CMU->HFCORECLKEN0 = (CMU->HFCORECLKEN0 & ~_CMU_HFCORECLKEN0_MASK)|CMU_HFCORECLKEN0_LE ; \
		CMU->HFPERCLKEN0  = ((CMU->HFPERCLKEN0 & ~_CMU_HFPERCLKEN0_MASK)|CMU_HFPERCLKEN0_GPIO|CMU_HFPERCLKEN0_ACMP0) ; \
	}

	#define mCMU_RestoreClkEN() { \
		CMU->HFCORECLKEN0 = u32CMUBackupCoreCLKEN ; \
		CMU->HFPERCLKEN0  = u32CMUBackupPerhCLkEN ; \
	}

	EXTERN void vCMU_Init( void ) ;
	
	#undef EXTERN
#endif
