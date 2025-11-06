#include <string.h>

long mywrite(int fd, const void *buf, unsigned long len){
    long ret;

    asm volatile(
        "mov $1, %%rax\n\t"        /* syscall number: sys_write = 1 */
        "mov %1, %%rdi\n\t"        /* arg0: fd */
        "mov %2, %%rsi\n\t"        /* arg1: buf */
        "mov %3, %%rdx\n\t"        /* arg2: len */
        "syscall\n\t"              /* invoke kernel */
        "mov %%rax, %0\n\t"        /* store return value in ret */
        : "=r" (ret)               /* output operand %0 */
        : "r" ((long)fd), "r" (buf), "r" (len)  /* input operands %1, %2, %3 */
        : "rax", "rdi", "rsi", "rdx", "rcx", "r11", "memory" /* clobbered regs */
    );

    return ret;
}

int main(int argc, char *argv[]){
    if(argc < 2){
        const char msg[] = "Usage: ./mywrite <message>\n";
        mywrite(2, msg, sizeof(msg) - 1); // write to stderr
        return 1;
    }

    const char *buf = argv[1];
    unsigned long len = strlen(buf);
    
    long ret = mywrite(1, buf, len);    // write to stdout
    mywrite(1, "\n", 1);

    if(ret < 0){
        const char err[] = "Error: write syscall failed\n";
        mywrite(2, err, sizeof(err) - 1);
        return 1;
    }

    return 0;
}
