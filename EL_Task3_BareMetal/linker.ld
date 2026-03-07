/* linker.ld — AArch64 bare-metal linker script for Raspberry Pi 3B+
 *
 * Load address 0x80000:
 *   - RPi3 firmware (start.elf) normally loads kernel8.img here and jumps to it
 *   - U-Boot also defaults to loading/running user payloads at this address
 *   - Must match the address you use in:  tftp 0x80000 / fatload ... 0x80000 / go 0x80000
 */

ENTRY(_start)

SECTIONS
{
    . = 0x80000;                    /* load + virtual address */

    .text :
    {
        KEEP(*(.text.boot))         /* startup.s MUST be first */
        *(.text)
        *(.text.*)
    }

    .rodata :
    {
        *(.rodata)
        *(.rodata.*)
    }

    .data :
    {
        *(.data)
        *(.data.*)
    }

    .bss (NOLOAD) :
    {
        . = ALIGN(8);
        __bss_start = .;            /* used in startup.s to zero BSS */
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(8);
        __bss_end = .;
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
    }
}
