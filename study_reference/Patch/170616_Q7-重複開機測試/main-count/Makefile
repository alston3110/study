
CC      = arm-linux-gnueabi-gcc
AS      = arm-linux-gnueabi-gcc
LD      = arm-linux-gnueabi-gcc
SIZE    = arm-linux-gnueabi-size
NM      = arm-linux-gnueabi-nm
OBJDUMP = arm-linux-gnueabi-objdump
OBJCOPY = arm-linux-gnueabi-objcopy
RM      = rm

TARGET  = mcu
MEMMAP  = efm32tg-

CFLAGS =  -mcpu=cortex-m3 -mfix-cortex-m3-ldrd -mthumb -msoft-float -g -O2 -s -Wall -ffunction-sections -fdata-sections -fomit-frame-pointer -fno-stack-protector
CFLAGS += -DEFM32TG110F16
CFLAGS += -Ih -I../CMSIS/include/core -I../CMSIS/include/efm32 -I../CMSIS/emlib/inc

AFLAGS = $(CFLAGS)

LDFLAGS = -mcpu=cortex-m3 -Wl,-Map=out/$(TARGET).map,--cref,--build-id=none -nostdlib -Wp,-remap

all:bin

OBJS += o/startup_efm32tg.o
OBJS += o/main.o
OBJS += o/acmp.o
OBJS += o/adc.o
OBJS += o/aes.o
OBJS += o/cmu.o
OBJS += o/gpio.o
OBJS += o/i2c.o
OBJS += o/i2ccmd.o
OBJS += o/rtc.o
OBJS += o/msc.o
OBJS += o/power.o
# OBJS += o/system_efm32tg.o

o/%.o : s/%.S
	$(CC) -c $(CFLAGS) $< -o $@

o/%.o : c/%.c
	$(CC) -c $(CFLAGS) $< -o $@

link:$(OBJS)
	$(LD) $(LDFLAGS) -Tefm32tg-400.ld $(OBJS) -o out/$(TARGET)-400.elf
	$(LD) $(LDFLAGS) -Tefm32tg-2000.ld $(OBJS) -o out/$(TARGET)-2000.elf
	
info:
	$(OBJDUMP) -Mforce-thumb -D out/$(TARGET)-400.elf > out/$(TARGET)-400.lst
	$(OBJDUMP) -Mforce-thumb -D out/$(TARGET)-2000.elf > out/$(TARGET)-2000.lst
	$(NM) -ACSn out/$(TARGET)-400.elf > out/$(TARGET)-400.sym
	$(NM) -ACSn out/$(TARGET)-2000.elf > out/$(TARGET)-2000.sym
	$(OBJCOPY) out/$(TARGET)-400.elf -O binary out/$(TARGET)-400.bin
	$(OBJCOPY) out/$(TARGET)-2000.elf -O binary out/$(TARGET)-2000.bin

path:
	mkdir -p o
	mkdir -p out

bin:path link info


clean:
	$(RM) -f o/*.*
	$(RM) -f out/*.*
