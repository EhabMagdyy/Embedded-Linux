# Initramfs-Based Root Filesystem Selection

## 1. Creat ramfs's `init` script

``` bash
#!/bin/sh

# 1. Mount Essential Virtual Filesystems
mount -t proc proc /proc
mount -t sysfs sys /sys
mount -t devtmpfs dev /dev

echo "============================================"
echo "   Initramfs: Root Filesystem Selector"
echo "============================================"

# i'm using usb disk, in case of SD card, it would be /dev/mmcblk0p2 and /dev/mmcblk0p3
DEV_ROOT1="/dev/sda2"
DEV_ROOT2="/dev/sda3"

# 2. Wait for USB/SD Device Detection
echo "Waiting for storage device to settle..."
# Loop until the partitions appear in /dev
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
    1)
        ROOT_DEV=$DEV_ROOT1
        ;;
    2)
        ROOT_DEV=$DEV_ROOT2
        ;;
    *)
        echo "Invalid input. Defaulting to RootFS 1."
        ROOT_DEV=$DEV_ROOT1
        ;;
esac

# 4. Mount Selected Partition
echo "Mounting $ROOT_DEV to /mnt/root..."

# Create mount point
mkdir -p /mnt/root

# Attempt to mount
if mount -t ext4 $ROOT_DEV /mnt/root; then
    echo "Mount successful."
else
    echo "FATAL: Failed to mount $ROOT_DEV. Dropping to shell."
    exec /bin/sh
fi

# 5. Switch Root
echo "Switching root to $ROOT_DEV..."

# Clean up virtual filesystems so the new OS can mount its own
umount /proc
umount /sys
umount /dev

# Execute switch_root to transition to the new root filesystem
exec switch_root /mnt/root /sbin/init

# If switch_root fails, we fall back to shell
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