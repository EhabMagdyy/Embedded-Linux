#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Invalid, Correct usage: %s <0|1>\n", argv[0]);
        return 1;
    }

    if (argv[1][0] != '0' && argv[1][0] != '1') {
        printf("Invalid value. Please enter 0 or 1.\n");
        return 1;
    }

    FILE *fp = fopen("/sys/class/leds/input3::capslock/brightness", "w");
    if (NULL == fp) {
        perror("Error opening brightness file");
        return 1;
    }

    fprintf(fp, "%c", argv[1][0]);
    fclose(fp);

    return 0;
}