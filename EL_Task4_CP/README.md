## Copy file from PC into SD Card via QEMU

#### 1- Setup Network Between PC and QEMU

```bash
# Run QEMU
sudo qemu-system-arm -M vexpress-a9 -kernel u-boot -nographic -sd ../../Linux/Embedded-Linux/EL_Task1_vSDCard/vsdcard.img -m 512M -nic tap -net nic

# In QEMU
setenv serverip 192.168.100.1
setenv ipaddr 192.168.100.2  
setenv netmask 255.255.255.0
saveenv

# In PC
sudo ip addr add 192.168.100.1/24 dev tap0
sudo ip link set tap0 up
```

#### 2- Load file from PC into DRAM via TFTP

```bash
tftp ${loadaddr} kernel.img
# Verify it's loaded
md ${loadaddr}
```

#### 3- Write file into SC Card from DRAM

```bash
# Write into FAT Partition
fatwrite mmc 0:1 ${loadaddr} kernel.img ${filesize} 
```

#### 4- Verify

```bash
# in PC
# Mount SD Card & see FAT Partitions contents
sudo losetup --partscan --show -f ../../Linux/Embedded-Linux/EL_Task1_vSDCard/vsdcard.img
/dev/loop38
sudo mount /dev/loop38p1 /mnt
ls /mnt/
```

