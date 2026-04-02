# Creating Qt app for RPi 3b+ image using buildroot

## [Demo Video](https://drive.google.com/file/d/1xk5odUrtoB0hmnGsfoXKL2U7VtUm3zpx/view?usp=sharing)

## 1. Download buidlroot
``` bash
git clone https://github.com/buildroot/buildroot.git
```

---

## 2. Configure buildroot for RPi3 64 board
``` bash
cd buildroot
# to list the available boards
ls configs/
make raspberrypi3_64_defconfig
# Customize your image
make menuconfig
```

---

## 3. Create your Qt application & added into packages
### - Create Qt app package
``` bash
ehab@HP:~/buildroot$ tree package/qtcalculator/
package/qtcalculator/
├── CMakeLists.txt
├── Config.in           # Kconfig menu entry — defines the package option shown in menuconfig (bool, depends on, select, help text)
├── qtcalculator.mk     # Buildroot package recipe — defines how to fetch, configure, build and install the package (version, site, dependencies, cmake flags)
├── main.cpp
├── Main.qml
├── MButton.qml
└── resources.qrc
```

---

### - Add the packdge to menuconfig
``` bash
ehab@HP:~/buildroot$ cat package/Config.in | head
menu "Target packages"

	source "package/busybox/Config.in"
	source "package/skeleton/Config.in"
	source "package/skeleton-custom/Config.in"
	source "package/skeleton-init-common/Config.in"
	source "package/skeleton-init-none/Config.in"
	source "package/skeleton-init-openrc/Config.in"
	source "package/skeleton-init-systemd/Config.in"
	source "package/skeleton-init-sysv/Config.in"
```

### - Enable Qt packages & DejaVu fonts
```
Target Packages > Graphics libraries and applications > Qt6
Target packages > Fonts, cursors, icons, sounds and themes > Dejavue
```

### - Build to generate your image
``` bash
make -j16

# Make sure you qt app added to the binaries
ehab@HP:~/buildroot$ ls output/target/usr/bin/ | grep Calc
appTask02_Calculator    # that the app name (from CMakeList.txt)
```

---

## 4. Flash image on your Disk
``` bash
# Wipe the old partition table and sectors
sudo wipefs -a /dev/sdX

# Flash the new image
sudo dd if=output/images/sdcard.img of=/dev/sdX bs=16M conv=fsync status=progress

# Verify partitions were created correctly
sudo fdisk -l /dev/sdX

# Eject and remove your SD Card/USB Disk
sudo eject /dev/sdX
```

---

## 5. inside RPi
``` bash
export LANG=C.UTF-8
export LC_ALL=C.UTF-8
export QT_QPA_PLATFORM=linuxfb
appTask02_Calculator
```

---

## Still having font problems? Copy Dejavue from host (you will need to enable ssh)
``` bash
scp -r /usr/share/fonts/truetype/dejavu/ root@<ipaddr>:/usr/lib/fonts/
```

---

## To enable ssh
### - Make sure you have these checked in `/etc/ssh/sshd_config`:
  ``` bash
  PermitRootLogin yes
  PasswordAuthentication yes
  ```
  
### - assign an ip address to the ethernet interface:
  ``` bash
  ip addr add <ipaddr> dev eth0
  ip link set eth0 up
  ```

---

## BOOT partition is small (needs to be more than 32MB)
``` bash
# Open the template
nano board/raspberrypi/genimage.cfg.in
```

```
Find and change `size = 32M` to `size = 64M`:

image boot.vfat {
    vfat {
        ...
    }
    size = 64M    <= change from 32M
}
```

---

## Understanding Questions
#### 1. Why does Buildroot generate an sdcard.img with two partitions (FAT boot + ext4 root) automatically, while in one single command, but in Labs 01–08 we had to do everything manually? What is the real advantage of this approach in a real product?

Buildroot automates what we did in lab(0-8) in a single command with a configuration menu, because it has scripts that already know how to layout the partitions and copy the generated files into an image file.
The real advantages of `buildroot` are the simplicity, consistency, speed of development and support for many packages which make the development much easier and efficient.


##### 2. Buildroot is extremely popular for small-to-medium embedded systems, but very large projects (Android, automotive, set-top boxes) use Yocto instead. explain the main reason why Yocto wins on huge, multi-board, long-term projects while Buildroot wins on single-board, fast prototypes.

`Yocto` uses a layer architecture, allowing different teams to share and reuse specific hardware support or software stacks across different boards without rebuilding everything from scratch. 

`Buildroot` uses a simple `make menuconfig` approach similar to the Linux kernel, making it much faster to set up, easier to understand, and perfect for getting a single board up and running quickly.
      
