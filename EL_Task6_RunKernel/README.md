# Build and Boot RPi3b+ Kernel (load rootsfs from disk/nfs with custom init)

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

## To Append your name or ID to the kernel version (e.g., 6.6.15-YourName-v1)
```
sed -i 's/CONFIG_LOCALVERSION=.*/CONFIG_LOCALVERSION="-Ehab-v1"/' ~/linux/.config
# then rebuild the image
```

---

## Run using NFS Server

### 0. output
> ![Image](https://github.com/user-attachments/assets/f4603db8-e920-4c19-ae73-4e6b44bdbb1e)

---

### 1. Set NFS Server on Host side

1. Install NFS server:
   ``` bash
   sudo apt install nfs-kernel-server
   ```
2. Create the NFS root directory and populate it with your rootfs:
   ``` bash
   sudo mkdir -p /srv/nfs/rootfs
   
   # copy init from disk rootfs to NFS
   sudo cp -a /media/ehab/ROOTFS/. /srv/nfs/rootfs/
   ```
3. Create the NFS root directory and populate it with your rootfs:
   ``` bash
   sudo nano /etc/exports
   ```
   - Add this line:
   ```
   /srv/nfs/rootfs  192.168.2.2(rw,no_root_squash,no_subtree_check,sync)
   ```

4. Apply and start:
   ``` bash
   sudo exportfs -ra
   sudo systemctl restart nfs-kernel-server
   
   # verify it's exported
   sudo exportfs -v
   ```
   
5. Set network
   ``` bash
   sudo ip addr add 192.168.2.1/24 dev eno1
   sudo ip link set eno1 up
   ```

---

### 2. in RPi side
1. Change bootargs in your `boot.cmd`:
   ```
   # replace the current bootargs line with this:
   setenv bootargs "console=tty1,115200 root=/dev/nfs nfsroot=192.168.2.1:/srv/nfs/rootfs,v3,tcp ip=192.168.2.2:192.168.2.1::255.255.255.0::eth0:off rw init=/init"
   ```
2. Then recompile and copy `boot.scr` into `boot` partition

> Now your Kernel loads from NFS Server

---

## Questions about kernel concepts
### 1. Monolithic vs Microkernel — Where does Linux stand?

| | Monolithic (Linux) | Microkernel (QNX) |
|---|---|---|
| core services | Kernel space | Kernel space & User space |
| Speed | Fast (direct calls) | Slower (IPC between servers) |
| Crash impact | One bad driver = kernel panic | One bad driver = restart that server only |

---

### 2. Why does every embedded device use Linux instead of QNX?

For most embedded devices (phones, TVs, routers) "good enough" latency + free + rich ecosystem beats "perfect" real-time + cost + limited support.

---

### 3. What is Android GKI and why did Google force it from Android 13?

**GKI (Generic Kernel Image)** is a single standardized kernel binary that Google compiles and signs — vendors add hardware-specific code only via **loadable kernel modules (LKMs)**, never by patching the core kernel. Google forced it from Android 12/13 to end the fragmentation nightmare where every vendor shipped a different kernel fork, making security patches and Android version updates take 1–2 years to reach devices (or never).

```
Before GKI:  Google releases patch → each vendor manually re-ports to their fork → 18 months later (maybe)
After  GKI:  Google releases patch → drop-in kernel update → all certified devices get it immediately
```

---

### 4. Why clone `raspberrypi/linux` instead of `torvalds/linux` for RPi?

`raspberrypi/linux` is the Raspberry Pi Foundation's fork with all board-specific drivers, device tree patches, and firmware interfaces already integrated and tested on real hardware. that are not in `torvalds/linux

---

### 5. Kernel image formats explained

| Image | Description |
|---|---|
| `vmlinux` | Raw uncompressed ELF kernel — output of the linker, used for debugging/symbols only, **not bootable directly** |
| `zImage` | Compressed ARM32 kernel with a self-decompression stub (ARM32) |
| `Image` | Raw uncompressed ARM64 kernel binary — stripped ELF, **bootable directly** (ARM64) |
| `Image.gz` | Gzip-compressed `Image` — smaller for transfer, bootloader must decompress before booting manually |
| `uImage` | `zImage` or `Image` wrapped with a 64-byte U-Boot header (`mkimage`) — tells U-Boot load address, entry point, CRC, ... |

```
vmlinux  (ELF, debug only)
   └─> strip/objcopy
         └─> Image       (raw binary, ARM64)
               └─> gzip → Image.gz
               └─> mkimage → uImage   (U-Boot header added)
         └─> compress → zImage  (ARM32, self-decompressing)
               └─> mkimage → uImage   (U-Boot header added)
```

---

### 6. Why `fdt_addr_r` for DTB? What is DTB?

A **DTB (Device Tree Blob)** is a binary file describes the hardware layout of a board (like which UART is at which address) - the kernel reads it at boot instead of having hardware details hardcoded. 
`fdt_addr_r` is the **U-Boot environment variable** that holds the RAM address where the DTB should be loaded.

---

### 7. `bootargs` parameters explained

| Parameter | Meaning |
|---|---|
| `root=/dev/sda2` | Which block device holds the root filesystem - kernel mounts this as `/` |
| `rootfstype=ext4` | Filesystem type of the root partition - skips auto-detection, speeds up mount |
| `console=tty1` | Where the kernel prints boot messages |
| `init=/init` | Path to the first userspace process (PID 1) — defaults to `/sbin/init` if omitted |

---

### 8. Why `bootz` for ARM32 and `booti` for ARM64?

`bootz` boots a **compressed `zImage`** which is the standard for ARM32
`booti` boots a raw **`Image`** which is needed for ARM64, cause it dropped self-decompressing kernels entirely.

---

### 9. What causes `VFS: Unable to mount root fs` panic?

missing `root=` in `bootargs` or wrong device

---

### 10. Why does `init.c` need `-static`? What if not?

because the compiler bundles the entire C library (`libc`) **inside the binary**. Without `-static`, the binary expects `ld.so` (the dynamic linker) and we don't have a `/lib` on the rootfs, so the dynamic linker is not found and the kernel panics with `No working init found`.

```
Dynamic (no -static):  kernel runs /init → ld.so looks for libc.so in /lib → not found → panic
Static   (-static):    kernel runs /init → fully self-contained → runs immediately
```

---

### 11. You passed `init=/bin/sh` but it still panics, why?

- `/bin/sh` does not exist on rootfs
- `/bin/sh` is not staticlly linked

