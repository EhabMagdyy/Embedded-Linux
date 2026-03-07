# RPi Bare-Metal : Loading & Running a Custom Binary via U-Boot

## 0. Demo
> https://github.com/user-attachments/assets/0afe0202-9cbf-4ec8-a45d-2d6e5f952cd4

---

## 1. Write the Source Code

### 1.1 Project Structure

```
EL_Task3_BareMetal/
├── startup.s
├── main.c
├── linker.ld
├── Makefile
└── blinky.img        (generated — this is what we deploy)
```

##### `startup.s` — Assembly Entry Point
##### `main.c` — GPIO Blink Logic
##### `linker.ld` — Linker Script
##### `Makefile` — Cross-compile and Generate blinky.img

## 2. Build the Binary

### 2.1 Link the Cross-Compiler
```makefile
CROSS := ~/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu-
```

### 2.2 Build

```bash
make clean && make
```

Expected output:
```
...as startup.o
...gcc main.o
...ld blinky.elf
...objcopy blinky.img
Built: blinky.img
```

### 2.3 Verify (optional)

```bash
make dump    # check _start is at 0x80000 and main follows
```

---

## 3. Prepare the SD Card or USB Flash Drive

### 3.1 Partition and Format

```bash
# Find your SD Card / USB Flash drive
lsblk                        # look for your device e.g. /dev/sdb
# Unmount if mounted
sudo umount /dev/sdb1
# Create Bootable Primary FAT Partition
sudo cfdisk /dev/sdb
# Mount it
sudo mount /dev/sdb1 /mnt
```

### 3.2 Get RPi Firmware Files

```
From https://github.com/raspberrypi/firmware > download those files:
bootcode.bin  start.elf  fixup.dat  bcm2710-rpi-3-b-plus.dtb
```

Copy these 4 files:

```bash
sudo cp ~/Downlodas/bootcode.bin               /mnt/
sudo cp ~/Downlodas/start.elf                  /mnt/
sudo cp ~/Downlodas/fixup.dat                  /mnt/
sudo cp ~/Downlodas/bcm2710-rpi-3-b-plus.dtb   /mnt/
```

### 3.3 Create `config.txt`

```bash
sudo nano /mnt/config.txt
```

Paste:
```ini
arm_64bit=1
kernel=u-boot.bin
enable_uart=1
dtoverlay=disable-bt
```

### 3.4 Configure and Build U-Boot

```bash
make rpi_3_b_plus_defconfig
export CROSS_COMPILE=~/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu-
make -j
```

### 3.5 Copy `blinky.img` (optional: done using tftp in u-boot)

```bash
sudo cp blinky.img /mnt/
```

### 3.6 Verify Contents

```bash
ls /mnt/
```

Expected:
```
bcm2710-rpi-3-b-plus.dtb
bootcode.bin
config.txt
fixup.dat
start.elf
u-boot.bin
blinky.img
```

### 3.7 Unmount

```bash
sudo umount /mnt
```

---

## 4. Hardware Wiring, UART Serial Connection

```
RPi Pin 6  (GND) → USB-TTL GND
RPi Pin 8  (TX)  → USB-TTL RX 
RPi Pin 10 (RX)  → USB-TTL TX 
```

---

## 5. Configure Ethernet for TFTP

### 5.1 Connect Ethernet Cable

### 5.2 Set Static IP on PC

```bash
sudo ip addr add 192.168.2.1/24 dev eno1
sudo ip link set eno1 up
```

### 5.3 Set Up TFTP Server

```bash
# Install
sudo apt install tftpd-hpa

# Copy binary to TFTP root (that's the step that was oprional above)
sudo cp blinky.img /srv/tftp/
sudo chmod 644 /srv/tftp/blinky.img

# Start/restart service
sudo systemctl restart tftpd-hpa
sudo systemctl status tftpd-hpa
```

---

## 6. Boot and Run

### 6.1 Open Serial Console

```bash
sudo apt install picocom
sudo picocom -b 115200 /dev/ttyUSB0
```

### 6.2 Power On RPi

Insert SD Card / USB Flash drive into RPi, power it on.
**Press any key immediately** to stop U-Boot autoboot.

You should see:
```
U-Boot>
```

### 6.3 Load and Run via TFTP

```bash
# Set IPs
setenv ipaddr    192.168.2.2
setenv serverip  192.168.2.1

# Test connection
ping 192.168.2.1

# Load binary into RAM
tftp 0x80000 blinky.img

# Execute
go 0x80000
```

Expected output:
```
## Starting application at 0x00080000 ...
```

Then your LED starts blinking

## Boot Chain Summary

```
USB Drive ROM scan
    └── bootcode.bin        (RPi 2nd stage — mandatory)
        └── start.elf       (GPU firmware — reads config.txt)
            └── u-boot.bin  (run U-Boot that's in config.txt)
                └── tftp → blinky.img at 0x80000
                    └── go 0x80000 → _start → main() → LED blinks
```

---

### Q: Brief explanation of the load address of .img you chose and why
0x80000 is chosen because the RPi3B+ firmware (start.elf) hardwires this as the default kernel load address — it places whatever binary is named in config.txt there and jumps to it. Using this address means zero extra configuration; the hardware and firmware already expect your code to start there.

### Q: Why do we need a startup.s file at all? Can’t we just write everything in C?
`_start` is the default entry point defined by the linker for ELF binaries in Linux, 

### go vs booti
`go`: treats the address as raw binary, jumps directly to it with no parsing. What you used for your bare-metal .img.
`booti`: expects a Linux kernel in ARM64 Image format (has a header). It also takes a device tree and initrd address: booti <kernel_addr> <initrd_addr> <dtb_addr>. It does extra setup before jumping (passes dtb to kernel, sets up registers per Linux boot protocol).
> In short: `go` = dumb jump to raw code. `booti` = smart jump to Linux kernel.
