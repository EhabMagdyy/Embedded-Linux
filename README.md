# Embedded Linux Task_1

### Some Linux Distributions

1. Ubuntu
2. Arch
3. Fedora
4. Kali
5. Debian

### man command

used to print the documentation of the external commands/binaries

### rmdir

remove empty directories

### rm

remove files or directories

### Copy /etc/passwd into mypasswd file in home directory

```shell
cp /etc/passwd ~/mypasswd
```



### Rename files

```shell
mv mypasswd oldpasswd
```



### Four ways to go to your home directory from /usr/bin

```shell
cd
cd ~
cd /home
cd ../../home
```



### List Linux commands in /usr/bin that start with letter w

```shell
ls /usr/bin/w*
```

output:

```
/usr/bin/w           /usr/bin/wget               /usr/bin/whoopsie-preferences
/usr/bin/w3m         /usr/bin/whatis             /usr/bin/winicontoppm
/usr/bin/w3mman      /usr/bin/whereis            /usr/bin/word-list-compress
/usr/bin/wall        /usr/bin/which              /usr/bin/wpa_passphrase
/usr/bin/watch       /usr/bin/which.debianutils  /usr/bin/write
/usr/bin/watchgnupg  /usr/bin/whiptail           /usr/bin/write.ul
/usr/bin/wbmptopbm   /usr/bin/who                /usr/bin/www-browser
/usr/bin/wc          /usr/bin/whoami
/usr/bin/wdctl       /usr/bin/whoopsie
```

