#include <stdint.h>

#define GPIO_BASE       0x3F200000UL

#define GPFSEL2_OFFSET     0x08    // Function select for GPIO 20:29
#define GPSET0_OFFSET      0x1C    // Pin output set  (GPIO 0:31)
#define GPCLR0_OFFSET      0x28    // Pin output clear (GPIO 0:31)

#define REG(base, offset)  (*((volatile uint32_t *)((base) + (offset))))

#define GPIO26_FSEL_SHIFT   18
#define GPIO26_FSEL_MASK    (0x7U << GPIO26_FSEL_SHIFT)
#define GPIO26_FSEL_OUT     (0x1U << GPIO26_FSEL_SHIFT)

#define GPIO26_BIT          (1U << 26)

static void delay(uint32_t count){
    volatile uint32_t i = count;
    while (i--);
}

void main(void)
{
    // 1. Configure GPIO 26 as output
    uint32_t sel = REG(GPIO_BASE, GPFSEL2_OFFSET);
    sel &= ~GPIO26_FSEL_MASK;
    sel |=  GPIO26_FSEL_OUT;
    REG(GPIO_BASE, GPFSEL2_OFFSET) = sel;

    // 2. Blink forever
    while (1) {
        // LED ON
        REG(GPIO_BASE, GPSET0_OFFSET) = GPIO26_BIT;
        delay(50000000);
        // LED OFF
        REG(GPIO_BASE, GPCLR0_OFFSET) = GPIO26_BIT;
        delay(50000000);
    }
}