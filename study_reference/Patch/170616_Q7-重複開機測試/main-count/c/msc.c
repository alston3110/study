#define DEF_IS_MSC_C
#include "common.h"

void vMSC_Init( void )
{
    /* Write MSC unlock code to enable interface */
    MSC->WRITECTRL |= MSC_WRITECTRL_WREN;
    MSC->LOCK = MSC_UNLOCK_CODE;
    /* Enable memory controller */
    MSC->WRITECTRL = MSC_WRITECTRL_WREN_DEFAULT;
}

void vMSC_Release( void )
{
    /* Write MSC unlock code to enable interface */
    MSC->WRITECTRL |= MSC_WRITECTRL_WREN ;
    MSC->LOCK = 0 ;
    /* Enable memory controller */
    MSC->WRITECTRL = MSC_WRITECTRL_WREN_DEFAULT ;
}

__ramfunc bool MSCWrite(uint8_t* writeData,uint32_t len,uint32_t* addr)
{
    int i;
    __asm("CPSID I");    //disable IRQ
    MSC->WRITECTRL=MSC_WRITECTRL_WREN;
    for(i=0;i<len;i+=4)
    {
        MSC->ADDRB=*addr;
        MSC->WRITECMD=MSC_WRITECMD_LADDRIM;
        if(MSC->STATUS & MSC_STATUS_INVADDR)
        {
            MSC->WRITECTRL= MSC_WRITECTRL_WREN_DEFAULT;
            __asm("CPSIE I");    //Enable IRQ
            return false;
        }
        if(MSC->STATUS & MSC_STATUS_LOCKED)
        {
            MSC->WRITECTRL= MSC_WRITECTRL_WREN_DEFAULT;
            __asm("CPSIE I");    //Enable IRQ
            return false;
        }
        while(!(MSC->STATUS & MSC_STATUS_WDATAREADY));
        MSC->WDATA=(uint32_t)*((uint32_t*)writeData);
        MSC->WRITECMD |=MSC_WRITECMD_WRITEONCE;
        while(MSC->STATUS & MSC_STATUS_BUSY);
        (*addr)+=4;
        writeData+=4;
    }
    MSC->WRITECTRL=MSC_WRITECTRL_WREN_DEFAULT;
    __asm("CPSIE I");    //Enable IRQ
    return true;
}

__ramfunc bool MSCErase(uint32_t addr)
{
    __asm("CPSID I");    //disable IRQ
    MSC->WRITECTRL=MSC_WRITECTRL_WREN;
    MSC->ADDRB=addr;
    MSC->WRITECMD=MSC_WRITECMD_LADDRIM;
    if(MSC->STATUS & MSC_STATUS_INVADDR)
    {
        MSC->WRITECTRL= MSC_WRITECTRL_WREN_DEFAULT;
        __asm("CPSIE I");    //Enable IRQ
        return false;
    }
    if(MSC->STATUS & MSC_STATUS_LOCKED)
    {
        MSC->WRITECTRL= MSC_WRITECTRL_WREN_DEFAULT;
        __asm("CPSIE I");    //Enable IRQ
        return false;
    }
    while(!(MSC->STATUS & MSC_STATUS_WDATAREADY));
    MSC->WRITECMD |=MSC_WRITECMD_ERASEPAGE;
    while(MSC->STATUS & MSC_STATUS_BUSY);
    MSC->WRITECTRL=MSC_WRITECTRL_WREN_DEFAULT;
    __asm("CPSIE I");    //Enable IRQ
    return true;
}
