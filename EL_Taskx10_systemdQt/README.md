# systemd Boot Design for Qt Application

https://github.com/user-attachments/assets/53ce0c1a-65c5-4e27-880c-2457947669fc

---

## Overview

- 1 static network configuration (`.network`)
- 1 loop-mounted filesystem image (`.mount`)
- 1 preparation service (`.service`)
- 1 Qt application service (`.service`)
- 1 custom target unit (`.target`) grouping all of the above

---

## File Structure

```
/etc/systemd/system/
├── mnt-appdata.mount        # Loop-mounts the filesystem image
├── qt-app-init.service      # Preparation service (oneshot)
├── qt-app.service           # Launches the Qt application
└── qt-app.target            # Groups all units into one target

/etc/systemd/network/
└── staticeth0.network       # Static IP configuration for eth0

/home/pi/
└── loopmount.img            # ext4 filesystem image (loop device source)

/mnt/appdata/                # Mount point for the loop image
```

---

## Unit Files

### 1. Network Configuration — `staticeth0.network`

Assigns a static IP to `eth0`. The gateway (`192.168.50.1`) is the connected laptop, which forwards traffic to the internet via NAT. DNS is set to Google's public resolver (`8.8.8.8`).

---

### 2. Mount Unit — `mnt-appdata.mount`

Mounts a local ext4 image file (`loopmount.img`) via a loop device to `/mnt/appdata`. The unit filename must match the mount point path — `mnt-appdata.mount` corresponds to `/mnt/appdata`.

---

### 3. Preparation Service — `qt-app-init.service`

A `oneshot` service that prepares the runtime environment before the Qt app launches. It creates `/tmp/qt-runtime/` and writes a status file. `RemainAfterExit=yes` keeps the service marked as **active** after its process exits, so downstream units that `Require=` it remain satisfied.

---

### 4. Qt Application Service — `qt-app.service`

Launches the Qt calculator application as the `pi` user. `DISPLAY=:0` is injected into the environment so the app can connect to the X11 display server. `RequiresMountsFor=/mnt/appdata` automatically adds both `Requires=` and `After=` dependencies for the mount, ensuring the loop image is mounted before this service starts.

---

### 5. Custom Target — `qt-app.target`

Groups the entire application stack under one synchronization point. Attached to `graphical.target` via `WantedBy=`, so the whole stack activates as part of the normal graphical boot flow.
##### Collective Control:
``` bash
sudo systemctl start qt-app.target    # starts all 3 units in correct order
sudo systemctl stop qt-app.target     # stops them all
systemctl list-dependencies qt-app.target  # sees the whole tree
```

---

## Dependency Workflow

The diagram below shows the full boot dependency chain, from hardware up to the Qt application:

```
graphical.target
└── qt-app.target
    ├── network-online.target          (network is fully configured and routable)
    ├── mnt-appdata.mount              (loop image is mounted at /mnt/appdata)
    ├── qt-app-init.service            (runtime dirs created, depends on mount + network)
    │   ├── After=mnt-appdata.mount
    │   └── Wants=network-online.target
    └── qt-app.service
        ├── After=qt-app-init.service
        ├── After=network-online.target
        └── RequiresMountsFor=/mnt/appdata
```

### Boot sequence in order

```
1. systemd-networkd applies staticeth0.network
        → eth0 gets IP 192.168.50.3/24

2. network-online.target becomes active
        → network is confirmed routable

3. mnt-appdata.mount activates
        → /home/pi/loopmount.img mounted at /mnt/appdata via loop device

4. qt-app-init.service runs (oneshot)
        → creates /tmp/qt-runtime/
        → writes "Init OK" to /tmp/qt-runtime/status.txt
        → remains active (RemainAfterExit=yes)

5. qt-app.service starts
        → DISPLAY=:0 injected into environment
        → /home/pi/appTask02_Calculator launched as user pi

6. qt-app.target becomes active
        → all units are up, stack is fully running
```

---

## Setup

### 1. Create the loop image

```bash
dd if=/dev/zero of=/home/pi/loopmount.img bs=1M count=10
sudo mkfs.ext4 /home/pi/loopmount.img
sudo mkdir -p /mnt/appdata
```

---

### 3. Enable systemd-networkd

```bash
sudo systemctl enable --now systemd-networkd
```

---

### 3. Enable all units

```bash
sudo systemctl daemon-reload

sudo systemctl enable mnt-appdata.mount
sudo systemctl enable qt-app-init.service
sudo systemctl enable qt-app.service
sudo systemctl enable qt-app.target
```

---

### 4. Start and verify without rebooting

```bash
sudo systemctl start qt-app.target

systemctl status mnt-appdata.mount
systemctl status qt-app-init.service
systemctl status qt-app.service
systemctl status qt-app.target
```

### 5. Verify the dependency chain

```bash
systemctl list-dependencies qt-app.target
```

Expected output:

```
qt-app.target
● ├─mnt-appdata.mount
● ├─qt-app-init.service
● ├─qt-app.service
● └─network-online.target
●   ├─NetworkManager-wait-online.service
●   └─systemd-networkd-wait-online.service
```

---

### 6. Fix RPi DNS & Network Setup (Laptop as Gateway) if needed

### On the RPi
```bash
# make sure routing & dns works
cat /etc/resolv.conf
# should show: nameserver 8.8.8.8
```

### On the laptop
```bash
# Enable IP forwarding
sudo sysctl -w net.ipv4.ip_forward=1

# Make sure wifi interface and nat is the same:
# Check:
ifconfig
sudo iptables -t nat -L POSTROUTING -v

# my interface is wlo1 but NAT is wlan0
# Removing the wrong wlan0
sudo iptables -t nat -D POSTROUTING -o wlan0 -j MASQUERADE
# Add the correct wlo1
sudo iptables -t nat -A POSTROUTING -o wlo1 -j MASQUERADE
# Verify
sudo iptables -t nat -L POSTROUTING -v
```

### Verify on RPi

```bash
# From the Pi:
ping 1.1.1.1       # tests routing through laptop
ping google.com    # tests DNS resolution
```
