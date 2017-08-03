#ifndef __CONFIG_MAIN_H
#define __CONFIG_MAIN_H

#define EFM32_POWER_SOURCE_DC      1
#define EFM32_POWER_SOURCE_CELL_BATTERY 0

#define FOR_LOOP_TIME_1MS  95



/*GPIO Setting*/
#define MAIN_POWER_ON_GPIO_PORT  0
#define MAIN_POWER_ON_GPIO_PIN   0
#define MAIN_POWER_ON_GPIO_MODE_SET() GPIO->P[MAIN_POWER_ON_GPIO_PORT].MODEL=((GPIO->P[MAIN_POWER_ON_GPIO_PORT].MODEL& (~_GPIO_P_MODEL_MODE0_MASK))|(GPIO_P_MODEL_MODE0_PUSHPULL))
#define MAIN_POWER_ON_GPIO_MODE_SET_INPUT() GPIO->P[MAIN_POWER_ON_GPIO_PORT].MODEL|=GPIO_P_MODEL_MODE0_INPUT
#define MAIN_POWER_ON_GPIO_OUT        mCommon_RWMemAddr(mCommon_BitBand((int)&GPIO->P[MAIN_POWER_ON_GPIO_PORT].DOUT,MAIN_POWER_ON_GPIO_PIN))
#define MAIN_POWER_ON_GPIO_IN         mCommon_RWMemAddr(mCommon_BitBand((int)&GPIO->P[MAIN_POWER_ON_GPIO_PORT].DIN,MAIN_POWER_ON_GPIO_PIN))

#define POWER_BUTTON_GPIO_PORT  2
#define POWER_BUTTON_GPIO_PIN   1
#define POWER_BUTTON_GPIO_MODE_SET() GPIO->P[POWER_BUTTON_GPIO_PORT].MODEL|=GPIO_P_MODEL_MODE1_INPUT
                                      
#define POWER_BUTTON_GPIO_IN         mCommon_RWMemAddr(mCommon_BitBand((int)&GPIO->P[POWER_BUTTON_GPIO_PORT].DIN,POWER_BUTTON_GPIO_PIN)) 
#define POWER_BUTTON_GPIO_INT_SET()  do{GPIO->EXTIPSELL|=GPIO_EXTIPSELL_EXTIPSEL1_PORTC; \
                                        GPIO->EXTIFALL|=(1<<POWER_BUTTON_GPIO_PIN); \
                                        GPIO->EXTIRISE|=(1<<POWER_BUTTON_GPIO_PIN); \
                                        GPIO->IFC|=(1<<POWER_BUTTON_GPIO_PIN);  \
                                        GPIO->IEN|=(1<<POWER_BUTTON_GPIO_PIN);}while(0)                                        

#define PMIC_ON_OFF_REQ_GPIO_PORT  1
#define PMIC_ON_OFF_REQ_GPIO_PIN   11
#define PMIC_ON_OFF_REQ_GPIO_MODE_SET() GPIO->P[PMIC_ON_OFF_REQ_GPIO_PORT].MODEH|=GPIO_P_MODEH_MODE11_PUSHPULL
#define PMIC_ON_OFF_REQ_GPIO_MODE_CLEAR() GPIO->P[PMIC_ON_OFF_REQ_GPIO_PORT].MODEH &= ~_GPIO_P_MODEH_MODE11_MASK
#define PMIC_ON_OFF_REQ_GPIO_OUT        mCommon_RWMemAddr(mCommon_BitBand((int)&GPIO->P[PMIC_ON_OFF_REQ_GPIO_PORT].DOUT,PMIC_ON_OFF_REQ_GPIO_PIN))


#define MCU_DC_DETECT_GPIO_PORT   2
#define MCU_DC_DETECT_GPIO_PIN    0
#define MCU_DC_DETECT_GPIO_MODE_SET() GPIO->P[MCU_DC_DETECT_GPIO_PORT].MODEL|=GPIO_P_MODEL_MODE0_INPUT
#define MCU_DC_DETECT_GPIO_IN         mCommon_RWMemAddr(mCommon_BitBand((int)&GPIO->P[MCU_DC_DETECT_GPIO_PORT].DIN,MCU_DC_DETECT_GPIO_PIN))                                          
#define MCU_DC_DETECT_GPIO_INT_SET()  do{GPIO->EXTIPSELL|=GPIO_EXTIPSELL_EXTIPSEL0_PORTC; \
                                        GPIO->EXTIFALL|=(1<<MCU_DC_DETECT_GPIO_PIN); \
                                        GPIO->EXTIRISE|=(1<<MCU_DC_DETECT_GPIO_PIN); \
                                        GPIO->IFC|=(1<<MCU_DC_DETECT_GPIO_PIN); \
                                        GPIO->IEN|=(1<<MCU_DC_DETECT_GPIO_PIN);}while(0)
                    
#define IMX6_INT_GPIO_PORT         2
#define IMX6_INT_GPIO_PIN          15
#define IMX6_INT_GPIO_MODE_SET()   GPIO->P[IMX6_INT_GPIO_PORT].MODEH|=GPIO_P_MODEH_MODE15_PUSHPULL
#define IMX6_INT_GPIO_OUT          mCommon_RWMemAddr(mCommon_BitBand((int)&GPIO->P[IMX6_INT_GPIO_PORT].DOUT,IMX6_INT_GPIO_PIN))

#define IMX6_RESET_GPIO_PORT         5
#define IMX6_RESET_GPIO_PIN          2
#define IMX6_RESET_GPIO_MODE_SET()   GPIO->P[IMX6_RESET_GPIO_PORT].MODEL|=GPIO_P_MODEL_MODE2_PUSHPULL
#define IMX6_RESET_GPIO_OUT          mCommon_RWMemAddr(mCommon_BitBand((int)&GPIO->P[IMX6_RESET_GPIO_PORT].DOUT,IMX6_RESET_GPIO_PIN))
                                          

#define MCU_BR2032_GPIO_PORT       2
#define MCU_BR2032_GPIO_PIN        14
#define MCU_BR2032_GPIO_MODE_SET()   GPIO->P[MCU_BR2032_GPIO_PORT].MODEH|=GPIO_P_MODEH_MODE14_WIREDAND
#define MCU_BR2032_GPIO_OUT          mCommon_RWMemAddr(mCommon_BitBand((int)&GPIO->P[IMX6_INT_GPIO_PORT].DOUT,IMX6_INT_GPIO_PIN))

/*I2C Setting*/
#define I2C_SDA_GPIO_PORT     4  
#define I2C_SDA_GPIO_PIN      12                                    
#define I2C_SCL_GPIO_PORT     4                                     
#define I2C_SCL_GPIO_PIN      13
#define I2C_GPIO_MODE_SET()   GPIO->P[I2C_SDA_GPIO_PORT].MODEH|=(GPIO_P_MODEH_MODE12_WIREDANDFILTER|GPIO_P_MODEH_MODE13_WIREDANDFILTER)
#define I2C_ADDR  (0xc<<1)  
#define I2C_LOCATION    I2C_ROUTE_LOCATION_LOC6
#define I2C_IEN         (I2C_IEN_ADDR|I2C_IEN_RXDATAV|I2C_IEN_SSTOP|I2C_IEN_CLTO)                                         

     
                                        
#endif
