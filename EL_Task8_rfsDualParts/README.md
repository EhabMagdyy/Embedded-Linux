# Initramfs-Based Root Filesystem Selection

## 1. Setup your Disk
<img width="882" height="563" alt="Image" src="https://github.com/user-attachments/assets/e1fb233b-4bf1-4e11-a6aa-f4fb5969d24c" />

- Create two partitions using `cfdisk`
- Format the Disk:
  ``` bash
  sudo mkfs.vfat -L BOOT /dev/sdb1
  sudo mkfs.ext4 -L ROOTFS1 /dev/sdb2
  sudo mkfs.ext4 -L ROOTFS2 /dev/sdb3
  ```
- Mount & Create the two `rootfs`

---

## 2. Creat ramfs's `init` script

``` bash
#!/bin/sh

# 1. Mount Virtual Filesystems
mount -t proc proc /proc
mount -t sysfs sys /sys
mount -t devtmpfs dev /dev

echo "============================================"
echo "   Initramfs: Root Filesystem Selector"
echo "============================================"

DEV_ROOT1="/dev/sda2"
DEV_ROOT2="/dev/sda3"

# 2. Wait for USB Device Detection
echo "Waiting for storage device to settle..."
while [ ! -b "$DEV_ROOT1" ] || [ ! -b "$DEV_ROOT2" ]; do
    echo "Polling for $DEV_ROOT1 and $DEV_ROOT2..."
    sleep 1
done

echo "Devices detected: $DEV_ROOT1 and $DEV_ROOT2"

# 3. User Selection Prompt
echo ""
echo "Select Root Filesystem Partition:"
echo "1) $DEV_ROOT1 (RootFS 1)"
echo "2) $DEV_ROOT2 (RootFS 2)"
echo -n "Enter choice [1/2]: "

read choice

case $choice in
    1) ROOT_DEV=$DEV_ROOT1 ;;
    2) ROOT_DEV=$DEV_ROOT2 ;;
    *) 
        echo "Invalid input. Defaulting to RootFS 1."
        ROOT_DEV=$DEV_ROOT1
        ;;
esac

# 4. Mount Selected Partition
echo "Mounting $ROOT_DEV to /mnt/root..."

mkdir -p /mnt/root

if mount -t ext4 $ROOT_DEV /mnt/root; then
    echo "Mount successful."
else
    echo "FATAL: Failed to mount $ROOT_DEV. Dropping to shell."
    exec /bin/sh
fi

# 5. Switch Root
echo "Switching root to $ROOT_DEV..."

# Clean up virtual filesystems
umount /proc
umount /sys

# Move /dev to new root (Prevents the "can't open /dev/ttyS0" error)
if [ -d /mnt/root/dev ]; then
    mount --move /dev /mnt/root/dev
    echo "/dev moved to new root."
else
    umount /dev
    echo "Warning: /dev does not exist in new root. Continuing without moving /dev."
fi

# Switch to new root
exec switch_root /mnt/root /sbin/init

# Fallback
echo "FATAL: switch_root failed. Dropping to shell."
exec /bin/sh
```

#### Make it execuatable
``` bash
sudo chmod +x init
```

---

#### 2. Make sure initramfs `bin` has the commands we use
```
initramfs/bin/
├── busybox
├── cat -> busybox
├── cttyhack -> busybox
├── echo -> busybox
├── ls -> busybox
├── mkdir -> busybox
├── mount -> busybox
├── pwd -> busybox
├── sh -> busybox
├── sleep -> busybox
├── switch_root -> busybox
├── umount -> busybox
└── uname -> busybox
```

---

<img width="1920" height="1080" alt="DualRootFS" src="https://github.com/user-attachments/assets/cc13fe90-fdec-4977-8fe8-2815c7660c4c" />

