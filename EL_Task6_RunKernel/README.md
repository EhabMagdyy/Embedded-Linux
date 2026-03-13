# Build and Boot RPi3b+ Kernel

> ![WhatsApp Image 2026-03-13 at 6 55 33 AM](https://github.com/user-attachments/assets/3ec8b8e8-3d10-4858-bded-61b98b7bbb0a)

---

## 0. overview of SD Card/USB Disk content:
### BOOT partition (FAT16) — `mmc/usb 0:1`

| File | Description |
|------|-------------|
| `bootcode.bin` | RPi first-stage bootloader — loaded by GPU on power-on, runs before anything else |
| `start.elf` | RPi GPU firmware — loads and starts the ARM CPU |
| `fixup.dat` | Memory split config between GPU and ARM CPU |
| `config.txt` | RPi firmware config — sets `kernel=u-boot.bin` and `arm_64bit=1` to hand off to U-Boot |
| `u-boot.bin` | U-Boot bootloader — manages boot sequence (USB → TFTP fallback) |
| `boot.scr` | Compiled U-Boot boot script (`boot.cmd` → `mkimage`) — found automatically by `bootflow scan` |
| `Image` | Raw ARM64 Linux kernel (uncompressed) — loaded by U-Boot via `booti` |
| `bcm2837-rpi-3-b-plus.dtb` | Device Tree Blob — describes RPi3B+ hardware to the kernel |

---

### ROOTFS partition (EXT4) — `mmc/usb 0:2`

| File | Description |
|------|-------------|
| `init` | Custom PID 1 init process + minimal shell (`init.c`) - cross-compiled statically for ARM64 |

---

## 1. Generate & Get needed files
- Build `Image`
- Get `bcm2837-rpi-3-b-plus.dtb`
- Generate `u-boot.bin`
- get Raspbery pi firmware files (`bootcode.bin`, `fixup.dat`, `start.elf`) from **raspberry pi firmaware rep**
- create `config.txt` as we did before

### Now U-Boot can run & load kernel but you would get a kernel panic

---

## 2. Making a U-Boot script to autoboot kernel at power on
1. create `boot.cmd` file with your commands
   - First from USB Disk (usb 0:1)
   - If not found → automatically falls back to TFTP from your laptop
   - If both fail → prints a clear error message
2. generate `boot.scr`
   ``` bash
   ~/Documents/ITI_9Months/EmbeddedLinux/u-boot/tools/mkimage -C none -A arm64 -T script -d boot.cmd boot.scr
   
   # For autoboot
   sudo cp boot.scr /path/to/boot/  # this will make the lernel autoboot
   # by default bootcmd=bootflow scan   -> bootflow scan searches for boot.scr and run it

   # if you don't want autoboot, you can put it in /srv/tftp
   tftp ${scriptaddr} boot.scr  # load into DRAM
   source ${scriptaddr}  # this will run the script
   ```

### A) no root at bootargs
```
setenv bootargs "console=tty1"
```
#### kernel panic: Unable to mount root fs
>![WhatsApp Image 2026-03-13 at 6 47 33 AM](https://github.com/user-attachments/assets/be9b4e9c-8102-414d-85f4-7caa238d5946)


### A) correct root is in bootargs
```
# make it /dev/mmblk0p2 if using sd card
setenv bootargs "console=tty1 root=/dev/sda2 rootwait rw"
```
#### kernel panic: No working init found
> ![WhatsApp Image 2026-03-13 at 6 47 33 AM(1)](https://github.com/user-attachments/assets/1068e2fe-9b54-45fb-9c94-84bf2f7aab7e)

---

## 3. add `/init` in rootfs to escape kernel panic
#### Create `init.c` which is a custom shell
   - Cross compile it staticlly for arm64
     ``` bash
     ~/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu-gcc -static -o init init.c
     ```
   - cp init /path/to/rootfs/
   - pass it in bootargs
     ```
     setenv bootargs "console=tty1 root=/dev/sda2 rootwait rw init=/init panic=5"
     ```
#### autoboot without kernel panic!
> https://github.com/user-attachments/assets/f6340a00-87fe-4cb5-aaac-042c601e1a82

---
