CROSS   := ~/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu-
CC      := $(CROSS)gcc
AS      := $(CROSS)as
LD      := $(CROSS)ld
OBJCOPY := $(CROSS)objcopy
OBJDUMP := $(CROSS)objdump

TARGET  := blinky
OBJ     := startup.o main.o
LDSCRIPT := linker.ld

CFLAGS  := -Wall -O2 -ffreestanding -nostdlib -nostartfiles -mgeneral-regs-only -march=armv8-a
ASFLAGS := -march=armv8-a
LDFLAGS := -T $(LDSCRIPT) --no-dynamic-linker

.PHONY: all clean dump

all: $(TARGET).img

startup.o: startup.s
	$(AS) $(ASFLAGS) -o $@ $<

main.o: main.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET).elf: $(OBJ) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(OBJ)

$(TARGET).img: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@
	@echo ""
	@echo "Run in U-Boot: $@  ->  tftp 0x80000 $@  then  go 0x80000"
	@echo ""

dump: $(TARGET).elf
	$(OBJDUMP) -D $<

clean:
	rm -f $(OBJ) $(TARGET).elf $(TARGET).img
