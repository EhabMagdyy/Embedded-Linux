# RPi3b+ Kernel (with busybox rootsfs & `initramfs`)

> ![Image](https://github.com/user-attachments/assets/744cde5b-beb9-4edc-b09e-718415c18b14)

> the first block of printed message is from `init` > `/etc/inittab` > `/etc/init.d/rcS`

> the second block if from `/etc/profile` at the begining of starting `/bin/sh`, to validate that its working

---

## Note:
we are modifing the previous repo `EL_Task6_RunKernel`, so must of the info is there
here are the new things we did

## First: we need busybox as rootfs base

### 1. Setup busybox
- Download busybox
   ``` bash
   git clone git@github.com:mirror/busybox.git
   cd busybox
   ```

- Configure busybox
   ``` bash
   export ARCH=arm64
   export CROSS_COMPILE=~/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu-
   make defconfig
   make menuconfig
   # in menuconfig
   ```
   ```
   Settings → [*] Build static binary (no shared libs)
            └─> [ ] SHA1 hardware acceleration
            └─> [ ] SHA256 hardware acceleration
   Save & Exit
   ```
   ``` bash
   make ARCH=arm64 \
     CROSS_COMPILE=~/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu- \
     -j$(nproc)

   # verify busybox file:
   file busybox
   # MUST be: ELF 64-bit LSB executable, ARM aarch64, statically linked
   ```

- Install in your nfs rootfs using rsync
   ``` bash
   rm -rf /tmp/busybox_install
   mkdir /tmp/busybox_install

   make ARCH=arm64 \
      CROSS_COMPILE=~/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu- \
      CONFIG_PREFIX=/tmp/busybox_install install

   sudo rsync -av --ignore-existing \
      /tmp/busybox_install/ \
      /srv/nfs/rootfs/
   ```

---

### 2. rootfs setup
- Now, you will have these fs:
   ```
   bin  dev  init  linuxrc  proc  sbin  sys  tmp  usr
   ```

- Create `etc` directory & make your `inittab` script & `init.d/rcS`
   ``` bash
   sudo mkdir /srv/nfs/rootfs/etc
   sudo touch /srv/nfs/rootfs/etc/inittab
   sudo mkdir /srv/nfs/rootfs/etc/init.d
   sudo touch /srv/nfs/rootfs/etc/init.d/rcS
   sudo chmod +x /srv/nfs/rootfs/etc/init.d/rcS
   ```
   
--- 

### 3. U-Boot setup
in `boot.cmd` bootargs `init=/sbin/init`
> the rest same as before

---

### 4. Run kernel as we did before

---

## Second: after we run busybox rootfs using nfs, we need to run initramfs

### 1. Create your initramfs folder
```
initramfs
├── bin
│   ├── busybox   # make other binaries soft link to it
│   ├── cttyhack -> busybox    # assigns the console as a controlling terminal
│   ├── echo -> busybox
│   ├── ls -> busybox
│   ├── mount -> busybox
│   └── sh -> busybox
├── dev
├── etc
│   ├── init.d
│   │   └── rcS   # startup script — runs once at boot before shell spawns (disables logs on console, prints banner)
│   ├── inittab   # BusyBox init config — mount {dev,sys,proc,temp}, run startup script, run shell, shutdown behavior
│   └── profile   # shell init script — runs automatically when login shell starts (-l flag when running shell)
├── init
├── proc
├── sbin
├── sys
└── tmp
```

---

### 2. Make it suitable to run using U-Boot
- Archeive `cpio` & Compress `gz` & generate ramdisk file via `mkimage`
   ``` bash
   cd initramfs
   find . | cpio -H newc -o | gzip > ../initramfs.cpio.gz
   cd ..
   ~/Documents/ITI_9Months/EmbeddedLinux/u-boot/tools/mkimage -A arm64 -T ramdisk -C gzip -d initramfs.cpio.gz initramfs.uboot
   ```

- But it in your tftp/disk to be loaded into DRAM
   ``` bash
   sudo cp initramfs.uboot /srv/tftp/
   ```

---

### 3. Setup boot.cmd
- Change bootargs
  ```
  setenv bootargs "console=tty1 rw rdinit=/init"
  ```

- Load `initramfs.uboot`
  ```
  tftp ${ramdisk_addr_r} initramfs.uboot
  ```

- Run kernel
  ```
  booti ${kernel_addr_r} ${ramdisk_addr_r} ${fdt_addr_r}
  ```

---

## Questions

### 1. What is initramfs? Why use it instead of mounting the real rootfs directly?
A small compressed filesystem loaded into RAM at boot before the real rootfs — used to do early setup (load drivers, mount logic) when then it can load the actual rootfs from (nfs/disk)


### 2. Why cpio format for initramfs? Why not tar or zip?
cpio is hardcoded into the kernel — the kernel's built-in initramfs extractor only understands cpio newc format.


### 3. What does rdinit= do? What happens if wrong path?
Tells the kernel which binary to run as PID 1 inside the initramfs. If wrong path - kernel panic: "requested rdinit not found".


### 4. Why must init be statically linked? What if dynamic?
initramfs has no shared libraries (/lib) — a dynamically linked binary would panic.


### 5. Difference: initramfs vs initrd?
initrd is an old block device image (ext2) mounted as a real disk — the kernel mounts it then pivot_roots. initramfs is a cpio archive extracted directly into a tmpfs — simpler, no block device needed, and is the modern standard.


### 6. Where is initramfs loaded in memory? Who decompresses it?
U-Boot loads it at ${ramdisk_addr_r} then passes the address to the kernel via DTB. The kernel itself decompresses and extracts it into a tmpfs at /.


### 7. How does kernel switch from initramfs to real rootfs?
init script calls `switch_root /newroot /sbin/init` — this pivots / to the real rootfs and re-execs init.

---

  #### You can remove your SD Card/USB Disk After U-boot runs!!
  > https://github.com/user-attachments/assets/2d5dca95-fa27-4c9b-98f8-3b905312cbf8
  
