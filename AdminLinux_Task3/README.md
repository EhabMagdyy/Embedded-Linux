## Section 1

##### Bash Script

```bash
#!/bin/bash

if [ -f ~/.bashrc ]; then
    echo "You have a .bashrc file."

    grep -qxF "export HELLO=${HOSTNAME}" ~/.bashrc || echo "export HELLO=$HOSTNAME" >> ~/.bashrc
    LOCAL=$(whoami)
    grep -qxF "LOCAL=$LOCAL" ~/.bashrc || echo "LOCAL=$LOCAL" >> ~/.bashrc
    gnome-terminal -- bash -ic "echo -e 'HELLO = $HELLO\nLOCAL = $LOCAL'; exec bash"

else
    echo "You do not have a .bashrc file."
fi

```

```bash
if [ -f ~/.bashrc ]; then
```

> Checks if `.bashrc` file exists under `/home`

```bash
grep -qxF "export HELLO=${HOSTNAME}" ~/.bashrc || echo "export HELLO=$HOSTNAME" >> ~/.bashrc
```

> Checks if `.bashrc` does NOT already contain `export HELLO=<hostname>`, then add it.

```bash
LOCAL=$(whoami)
grep -qxF "LOCAL=$LOCAL" ~/.bashrc || echo "LOCAL=$LOCAL" >> ~/.bashrc
```

> 1. Create a Local Variable `LOCAL` with the value of `whoami` command
> 2. Checks `.bashrc` does NOT already contain `LOCAL=$LOCAL`, then add it.

```bash
gnome-terminal -- bash -ic "echo -e 'HELLO = $HELLO\nLOCAL = $LOCAL'; exec bash"
```

> 1. It opens a new gnome terminal, runs a bash shell interactively.
> 2. executes a command to print the values of `HELLO` and `LOCAL`.
> 3. then keeps the terminal open.

##### What happened when the terminal is opened?

it prints the value of the global variable `HELLO` and also the value of the local variable `LOCAL`, because it's in the `.bashrc` file so the new bash shell did not inherit them, it reload them.

> Testing the visibility of the variables using another shell `dash`

```bash
ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ sh
$ echo $HELLO
HP
$ echo $LOCAL

$ exit
```

> 1. `HELLO` was exported -> inherited by `sh`
>
> 2. `LOCAL` was only a local variable in bash -> not inherited by `sh`



## Section 2

1. ```bash
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ ls /usr/bin > /tmp/commands
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ cat /tmp/commands | head -n 10
   [
   411toppm
   aa-enabled
   aa-exec
   aa-features-abi
   aconnect
   acpidbg
   acpi_listen
   add-apt-repository
   addpart
   ```

2. ```bash
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ vim ~/.profile 
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ cat ~/.profile | tail -n 4 
   # Show date & time at login
   echo "Login Date & Time: $(date)"
   # Make it permenant
   export PS1="\u@\h:\w$ "
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ source ~/.profile 
   Login Date & Time: Tue  4 Nov 14:40:30 EET 2025
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ echo "Done:)"
   Done:)
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ exit
   exit
   ```

3. ```bash
   # wc command: count number of lines/words
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ ls -1 /usr/bin/ | wc -l
   1947
   ```

4. a) `cat filename2` ignores the piped input and only prints contents of `filename2`.

   b) 

   ```bash
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ ls | rm
   rm: missing operand
   Try 'rm --help' for more information.
   ```

   c) `/etc/passwd` is a file, not a directory. so ls will print the file name `/etc/passwd`, and `wc -l` will count it as one line -> so the output is `1`

5. ```bash
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ find / -name ".profile" 2>/tmp/null
   /snap/core24/1225/etc/skel/.profile
   /snap/core24/1196/etc/skel/.profile
   /snap/core22/2139/etc/skel/.profile
   /snap/core22/1122/etc/skel/.profile
   /snap/core20/2669/etc/skel/.profile
   /etc/skel/.profile
   /home/ehab/.profile
   ```

6. ```bash
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ ls -i /
         12 bin    54263809 home          11 lost+found   3932161 root         1 sys
   52690945 boot         13 lib     48758785 media              1 run   11796481 tmp
    9437185 cdrom        14 lib32   52953089 mnt               17 sbin   2621441 usr
          1 dev          15 lib64   30932993 opt         14417921 snap  43778049 var
   24379393 etc          16 libx32         1 proc         4718593 srv
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ ls -i /etc
   24379400 acpi                           24380667 lsb-release
   24379530 adduser.conf                   24379576 machine-id
   24379401 alsa                           24379577 magic
   24379402 alternatives                   24379578 magic.mime
   24379531 anacrontab                     24379538 mailcap
   24510468 apache2                        24379580 mailcap.order
   24379532 apg.conf                       24379581 manpath.config
   24379403 apm                            24379582 mime.types
   24379404 apparmor                       24510540 minicom
   24379405 apparmor.d                     24379583 mke2fs.conf
   24379406 apport                         24379395 ModemManager
   24379533 appstream.conf                 24379468 modprobe.d
   24379407 apt                            24379584 modules
   24379408 avahi                          24379469 modules-load.d
   24379537 avrdude.conf                   24379579 mtab
   24379534 bash.bashrc                    24379586 nanorc
   24379535 bash_completion                24379587 netconfig
   24379409 bash_completion.d              24379470 netplan
   24379536 bindresvport.blacklist         24379471 network
   24379410 binfmt.d                       24379472 networkd-dispatcher
   24379411 bluetooth                      24379396 NetworkManager
   24382331 bmon.conf                      24379588 networks
   24510537 byobu                          24379473 newt
   24379413 ca-certificates                24379589 nftables.conf
   24379458 ca-certificates.conf           24379423 nsswitch.conf
   24379539 ca-certificates.conf.dpkg-old  24510466 openal
   24382335 cczerc                         24379474 openvpn
   24379414 chatscripts                    24379475 opt
   24510498 cni                            24380753 os-release
   24379416 console-setup                  24379397 PackageKit
   24379417 cracklib                       24379592 pam.conf
   24379418 cron.d                         24379476 pam.d
   24379419 cron.daily                     24379593 papersize
   24379420 cron.hourly                    24379607 passwd
   24379421 cron.monthly                   24382373 passwd-
   24379541 crontab                        24379477 pcmcia
   24379422 cron.weekly                    24379478 perl
   24379424 cups                           24379479 pki
   24379425 cupshelpers                    24379480 pm
   24379426 dbus-1                         24379595 pnm2ppa.conf
   24379427 dconf                          24379481 polkit-1
   24379542 debconf.conf                   24379482 ppp
   24379543 debian_version                 24379562 printcap
   24379428 default                        24379596 profile
   24379544 deluser.conf                   24379483 profile.d
   24379429 depmod.d                       24379597 protocols
   24379430 dhcp                           24379484 pulse
   24379431 dictionaries-common            24379485 python3
   24510493 dnsmasq.d                      24379486 python3.10
   24510492 docker                         24379487 rc0.d
   24379432 dpkg                           24379488 rc1.d
   24379545 e2scrub.conf                   24379489 rc2.d
   24379433 emacs                          24379490 rc3.d
   24379546 environment                    24379491 rc4.d
   24379434 environment.d                  24379492 rc5.d
   24379547 ethertypes                     24379493 rc6.d
   24379435 firefox                        24379494 rcS.d
   24379436 fonts                          24379598 resolv.conf
   24379548 fprintd.conf                   24379599 rmt
   24379394 fstab                          24379600 rpc
   24379549 fuse.conf                      24379601 rsyslog.conf
   24379437 fwupd                          24379496 rsyslog.d
   24379550 gai.conf                       24379602 rygel.conf
   24379438 gdb                            24379497 sane.d
   24379439 gdm3                           24379498 security
   24379440 geoclue                        24379499 selinux
   24379441 ghostscript                    24379603 sensors3.conf
   24379442 glvnd                          24379500 sensors.d
   24379443 gnome                          24379604 services
   24379444 groff                          24379501 sgml
   24382366 group                          24380121 shadow
   24379552 group-                         24379594 shadow-
   24379445 grub.d                         24380138 shells
   24382361 gshadow                        24379502 skel
   24382374 gshadow-                       24379503 snmp
   24379446 gss                            24379504 speech-dispatcher
   24379447 gtk-2.0                        24379505 ssh
   24379448 gtk-3.0                        24379506 ssl
   24379553 hdparm.conf                    24382384 subgid
   24379554 host.conf                      24379608 subgid-
   24379555 hostid                         24382383 subuid
   24379556 hostname                       24382378 subuid-
   24379557 hosts                          24379609 sudo.conf
   24379558 hosts.allow                    24379611 sudoers
   24379559 hosts.deny                     24379508 sudoers.d
   24379449 hp                             24379610 sudo_logsrvd.conf
   24379450 ifplugd                        24379612 sysctl.conf
   24510476 ImageMagick-6                  24379509 sysctl.d
   24379451 init                           24379510 systemd
   24379452 init.d                         24379511 terminfo
   24379453 initramfs-tools                24379512 thermald
   24379560 inputrc                        24379513 thunderbird
   24379454 insserv.conf.d                 24379613 timezone
   24379455 ipp-usb                        24379514 tmpfiles.d
   24379456 iproute2                       24379515 ubuntu-advantage
   24380107 issue                          24379614 ucf.conf
   24380608 issue.net                      24379516 udev
   24510501 java-11-openjdk                24379517 udisks2
   24379457 kernel                         24379518 ufw
   24380723 kernel-img.conf                24382334 updatedb.conf
   24379564 kerneloops.conf                24379519 update-manager
   24379460 ldap                           24379520 update-motd.d
   24380131 ld.so.cache                    24379521 update-notifier
   24379566 ld.so.conf                     24379398 UPower
   24379459 ld.so.conf.d                   24379615 usb_modeswitch.conf
   24379567 legal                          24379522 usb_modeswitch.d
   24379568 libao.conf                     24379591 vdpau_wrapper.cfg
   24379569 libaudit.conf                  24379523 vim
   24379461 libblockdev                    24379616 vtrgb
   24379462 libnl-3                        24379524 vulkan
   24379463 libpaper.d                     24510495 w3m
   24379464 libreoffice                    24379617 wgetrc
   24510471 lighttpd                       24379525 wpa_supplicant
   24379570 locale.alias                   24379399 X11
   24379495 locale.gen                     24379618 xattr.conf
   24381146 localtime                      24379526 xdg
   24379465 logcheck                       24379527 xml
   24379573 login.defs                     24510541 zsh
   24379574 logrotate.conf                 24379619 zsh_command_not_found
   24379466 logrotate.d
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ ls -i /etc/hosts
   24379557 /etc/hosts
   ```

7. ```bash
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ sudo ln -s /etc/passwd /boot/linkPassed
   [sudo] password for ehab: 
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ ls -la /boot/linkPassed 
   lrwxrwxrwx 1 root root 11 Nov  4 14:58 /boot/linkPassed -> /etc/passwd
   ```

8. ```bash
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ sudo ln /etc/passwd /boot/hardlinkPassed
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ ls -i /etc/passwd /boot/hardlinkPassed 
   24379607 /boot/hardlinkPassed  24379607 /etc/passwd
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ df / /boot
   Filesystem     1K-blocks     Used Available Use% Mounted on
   /dev/sda1      960302096 26779620 884668052   3% /
   /dev/sda1      960302096 26779620 884668052   3% /
   # Hard link worked, because both /boot and / are in the same filesystem.
   ```

9. ```bash
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ echo \
   > Hi
   Hi
   # `\` is a line continuation
   # `>` it's the secondary prompt, controlled by environment variable $PS2
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ echo $PS2
   >
   ```

   > Change prompt from `>` to `:`

   ```bash
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ export PS2=": "
   ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task3$ echo \
   : Hi
   Hi
   ```

   