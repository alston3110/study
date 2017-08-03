#define DEF_IS_GPIO_C
#include "common.h"

void vGPIO_SetMode( uint32_t u32PortBit , uint32_t u32Mode )
{
	volatile uint32_t *pu32Adr ;
	uint32_t u32Port  ;
	uint32_t u32Bit   ;
	uint32_t u32Mask  ;
	uint32_t u32Shift ;
	
	u32Port = ((u32PortBit & 0xF0) >> 4) ;
	u32Bit  = (u32PortBit & 0x0F) ;
	
	if ( u32Bit >= 8 )
	{
		pu32Adr = (uint32_t *)&GPIO->P[u32Port].MODEH ;
		u32Bit    = u32Bit - 8 ;
	}
	else
	{
		pu32Adr = (uint32_t *)&GPIO->P[u32Port].MODEL ;
	}
	u32Shift = u32Bit * 4 ;
	u32Mask = ~(0x0F << u32Shift) ;
	
	*pu32Adr = ( *pu32Adr & u32Mask ) | ( u32Mode << u32Shift ) ;
}

void vGPIO_SetInterrupt( uint32_t u32PortBit , uint32_t u32Mode )
{
	volatile uint32_t *pu32Adr ;
	uint32_t u32Port    ;
	uint32_t u32Bit     ;
	uint32_t u32TempBit ;
	uint32_t u32Mask    ;
	uint32_t u32Shift   ;
	
	u32Port = ((u32PortBit & 0xF0) >> 4) ;
	u32Bit  = (u32PortBit & 0x0F) ;
	
	if ( u32Bit >= 8 )
	{
		pu32Adr = (uint32_t *)&GPIO->EXTIPSELH ;
		u32TempBit = u32Bit - 8 ;
	}
	else
	{
		pu32Adr = (uint32_t *)&GPIO->EXTIPSELL ;
		u32TempBit = u32Bit ;
	}
	
	u32Shift = u32TempBit * 4 ;
	u32Mask = ~(0x07 << u32Shift) ;
	
	*pu32Adr = ( *pu32Adr & u32Mask ) | ( u32Port << u32Shift ) ;
	if ( u32Mode & 0x01 )
	{
		GPIO->EXTIFALL  |= ( 1 << u32Bit ) ;
	}
	if ( u32Mode & 0x02 )
	{
		GPIO->EXTIRISE  |= ( 1 << u32Bit ) ;
	}
	GPIO->IFC       |= ( 1 << u32Bit ) ;
	GPIO->IEN       |= ( 1 << u32Bit ) ;
}

uint32_t u32GPIO_Read( uint32_t u32PortBit )
{
	uint32_t u32Port  ;
	uint32_t u32Bit   ;
	
	u32Port = ((u32PortBit & 0xF0) >> 4) ;
	u32Bit  = (u32PortBit & 0x0F) ;
	
	return ( mCommon_RWMemAddr( mCommon_BitBand( (int)&GPIO->P[u32Port].DIN , u32Bit ) ) ) ;
}

void vGPIO_Write( uint32_t u32PortBit , uint32_t u32HiLo )
{
	uint32_t u32Port  ;
	uint32_t u32Bit   ;
	
	u32Port = ((u32PortBit & 0xF0) >> 4) ;
	u32Bit  = (u32PortBit & 0x0F) ;
	
	mCommon_RWMemAddr( mCommon_BitBand( (int)&GPIO->P[u32Port].DOUT , u32Bit ) ) = u32HiLo ;
}

void vGPIO_InterruptDisOrEn( uint32_t u32PortBit , uint32_t u32Mode )
{
	uint32_t u32Bit   ;

	u32Bit  = (u32PortBit & 0x0F) ;
	if ( u32Mode )
	{
		GPIO->IEN       |= ( 1 << u32Bit ) ;
	}
	else
	{
		GPIO->IEN       &= ( ~( 1 << u32Bit ) ) ;
	}
}

void vGPIO_Init( void )
{
	CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO ;
	GPIO->INSENSE     = GPIO_INSENSE_INT ;
  
	/* Power On/Off */
	vGPIO_SetMode( DEF_GPIO_POWER_ON , DEF_GPIO_MODE_INPUT ) ;
	if ( u32GPIO_Read( DEF_GPIO_POWER_ON ) )
	{
		vGPIO_Write( DEF_GPIO_POWER_ON , HIGH ) ;
	}
	else
	{
		vGPIO_Write( DEF_GPIO_POWER_ON , LOW ) ;
	}
	vGPIO_SetMode( DEF_GPIO_POWER_ON , DEF_GPIO_MODE_PUSHPULL ) ;
	vGPIO_SetMode( DEF_GPIO_POR , DEF_GPIO_MODE_WIREDAND ) ;
	/* Power Button */
	vGPIO_SetMode( DEF_GPIO_POWER_BUTTON , DEF_GPIO_MODE_INPUT ) ;
}

