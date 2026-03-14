# =============================================================================
# boot.cmd — U-Boot Boot Script for Raspberry Pi 3B+
# =============================================================================
#
# KERNEL NOTE:
#   "Image" is the raw uncompressed ARM64 kernel → use booti (NOT bootz).
#   bootz  = 32-bit ARM  zImage
#   booti  = 64-bit ARM  Image   ← correct for RPi3B+
#
# BOOT ORDER:
#   1. USB Disk  (usb 0:1)
#   2. TFTP      (server 192.168.2.1 → board 192.168.2.2)
#   3. Error message + drop to U-Boot prompt
#
# HOW TO COMPILE:
#   mkimage -C none -A arm64 -T script -d boot.cmd boot.scr
#
# WHERE TO PLACE boot.scr:
#   Copy boot.scr to the root of the FAT partition on your USB disk (usb 0:1),
#   alongside Image and bcm2837-rpi-3-b-plus.dtb:
#
#     usb 0:1/
#     ├── Image
#     ├── bcm2837-rpi-3-b-plus.dtb
#     └── boot.scr                  ← here
#
# HOW IT RUNS AUTOMATICALLY:
#   This board uses the modern U-Boot boot standard. bootcmd is already set to:
#
#     bootcmd=bootflow scan
#
#   On every power-on/reset, U-Boot runs "bootflow scan" which automatically
#   scans all devices (USB, MMC, ...), finds boot.scr on usb 0:1, loads it
#   into ${scriptaddr}, and executes it — no setenv or saveenv needed.
#
#   NOTE: saveenv is NOT used here because the environment is stored on MMC
#   and this setup boots exclusively from USB.
#
# =============================================================================

echo ""
echo "============================================="
echo "   RPi3B+ Auto-Boot — $(date) "
echo "   Order: [1] USB  →  [2] TFTP  →  [3] Error"
echo "============================================="
echo ""


# =============================================================================
# BOOT ARGUMENTS  (shared by both USB and TFTP boot paths)
# =============================================================================
#   console=tty1          → kernel messages on HDMI monitor
#   console=ttyS1,115200  → also mirror to UART (remove if not using serial)
#   root=                 → CHANGE this to match your root partition:
#                             USB boot  → /dev/sda2   (typical)
#                             NFS boot  → nfsroot=192.168.2.1:/path,v3 ip=dhcp
# =============================================================================

setenv bootargs "console=tty1 rw init=/init"


# =============================================================================
# ATTEMPT 1 — USB Disk  (usb 0:1)
# =============================================================================

echo ">>> [1/2] Scanning USB bus..."
usb start
echo ""
# echo ">>> [1/2] Attempting USB boot from usb 0:1 ..."
# echo ""

# if fatload usb 0:1 ${kernel_addr_r} Image; then

#     echo "    [OK] Kernel  'Image'                   loaded from USB"

#     if fatload usb 0:1 ${fdt_addr_r} bcm2837-rpi-3-b-plus.dtb; then

#         echo "    [OK] DTB     'bcm2837-rpi-3-b-plus.dtb' loaded from USB"

#         if fatload usb 0:1 ${ramdisk_addr_r} initramfs.uboot; then
#             echo "    [OK] Initramfs 'initramfs.uboot'      loaded from USB"
#         else
#             echo "    [--] Initramfs not found on USB — booting without it"
#             setenv ramdisk_addr_r "-"
#         fi

#         echo ""
#         echo ">>> Booting kernel from USB ..."
#         echo ""
#         booti ${kernel_addr_r} ${ramdisk_addr_r} ${fdt_addr_r}

#         # booti only returns on failure — if we reach here something went wrong
#         echo ""
#         echo "    [!!] booti returned unexpectedly — image may be corrupt or"
#         echo "         incompatible. Falling through to TFTP ..."

#     else
#         echo "    [--] DTB 'bcm2837-rpi-3-b-plus.dtb' not found on USB (usb 0:1)"
#     fi

# else
#     echo "    [--] Kernel 'Image' not found on USB (usb 0:1)"
#     echo "         (disk not connected, wrong partition, or wrong filesystem?)"
# fi

# echo ""


# =============================================================================
# ATTEMPT 2 — TFTP  (laptop 192.168.2.1  →  board 192.168.2.2)
# =============================================================================

echo ">>> [2/2] Attempting TFTP boot ..."
echo "          board  ip : 192.168.2.2"
echo "          server ip : 192.168.2.1"
echo ""

setenv ipaddr   192.168.2.2
setenv serverip 192.168.2.1

# Override bootargs for TFTP/NFS boot if you need a different root device.
# Uncomment and adjust the line below; otherwise the USB bootargs above are used.
# setenv bootargs "console=tty1 console=ttyS1,115200 root=/dev/nfs nfsroot=192.168.2.1:/nfsroot,v3 ip=192.168.2.2:192.168.2.1::255.255.255.0 rw"

if tftp ${kernel_addr_r} Image; then

    echo "    [OK] Kernel  'Image'                   loaded via TFTP"

    if tftp ${fdt_addr_r} bcm2837-rpi-3-b-plus.dtb; then

        echo "    [OK] DTB     'bcm2837-rpi-3-b-plus.dtb' loaded via TFTP"

        if tftp ${ramdisk_addr_r} initramfs.uboot; then
            echo "    [OK] Initramfs 'initramfs.uboot'      loaded via TFTP"
        else
            echo "    [--] Initramfs not found on TFTP — booting without it"
            setenv ramdisk_addr_r "-"
        fi

        echo ""
        echo ">>> Booting kernel from TFTP ..."
        echo ""
        booti ${kernel_addr_r} ${ramdisk_addr_r} ${fdt_addr_r}

        echo ""
        echo "    [!!] booti returned unexpectedly — image may be corrupt."

    else
        echo "    [--] DTB 'bcm2837-rpi-3-b-plus.dtb' not found on TFTP server"
    fi

else
    echo "    [--] Kernel 'Image' not found on TFTP server"
    echo "         (check: ethernet cable, tftpd running, server IP correct?)"
fi

echo ""


# =============================================================================
# ALL BOOT ATTEMPTS FAILED
# =============================================================================

echo "============================================="
echo " !!!        ALL BOOT ATTEMPTS FAILED      !!!"
echo "============================================="
echo ""
echo " Checked:"
echo "   1. USB  → usb 0:1  (fatload)"
echo "      Expected files: Image"
echo "                      bcm2837-rpi-3-b-plus.dtb"
echo "                      initramfs.uboot"
echo ""
echo "   2. TFTP → server 192.168.2.1"
echo "      Expected files: Image"
echo "                      bcm2837-rpi-3-b-plus.dtb"
echo "                      initramfs.uboot"
echo ""
echo " Common causes:"
echo "   - USB disk not connected, wrong partition number,"
echo "     or filesystem is not FAT"
echo "   - Filename mismatch (check case — FAT is case-sensitive"
echo "     in U-Boot)"
echo "   - TFTP server not started on laptop"
echo "     (sudo systemctl start tftpd-hpa)"
echo "   - Network cable unplugged or wrong IP addresses"
echo "   - Files not in TFTP root directory"
echo "     (usually /srv/tftp or /var/lib/tftpboot)"
echo ""
echo " Dropping to U-Boot prompt."
echo " Useful commands:"
echo "   usb info          — list connected USB devices"
echo "   fatls usb 0:1     — list files on USB partition"
echo "   ping 192.168.2.1  — test TFTP server reachability"
echo "   run bootcmd       — retry boot sequence"
echo "============================================="