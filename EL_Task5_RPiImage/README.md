# Run Raspberry Pi 3b+ image via U-Boot
## Using QEMU
### 0. Output
> <img width="1920" height="1080" alt="Image" src="https://github.com/user-attachments/assets/aa1522bc-3776-400e-8738-07c66b9614d3" />
---

### 1. Create Virtual SD Card
as we did in `EL_Task1_vSDCard` Folder

---

### 2. Get RPi3b+ `Image`
- From Raspberry linux repo
- Generate Kernel image
  ``` bash
  export ARCH=arm64
  make bcm2711_defconfig
  # You can customize your image
  make menuconfig
  export CROSS_COMPILE=~/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu-
  # build your image
  make -j8
  make -j modules dtbs
  ```
- now, you will have `Image` uner `arch/arm64/boot/`
- you also have `vmlinux` has debug info

---

### 3. Generate `u-boot.bin` for RPi3b+
``` bash
# in u-boot folder
make rpi_3_b_plus_defconfig
export CROSS_COMPILE=~/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu-
make -j
```

---

### 4. Run QEMU
``` bash
# Kernel logs on QEMU window: 
qemu-system-aarch64 -M raspi3b \
  -cpu cortex-a53 \
  -m 1024 \
  -kernel u-boot.bin \
  -dtb bcm2837-rpi-3-b-plus.dtb \
  -device usb-kbd \
  -sd ../EL_Task1_vSDCard/vsdcard.img \
  -serial stdio \
  -display sdl
# Then set bootargs in qemu:
setenv bootargs "console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 rootwait"

# OR

# Kernel logs on my terminal:
qemu-system-aarch64 -M raspi3b \
  -cpu cortex-a53 \
  -m 1024 \
  -kernel u-boot.bin \
  -dtb bcm2837-rpi-3-b-plus.dtb \
  -device usb-kbd \
  -sd ../EL_Task1_vSDCard/vsdcard.img \
  -serial stdio

### Load & Run Kernel
load mmc 0:1 ${kernel_addr_r} Image
# we passed the dtb with qemu, it's in fdtcontroladdr
booti ${kernel_addr_r} - ${fdtcontroladdr}
```

---

## Using Physical Hardware
### 0. Output
> ![WhatsApp Image 2026-03-12 at 6 42 16 PM](https://github.com/user-attachments/assets/f8b5c206-62e5-405f-a65c-4dfe39cdc3dc)

---

### 1. Partition & Fromat SD Card/USB Disk
### 2. Put your the neeeded files in the FAT partition
```
/path/to/BOOT/
├── bcm2837-rpi-3-b-plus.dtb  # or bcm2710-rpi-3-b-plus.dtb
├── bootcode.bin
├── config.txt
├── fixup.dat
├── Image
├── start.elf
└── u-boot.bin
```

--- 

### 3. Conned & Power on the RPi
> as we did in `EL_Task3_BareMetal`

---

### 4. Load & Run the Kernel
i'm using picocom:
``` bash
sudo picocom -b 115200 /dev/ttyUSB0
```

in U-Boot
```
setenv bootargs "console=tty1,115200"  # output on the external monitor
fatload usb 0:1 ${fdt_addr_r} bcm2837-rpi-3-b-plus.dtb  # mmc if using sd card
fatload usb 0:1 ${kernel_addr_r} Image
booti ${kernel_addr_r} - ${fdt_addr_r}
```
