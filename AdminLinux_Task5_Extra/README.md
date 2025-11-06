### Usage

```bash
ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task5_Extra$ gcc -Wall mywrite.c -o mywrite
ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task5_Extra$ ./mywrite
Usage: ./mywrite <message>
ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task5_Extra$ ./mywrite Ehab
Ehab
ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task5_Extra$ ./mywrite Ehab Magdy
Ehab
ehab@HP:~/Documents/ITI_9Months/Linux/Embedded-Linux/AdminLinux_Task5_Extra$ ./mywrite "Ehab Magdy"
Ehab Magdy
```

### How the write syscall works

| Step | What happens                                                 |
| :--: | ------------------------------------------------------------ |
|  0️⃣   | Provide the input buffer (e.g., from command-line or a string in your program). |
|  1️⃣   | Fill CPU registers with the syscall number (`rax = 1`) and arguments (`rdi = fd`, `rsi = buf`, `rdx = len`). |
|  2️⃣   | The `syscall` instruction is executed → CPU switches from user mode to kernel mode. |
|  3️⃣   | The kernel dispatches the call to the `sys_write()` function. |
|  4️⃣   | The kernel writes your buffer to the file descriptor (e.g., `stdout` → terminal). |
|  5️⃣   | The kernel places the return value (number of bytes written or `-errno`) in `rax` and returns control to user mode. |
|  6️⃣   | You move that result from `rax` into your C variable.        |
|  7️⃣   | The message appears on your terminal                         |

### System Call Registers by Architecture

| Arch   | Syscall NR | Return | arg0 | arg1 | arg2 | arg3 | arg4 | arg5 |
| ------ | ---------- | ------ | ---- | ---- | ---- | ---- | ---- | ---- |
| ARM    | r7         | r0     | r0   | r1   | r2   | r3   | r4   | r5   |
| ARM64  | x8         | x0     | x0   | x1   | x2   | x3   | x4   | x5   |
| x86    | eax        | eax    | ebx  | ecx  | edx  | esi  | edi  | ebp  |
| x86_64 | rax        | rax    | rdi  | rsi  | rdx  | r10  | r8   | r9   |

### Linux x86_64 Syscall Table

| NR   | Syscall | %rax | arg0 (%rdi)          | arg1 (%rsi)     | arg2 (%rdx)  |
| :--- | ------- | ---- | -------------------- | --------------- | :----------- |
| 0    | read    | 0x00 | unsigned int fd      | char *buf       | size_t count |
| 1    | write   | 0x01 | unsigned int fd      | const char *buf | size_t count |
| 2    | open    | 0x02 | const char *filename | int flags       | umode_t mode |
