#define DEF_IS_AES_C
#include "common.h"

//{128,85,120,35,18,152,59,99,72,123,23,65,134,32,19,83};
uint8_t const u8AESKeyData[] = {
	0xA8,0xB3,0xB9,0x80,0x1E,0x3F,0x0E,0x8E,0x90,0x51,0x2B,0x7B,0x83,0x02,0x0B,0xDF
} ;

void vAES_Init( void )
{
	CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_AES ;
}

uint32_t u32AES_ByteReverse( uint32_t data )
{
	return ( ((data&0x000000FF)<<24)|((data&0x0000FF00)<<8)|((data&0x00FF0000)>>8)|((data&0xFF000000)>>24) ) ;
}

void vAES_128EnDeCrypt( uint8_t encrypt , uint32_t *inkey , uint32_t *indata , uint32_t *outdata )
{
	signed char i;
	
	/* Load key into high key for key buffer usage */
	for (i = 3; i >= 0; i--)
	{
		AES->KEYHA = u32AES_ByteReverse(*(inkey+i));
	}
	
	if (encrypt)
	{
		/* Select encryption mode */
		AES->CTRL = AES_CTRL_KEYBUFEN | AES_CTRL_DATASTART;
	}
	else
	{
		/* Select decryption mode */
		AES->CTRL = AES_CTRL_DECRYPT | AES_CTRL_KEYBUFEN | AES_CTRL_DATASTART;
	}
	
	/* Load block to be encrypted/decrypted */
	for (i = 3; i >= 0; i--)
	{
		AES->DATA = u32AES_ByteReverse(*(indata+i));
	}

	/* Wait for completion */
	while(AES->STATUS & AES_STATUS_RUNNING)	;

	/* Save encrypted/decrypted data */
	for (i = 3; i >= 0; i--)
	{
		*(outdata+i) = u32AES_ByteReverse(AES->DATA);
	}
}


