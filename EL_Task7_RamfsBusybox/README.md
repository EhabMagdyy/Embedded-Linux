# RPi3b+ Kernel (with busybox rootsfs & `initramfs`)

> 

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
   Settings → [*] Build static binary (no shared libs)
            └─> [ ] SHA1 hardware acceleration
            └─> [ ] SHA256 hardware acceleration
   Save & Exit
   ```
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
   ```
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