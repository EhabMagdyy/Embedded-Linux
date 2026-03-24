# Creating RPi 3b+ image using buildroot

## 1. Download buidlroot
``` bash
git clone https://github.com/buildroot/buildroot.git
```

## 2. Build RPi3 64 board config
``` bash
cd buildroot
# to list the available boards
ls configs/
make raspberrypi3_64_defconfig
# Customize your image
make menuconfig
make -j16   # took an hour with me
```

## 3. Flash image on your Disk
``` bash
sudo dd if=output/images/sdcard.img of=/dev/sdb bs=16M
sync
```

---

## Understanding Questions
#### 1. Why does Buildroot generate an sdcard.img with two partitions (FAT boot + ext4 root) automatically, while in one single command, but in Labs 01–08 we had to do everything manually? What is the real advantage of this approach in a real product?

Buildroot automates what we did in lab(0-8) in a single command with a configuration menu, because it has scripts that already know how to layout the partitions and copy the generated files into an image file.
The real advantages of `buildroot` are the simplicity, consistency, speed of development and support for many packages which make the development much easier and efficient.


##### 2. Buildroot is extremely popular for small-to-medium embedded systems, but very large projects (Android, automotive, set-top boxes) use Yocto instead. explain the main reason why Yocto wins on huge, multi-board, long-term projects while Buildroot wins on single-board, fast prototypes.

`Yocto` uses a layer architecture, allowing different teams to share and reuse specific hardware support or software stacks across different boards without rebuilding everything from scratch. 

`Buildroot` uses a simple `make menuconfig` approach similar to the Linux kernel, making it much faster to set up, easier to understand, and perfect for getting a single board up and running quickly.
      
