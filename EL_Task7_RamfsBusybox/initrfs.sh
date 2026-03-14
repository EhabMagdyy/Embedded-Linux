#bin/bash

echo "Generating initramfs u-boot file from 'initramfs' directory, and copying it to TFTP server directory..."
cd initramfs
find . | cpio -H newc -o | gzip > ../initramfs.cpio.gz
cd ..
~/Documents/ITI_9Months/EmbeddedLinux/u-boot/tools/mkimage -A arm64 -T ramdisk -C gzip -d initramfs.cpio.gz initramfs.uboot
sudo cp initramfs.uboot /srv/tftp/
ls -la /srv/tftp/
echo "Done."