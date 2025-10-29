### Question 1

```shell
ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task2$ vim binary_search.c
```

```bash
ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task2$ cat binary_search.c 
#include <stdio.h>

int binarySearch(int* arr, int n, int targetIdx){
	int l = 0, r = n-1;
	while(l <= r){
		int mid = l + (r - l) / 2;
		if(targetIdx == arr[mid]){
			return mid;
		}
		else if(arr[mid] < targetIdx){
			l = mid + 1;
		}
		else
			r = mid - 1;
	}
	return -1;
}

int main(){
	int arr[] = {2,3,5,6,7,9,13,23};
	printf("Target Index: %d", binarySearch(arr, sizeof(arr)/sizeof(arr[0]), 7));
	return 0;
}
```

```bash
ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task2$ gcc binary_search.c 
ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task2$ ./a.out
Target Index: 4
```



### Question 2

```bash
ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task2$ sudo mv a.out /usr/local/bin
[sudo] password for ehab: 
ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task2$ cd
ehab@HP:~$ a.out
Target Index: 4
```

##### it works because /usr/local/bin is in the PATH:

```bash
ehab@HP:~$ printenv PATH
/home/ehab/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/snap/bin:/snap/bin:/snap/bin
```



### Na2na2a

1. ##### Available shells in my system

   ```
   ehab@HP:~$ cat /etc/shells
   
   # /etc/shells: valid login shells
   
   /bin/sh
   /bin/bash
   /usr/bin/bash
   /bin/rbash
   /usr/bin/rbash
   /usr/bin/sh
   /bin/dash
   /usr/bin/dash
   /usr/bin/tmux
   ```

2. ##### List the environment variables in your current shell

   ```bash
   ehab@HP:~$ printenv
   SHELL=/bin/bash
   SESSION_MANAGER=local/HP:@/tmp/.ICE-unix/2263,unix/HP:/tmp/.ICE-unix/2263
   QT_ACCESSIBILITY=1
   COLORTERM=truecolor
   XDG_CONFIG_DIRS=/etc/xdg/xdg-ubuntu:/etc/xdg
   SSH_AGENT_LAUNCHER=gnome-keyring
   XDG_MENU_PREFIX=gnome-
   GNOME_DESKTOP_SESSION_ID=this-is-deprecated
   TERMINATOR_DBUS_PATH=/net/tenshu/Terminator2
   LC_ADDRESS=en_GB.UTF-8
   GNOME_SHELL_SESSION_MODE=ubuntu
   LC_NAME=en_GB.UTF-8
   SSH_AUTH_SOCK=/run/user/1000/keyring/ssh
   XMODIFIERS=@im=ibus
   DESKTOP_SESSION=ubuntu
   LC_MONETARY=en_GB.UTF-8
   GTK_MODULES=gail:atk-bridge
   PWD=/home/ehab
   XDG_SESSION_DESKTOP=ubuntu
   LOGNAME=ehab
   XDG_SESSION_TYPE=wayland
   SYSTEMD_EXEC_PID=2296
   XAUTHORITY=/run/user/1000/.mutter-Xwaylandauth.0ZDCF3
   GJS_DEBUG_TOPICS=JS ERROR;JS LOG
   HOME=/home/ehab
   USERNAME=ehab
   IM_CONFIG_PHASE=1
   LANG=en_US.UTF-8
   LC_PAPER=en_GB.UTF-8
   XDG_CURRENT_DESKTOP=ubuntu:GNOME
   VTE_VERSION=6800
   WAYLAND_DISPLAY=wayland-0
   MANAGERPID=2104
   GJS_DEBUG_OUTPUT=stderr
   GNOME_SETUP_DISPLAY=:1
   LESSCLOSE=/usr/bin/lesspipe %s %s
   XDG_SESSION_CLASS=user
   IBUS_DISABLE_SNOOPER=1
   LC_IDENTIFICATION=en_GB.UTF-8
   TERM=xterm-256color
   LESSOPEN=| /usr/bin/lesspipe %s
   USER=ehab
   DISPLAY=:0
   SHLVL=2
   LC_TELEPHONE=en_GB.UTF-8
   QT_IM_MODULE=ibus
   LC_MEASUREMENT=en_GB.UTF-8
   PAPERSIZE=a4
   XDG_RUNTIME_DIR=/run/user/1000
   LC_TIME=en_GB.UTF-8
   JOURNAL_STREAM=8:10889
   XDG_DATA_DIRS=/usr/share/ubuntu:/home/ehab/.local/share/flatpak/exports/share:/var/lib/flatpak/exports/share:/usr/local/share/:/usr/share/:/var/lib/snapd/desktop
   PATH=/home/ehab/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/snap/bin:/snap/bin:/snap/bin
   GDMSESSION=ubuntu
   DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/1000/bus
   GIO_LAUNCHED_DESKTOP_FILE_PID=6264
   GIO_LAUNCHED_DESKTOP_FILE=/usr/share/applications/terminator.desktop
   LC_NUMERIC=en_GB.UTF-8
   OLDPWD=/home/ehab/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task2
   _=/usr/bin/printenv
   ```

3. ##### My current shell name

   ```bash
   ehab@HP:~$ echo $SHELL
   /bin/bash
   ```

4. ##### \ -> let us write commands on multiple lines

   ```
   ehab@HP:~$ echo hello \
   > ehab \
   > !
   hello ehab !
   ```

5. Creating alias

   ```bash
   ehab@HP:~$ alias PrintBash='echo $PATH'
   ehab@HP:~$ PrintBash
   /home/ehab/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/snap/bin:/snap/bin:/snap/bin
   ```