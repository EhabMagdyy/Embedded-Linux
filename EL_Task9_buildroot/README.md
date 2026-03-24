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