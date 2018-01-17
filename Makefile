drv-objects := $(patsubst %.c,%.o,$(wildcard drv/*.c))
sys-objects := $(patsubst %.c,%.o,$(wildcard sys/*.c))
usb-objects := $(patsubst %.c,%.o,$(wildcard usb/*.c))
src-objects := $(patsubst %.c,%.o,$(wildcard src/*.c))
stdperiph-objects := $(patsubst %.c,%.o,$(wildcard stdperiph/src/*.c))
fsusb-objects := $(patsubst %.c,%.o,$(wildcard fsusb/src/*.c))

startup-object := sys/startup_stm32f10x_md.o

AS := $(CROSS_COMPILE)as
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
OBJDUMP := $(CROSS_COMPILE)objdump
OBJCOPY := $(CROSS_COMPILE)objcopy
SIZE := $(CROSS_COMPILE)size

CFLAGS := -ffreestanding -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD -O2 -flto
CFLAGS += -Isrc -Idrv -Isys -Iusb -Icmsis/inc -Istdperiph/inc -Ifsusb/inc -Icmsis/stm32/inc

CCLDFLAGS := -Wl,-T -Wl,stm32.ld -flto -ffreestanding -nostdlib

# Available plats: BLUEPILL, STLINK_V2_CLONE_DONGLE
PLAT ?= BLUEPILL

CFLAGS += -D$(PLAT)

usb-blaster.axf: $(startup-object) $(drv-objects) $(sys-objects) $(usb-objects) $(src-objects) $(stdperiph-objects) $(fsusb-objects)
	$(CC) $(CCLDFLAGS) $^ -o $@

%.hex: %.axf
	$(OBJCOPY) -O ihex $< $@

%.bin: %.axf
	$(OBJCOPY) -O binary $< $@

%.lst: %.axf
	$(OBJDUMP) -x -S $< > $@

flash: usb-blaster.hex
	st-flash --format ihex write usb-blaster.hex

clean:
	rm -f $(startup-object) $(drv-objects) $(sys-objects) $(usb-objects) $(src-objects) $(stdperiph-objects) $(fsusb-objects)
	rm -f usb-blaster.axf usb-blaster.hex usb-blaster.bin usb-blaster.lst
