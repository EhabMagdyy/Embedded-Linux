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



### type

tells us how the command is interpreted by the shell, whether it’s a built-in, an alias, a function, or an external binary.

```shell
ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux$ type cd
cd is a shell builtin
```

```shell
ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux$ type ls
ls is aliased to `ls --color=auto'
```



### Two types of command file in /usr/bin that start with letter c

```shell
ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux$ ls /usr/bin/c*
/usr/bin/c++                     /usr/bin/cmuwmtopbm
```



### Using man command, find the command to read a file

To search within man pages

```shell
man -k read
```



### apropos command

searches the man page descriptions for keywords, helpful when you don’t remember a command’s name.

```shell
ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux$ apropos mv
git-mv (1)           - Move or rename a file, a directory, or a symlink
lvmvdo (7)           - Support for Virtual Data Optimizer in LVM
mv (1)               - move (rename) files
```



### Bash Script

##### Create a shell file:

```shell
touch LScript.sh
```

##### Script:

```bash
#!/bin/bash

mkdir -p ~/myDirectory

mkdir -p ~/myDirectory/secondDirectory

touch ~/myDirectory/secondDirectory/myNotePaper

cp ~/myDirectory/secondDirectory/myNotePaper ~/myDirectory/

mv ~/myDirectory/myNotePaper ~/myDirectory/myOldNotePaper

echo "Done :)"

```

##### Make it executable:

```shell
chmod +x LScript.sh
```

##### Run:

```shell
ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux$ ./LScript.sh 
Done :)
```

