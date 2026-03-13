/*
 * =============================================================================
 * main.c — Minimal Init + Shell for Embedded Linux (RPi3B+)
 * =============================================================================
 *
 * This program runs as PID 1 (init). It:
 *   1. Mounts essential virtual filesystems (proc, sys, devtmpfs)
 *   2. Prints a welcome banner
 *   3. Drops into a minimal interactive shell
 *
 * SUPPORTED BUILT-IN COMMANDS:
 *   ls   [path]        — list directory contents
 *   cat  <file>        — print file contents
 *   echo <text>        — print text
 *   pwd                — print working directory
 *   cd   <path>        — change directory
 *   mkdir <path>       — create directory
 *   clear              — clear screen
 *   uname              — print kernel version
 *   free               — print memory info (reads /proc/meminfo)
 *   ps                 — list processes (reads /proc)
 *   help               — list all commands
 *   exit / reboot      — reboot the system
 *
 * CROSS COMPILE:
 *   aarch64-linux-gnu-gcc -static -o init main.c
 *
 * DEPLOY:
 *   sudo cp init /mnt/sda2/sbin/init
 *   sudo chmod +x /mnt/sda2/sbin/init
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

/* ── tunables ────────────────────────────────────────────────────── */
#define SHELL_NAME   "EhabShell"
#define VERSION      "1.0"
#define PROMPT       "\033[1;32mEhab@RPi3B+\033[0m:\033[1;34m%s\033[0m$ "
#define MAX_INPUT    256
#define MAX_ARGS     32
/* ─────────────────────────────────────────────────────────────────── */

/* ── forward declarations ────────────────────────────────────────── */
static void mount_filesystems(void);
static void print_banner(void);
static void run_shell(void);
static int  parse_args(char *line, char **argv);

static void cmd_ls(int argc, char **argv);
static void cmd_cat(int argc, char **argv);
static void cmd_echo(int argc, char **argv);
static void cmd_pwd(void);
static void cmd_cd(int argc, char **argv);
static void cmd_mkdir(int argc, char **argv);
static void cmd_clear(void);
static void cmd_uname(void);
static void cmd_free(void);
static void cmd_ps(void);
static void cmd_help(void);
/* ─────────────────────────────────────────────────────────────────── */


/* ═══════════════════════════════════════════════════════════════════
 * ENTRY POINT — PID 1
 * ═══════════════════════════════════════════════════════════════════ */
int main(void)
{
    mount_filesystems();
    print_banner();
    run_shell();

    /* Should never reach here — if we do, kernel will panic.
     * Do a clean reboot instead.                              */
    fprintf(stderr, "\n[init] Shell exited — rebooting...\n");
    sync();
    reboot(RB_AUTOBOOT);
    return 0;
}


/* ═══════════════════════════════════════════════════════════════════
 * MOUNT ESSENTIAL VIRTUAL FILESYSTEMS
 * ═══════════════════════════════════════════════════════════════════ */
static void mount_filesystems(void)
{
    /* Create mount points if they don't exist */
    mkdir("/proc", 0755);
    mkdir("/sys",  0755);
    mkdir("/dev",  0755);
    mkdir("/tmp",  0777);

    if (mount("proc", "/proc", "proc", 0, NULL) < 0)
        fprintf(stderr, "[init] Warning: could not mount /proc: %s\n", strerror(errno));

    if (mount("sysfs", "/sys", "sysfs", 0, NULL) < 0)
        fprintf(stderr, "[init] Warning: could not mount /sys: %s\n", strerror(errno));

    /* devtmpfs — populated by kernel with device nodes */
    if (mount("devtmpfs", "/dev", "devtmpfs", 0, NULL) < 0)
        fprintf(stderr, "[init] Warning: could not mount /dev: %s\n", strerror(errno));

    if (mount("tmpfs", "/tmp", "tmpfs", 0, "size=16m") < 0)
        fprintf(stderr, "[init] Warning: could not mount /tmp: %s\n", strerror(errno));
}


/* ═══════════════════════════════════════════════════════════════════
 * WELCOME BANNER
 * ═══════════════════════════════════════════════════════════════════ */
static void print_banner(void)
{
    /* Read kernel version from /proc/version */
    char kver[128] = "unknown";
    FILE *f = fopen("/proc/version", "r");
    if (f) {
        fgets(kver, sizeof(kver), f);
        fclose(f);
        /* Trim at newline */
        char *nl = strchr(kver, '\n');
        if (nl) *nl = '\0';
    }

    printf("\n");
    printf("  \033[1;36m╔══════════════════════════════════════════╗\033[0m\n");
    printf("  \033[1;36m║  %-40s  ║\033[0m\n", SHELL_NAME " v" VERSION);
    printf("  \033[1;36m║  Raspberry Pi 3B+ — Custom Init Shell    ║\033[0m\n");
    printf("  \033[1;36m╚══════════════════════════════════════════╝\033[0m\n");
    printf("\n");
    printf("  Kernel  : %s\n", kver);
    printf("  PID     : %d (I am init)\n", getpid());
    printf("\n");
    printf("  Type \033[1mhelp\033[0m to list available commands.\n");
    printf("\n");
}


/* ═══════════════════════════════════════════════════════════════════
 * MAIN SHELL LOOP
 * ═══════════════════════════════════════════════════════════════════ */
static void run_shell(void)
{
    char  line[MAX_INPUT];
    char *argv[MAX_ARGS];
    char  cwd[256];

    /* ── startup commands ── */
    const char *startup[] = {
        "echo Hello from Ehab's Kernel!",
        NULL
    };

    for (int i = 0; startup[i] != NULL; i++) {
        /* print as if the user typed it */
        printf(PROMPT, "/");
        printf("%s\n", startup[i]);

        /* execute it */
        char tmp[MAX_INPUT];
        strncpy(tmp, startup[i], MAX_INPUT);
        char *args[MAX_ARGS];
        int argc = parse_args(tmp, args);
        if (argc > 0) {
            if      (strcmp(args[0], "echo") == 0) cmd_echo(argc, args);
        }
    }

    while (1) {

        /* Print prompt with current directory */
        if (getcwd(cwd, sizeof(cwd)) == NULL)
            strncpy(cwd, "?", sizeof(cwd));
        printf(PROMPT, cwd);
        fflush(stdout);

        /* Read input */
        if (fgets(line, sizeof(line), stdin) == NULL) {
            /* EOF (Ctrl-D) */
            printf("\n");
            break;
        }

        /* Strip trailing newline */
        line[strcspn(line, "\n")] = '\0';

        /* Skip empty lines */
        if (strlen(line) == 0)
            continue;

        /* Parse into argv */
        int argc = parse_args(line, argv);
        if (argc == 0)
            continue;

        /* Dispatch commands */
        if      (strcmp(argv[0], "ls")    == 0) cmd_ls(argc, argv);
        else if (strcmp(argv[0], "cat")   == 0) cmd_cat(argc, argv);
        else if (strcmp(argv[0], "echo")  == 0) cmd_echo(argc, argv);
        else if (strcmp(argv[0], "pwd")   == 0) cmd_pwd();
        else if (strcmp(argv[0], "cd")    == 0) cmd_cd(argc, argv);
        else if (strcmp(argv[0], "mkdir") == 0) cmd_mkdir(argc, argv);
        else if (strcmp(argv[0], "clear") == 0) cmd_clear();
        else if (strcmp(argv[0], "uname") == 0) cmd_uname();
        else if (strcmp(argv[0], "free")  == 0) cmd_free();
        else if (strcmp(argv[0], "ps")    == 0) cmd_ps();
        else if (strcmp(argv[0], "help")  == 0) cmd_help();
        else if (strcmp(argv[0], "exit")  == 0 ||
                 strcmp(argv[0], "reboot")== 0) {
            printf("[init] Syncing and rebooting...\n");
            sync();
            reboot(RB_AUTOBOOT);
        }
        else {
            printf("%s: command not found. Type 'help' for a list.\n", argv[0]);
        }
    }
}


/* ═══════════════════════════════════════════════════════════════════
 * ARGUMENT PARSER  (splits on spaces)
 * ═══════════════════════════════════════════════════════════════════ */
static int parse_args(char *line, char **argv)
{
    int argc = 0;
    char *token = strtok(line, " \t");
    while (token && argc < MAX_ARGS - 1) {
        argv[argc++] = token;
        token = strtok(NULL, " \t");
    }
    argv[argc] = NULL;
    return argc;
}


/* ═══════════════════════════════════════════════════════════════════
 * BUILT-IN COMMANDS
 * ═══════════════════════════════════════════════════════════════════ */

/* ls [path] */
static void cmd_ls(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1] : ".";
    DIR *dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "ls: cannot open '%s': %s\n", path, strerror(errno));
        return;
    }
    struct dirent *entry;
    int col = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.')    /* hide dot files */
            continue;
        printf("%-20s", entry->d_name);
        if (++col % 4 == 0) printf("\n");
    }
    if (col % 4 != 0) printf("\n");
    closedir(dir);
}

/* cat <file> */
static void cmd_cat(int argc, char **argv)
{
    if (argc < 2) { fprintf(stderr, "cat: missing filename\n"); return; }
    FILE *f = fopen(argv[1], "r");
    if (!f) { fprintf(stderr, "cat: %s: %s\n", argv[1], strerror(errno)); return; }
    char buf[512];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0)
        fwrite(buf, 1, n, stdout);
    fclose(f);
}

/* echo <text...> */
static void cmd_echo(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        printf("%s", argv[i]);
        if (i < argc - 1) printf(" ");
    }
    printf("\n");
}

/* pwd */
static void cmd_pwd(void)
{
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)))
        printf("%s\n", cwd);
    else
        fprintf(stderr, "pwd: %s\n", strerror(errno));
}

/* cd <path> */
static void cmd_cd(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1] : "/";
    if (chdir(path) < 0)
        fprintf(stderr, "cd: %s: %s\n", path, strerror(errno));
}

/* mkdir <path> */
static void cmd_mkdir(int argc, char **argv)
{
    if (argc < 2) { fprintf(stderr, "mkdir: missing path\n"); return; }
    if (mkdir(argv[1], 0755) < 0)
        fprintf(stderr, "mkdir: %s: %s\n", argv[1], strerror(errno));
}

/* clear */
static void cmd_clear(void)
{
    printf("\033[2J\033[H");
    fflush(stdout);
}

/* uname */
static void cmd_uname(void)
{
    FILE *f = fopen("/proc/version", "r");
    if (!f) { fprintf(stderr, "uname: cannot read /proc/version\n"); return; }
    char buf[256];
    if (fgets(buf, sizeof(buf), f))
        printf("%s", buf);
    fclose(f);
}

/* free — reads /proc/meminfo */
static void cmd_free(void)
{
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) { fprintf(stderr, "free: cannot read /proc/meminfo\n"); return; }
    char line[128];
    int printed = 0;
    while (fgets(line, sizeof(line), f) && printed < 5) {
        printf("%s", line);
        printed++;
    }
    fclose(f);
}

/* ps — scans /proc for numeric directories */
static void cmd_ps(void)
{
    DIR *dir = opendir("/proc");
    if (!dir) { fprintf(stderr, "ps: cannot open /proc\n"); return; }
    printf("  %-8s  %s\n", "PID", "Name");
    printf("  %-8s  %s\n", "---", "----");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        /* only numeric entries are PIDs */
        if (entry->d_name[0] < '1' || entry->d_name[0] > '9')
            continue;
        char comm_path[64];
        snprintf(comm_path, sizeof(comm_path), "/proc/%s/comm", entry->d_name);
        FILE *f = fopen(comm_path, "r");
        char comm[64] = "?";
        if (f) {
            if (fgets(comm, sizeof(comm), f))
                comm[strcspn(comm, "\n")] = '\0';
            fclose(f);
        }
        printf("  %-8s  %s\n", entry->d_name, comm);
    }
    closedir(dir);
}

/* help */
static void cmd_help(void)
{
    printf("\n");
    printf("  \033[1mAvailable commands:\033[0m\n\n");
    printf("  %-20s %s\n", "ls [path]",    "List directory contents");
    printf("  %-20s %s\n", "cat <file>",   "Print file to stdout");
    printf("  %-20s %s\n", "echo <text>",  "Print text");
    printf("  %-20s %s\n", "pwd",          "Print working directory");
    printf("  %-20s %s\n", "cd <path>",    "Change directory");
    printf("  %-20s %s\n", "mkdir <path>", "Create directory");
    printf("  %-20s %s\n", "clear",        "Clear screen");
    printf("  %-20s %s\n", "uname",        "Print kernel version");
    printf("  %-20s %s\n", "free",         "Print memory info");
    printf("  %-20s %s\n", "ps",           "List running processes");
    printf("  %-20s %s\n", "help",         "Show this message");
    printf("  %-20s %s\n", "exit / reboot","Reboot the system");
    printf("\n");
}