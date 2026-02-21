## U-Boot

### Part A : U-Boot Build and Deployment

##### 1- Define what you Know about the bootloader?

 It's the first software that runs when a device powers on.
 Its main job is to initialize basic hardware and load the OS into RAM and start it.

##### 2- Draw and Explain the exact boot chain on Raspberry Pi from power-on until you see the U-Boot prompt.

1. Power on
2. Boot Rom (inside SoC)
   	- Runs on the GPU
   	- Initializes minimal hardware to read from storage
   	- looks for a boot device in a fixed order (SD Card -> USB "if available" -> Network "if available")

3. Read Bootcode.bin (older RPis)

    - Loaded from FAT Partition into DRAM
    - Then it loads start.elf file into DRAM
    - start.elf applies settings from `config.txt`
    - prepares ARM CPU to start.
    - loads U-Boot

4.  U-Boot

    - init hardware (Clock, CPU, RAM, Peripherals)
    - loads linux kernel from SD Card/USB/.. into RAM
    - passes boot args
    - starts the kernel

5. Linux Kernel

    - Decompress itself
    - initialiaze MMU, scheduler, Drivers, mount rootfs
    - runs PID 1 (init/systemd)

6. init/systemd

     - start services (network, ssh, display, ...)
     - brings system to target multi-user.target ot graphical.target

7. login/Desktop

 ​    - Console login or GUI Desktop (if enabled).

##### Bounce: Draw and Explain the exact boot chain on your PC from power-on until Running the OS

1. Power ON

2. Firmware (UEFI / Legacy BIOS)

   - Initializes CPU, RAM, chipset, basic devices

   - Performs POST (Power-On Self-Test)

   - Selects boot device (Disk / USB / Network)

3. Boot Manager (from EFI System Partition)

   - Loaded by UEFI from disk

   - Example on Linux: GRUB

   - Allows OS selection and boot options

4. OS Loader (Linux Kernel or Windows Boot Loader)

   - Loads kernel into RAM

   - Loads initramfs (Linux)

   - Passes boot parameters

   - Jumps to kernel entry point

5. OS Kernel

   - Decompresses itself
     - Initializes:
       - MMU
       - Scheduler
       - Drivers

   - Mounts root filesystem

6. Init System (PID 1)

   - Linux: systemd

   - Starts system services (network, display manager, etc.)

7. Login / Desktop
   - Console login or GUI

##### 3- What is the difference between U-Boot and GRUB ?

###### U-Boot

- Used in embedded boards
- Responsible for low-level hardware bring-up
- Loads kernel, DTB, initramfs
- Can flash firmware, recover broken systems

###### GRUB

- Used on PCs
- Firmware (UEFI/BIOS) already initialized hardware
- Focused on OS selection and kernel loading
- Provides boot menu and boot parameters

##### 4- What files must be placed in the Raspberry Pi boot partition to boot U-Boot, and define what is the important of each of them?

- `bootcode.bin`: Responsible for initializing the hardware and loading the next stage bootloader.
- `start.elf`: Responsible for loading the U-Boot bootloader.
- `u-boot.bin`: This is the U-Boot bootloader itself, which is  responsible for loading the OS kernel and providing a command-line interface for the user to interact with.
- `config.txt`: Tells `start.elf` the name of the U-Boot binary to load and other hardware-specific configurations.

##### 5- Build and Test Custom U-Boot in QEMU (Cortex-A9):

##### a. Build U-Boot , Customize U-Boot via menuconfig, and Explain the steps you make to configuration.

```bash
# setup the default configuration for the vexpress_ca9x4 board
make vexpress_ca9x4_defconfig

# setup the cross-compilation environment for ARM architecture
export CROSS_COMPILE=arm-linux-gnueabi-

# customize the U-Boot configuration
make menuconfig

# build
make -j
```

##### b. Run U-Boot in QEMU, and Explain the command you use.

```bash
# Run vexpress on qemu emulator with no graphics option
qemu-system-arm -M vexpress-a9 -kernel u-boot -nographic
```

##### 6- Build and Deploy U-Boot on Real Raspberry Pi 3B+ (AArch64)

##### a. Build U-Boot , Customize U-Boot via menuconfig, and Explain the steps you make to configuration.

```bash
# setup the default configuration for the rpi 3b plus board
make rpi_3_b_plus_defconfig

# setup the cross-compilation environment for RPi ARM architecture
export CROSS_COMPILE=~/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu-

# customize the U-Boot configuration
make menuconfig

# build
make -j
```

##### 7- Which file provides the hardware description to U-Boot on the Raspberry Pi 3B+ and at which stage is it loaded?

the device tree blob file (bcm2837-rpi-3-b-plus.dtb), it's loaded by the GPU firmware `start.elf` stage, before U-Boot runs.

##### 8- After losetup --partscan --show -f sd.img we get devices like /dev/loop5p1 and /dev/loop5p2. Explain how the Linux kernel knows where the partitions start inside
the image file.

Kernel reads the partition table MBR stored inside the image file which has info about the start and size of each partition, The kernel uses this information to create device nodes for each  partition, such as `/dev/loop5p1` which can then be  accessed like regular block devices.

---

### Part B : U-Boot Commands Environment

##### 1- What is the using of “bdinfo” command?

Shows board and memory info.

##### 2- What is the using of “printenv” command?

Displays U-Boot environment variables.

##### 3- What is the DRAM start address?

-> start    = 0x60000000

##### 4- List and Load Files from FAT Partition.
> <img width="775" height="678" alt="Image" src="https://github.com/user-attachments/assets/85fc410d-0024-4d44-b8fe-8eeaa2a89b3e" />

##### 5- Make the U-Boot banner say “Welcome to Our-Boot – Intake 46”

make menuconfig -> General setup -> Local Version
> <img width="956" height="346" alt="Image" src="https://github.com/user-attachments/assets/0f858afe-4340-48d7-a5d0-6bafebbdaacf" />

##### 6- Add a custom command hello that prints your name

Write your command handler inside a file in cmd folder under u-boot, then add `obj-y += cmd_hello.o` in `cmd/Makefile`

> <img width="957" height="615" alt="Image" src="https://github.com/user-attachments/assets/fed1af58-a3f8-4fe4-93ec-eb65f8f5d4cd" />

##### 7- Network Booting with TFTP
###### a. Set Up a TFTP Server on Your Laptop

``` bash
sudo apt install tftpd-hpa
# Copy your files in tftp server
sudo cp file.txt /srv/tftp/
# You can change configuration from here
sudo nano /etc/default/tftpd-hpa
# Restart tftp servicewith the new configurations
sudo systemctl restart tftpd-hpa
```

###### b. From U-Boot (QEMU or Real RPi) Configure Network & Test


###### c. Load Kernel + DTB via TFTP


##### 8- What is the difference between run and go commands?

`go`: Run an application loaded in DRAM that doesn't require `DTB`.
But after this you cannot go back to `u-boot`, it's no longer in DRAM "it actually still there but kernel overwrites it like what happens in C Stacks".

`run`: Used to run variable that continue script

##### 9- What is the purpose of bootargs and who reads it?

This is a string of kernel command-line parameters.

It usually contains: console device, root filesystem, log level
Example:
``` bash
console=ttyAMA0,115200 root=/dev/mmcblk0p2 rootwait rw
```

The Linux kernel reads bootargs, U-Boot does NOT interpret it, it just passes it to kerenel.

##### 10- Why do we use 0x62000000 and not 0x60000000 for kernel address on Raspberry
Pi?

0x60000000 may overlap GPU/U-Boot reserved RAM, but 0x62000000 It lies in a safe region of RAM.


---

### Extra Notes

##### `setenv`
Set environment variable.

##### `saveenv`
Save environment variable you set to be loaded in the next power cycle.

##### `editenv`
Edit an environment variable "You must select it from menuconfig", instead of reseting.

##### Loading and Starting Kernel
1. Load kernel image, dtb, ramfs and other required files into DRAM.
2. Start kernel (`bootz`/`booti`)
