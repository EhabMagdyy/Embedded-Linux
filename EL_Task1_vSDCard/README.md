## 📀 Creating a Virtual SD Card

### 1. Create Empty Image File (1GB)

```bash
touch vsdcard.img
dd if=/dev/zero of=vsdcard.img bs=1M count=1024
```

> 1024+0 records in
> 1024+0 records out
> 1073741824 bytes (1.1 GB, 1.0 GiB) copied, 2.94177 s, 365 MB/s

### 2. Create Partitions Inside the Image

```bash
cfdisk vsdcard.img
```

> <img width="917" height="608" alt="Image" src="https://github.com/user-attachments/assets/4b8b4a9f-cbc9-4aba-bdb1-d5d541db514f" />


###  3. Attach Image to Loop Device

```bash
sudo losetup -Pf --show vsdcard.img
```

> /dev/loop12
>
> The `-P` flag tells Linux to scan partitions and create:
>
> ```bash
> /dev/loop12p1
> /dev/loop12p2
> ```

###  4. Format the Partitions

```bash
# Format Boot Partition (FAT16)
sudo mkfs.vfat /dev/loop12p1 -n BOOT -F 16
# Format Root Filesystem (ext4)
sudo mkfs.ext4 /dev/loop12p2 -L rootfs
```

### 5. Mount Partitions

```bash
mkdir boot rootfs

sudo mount /dev/loop12p1 boot
sudo mount /dev/loop12p2 rootfs
```

### 6. Unmount and Detach Loop Device

```bash
sudo umount boot
sudo umount rootfs

sudo losetup -d /dev/loop12
```

