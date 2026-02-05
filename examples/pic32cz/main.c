#include <wolfHAL/wolfHAL.h>
#include "pic32cz_curiosity_ultra.h"

void main(void)
{
    whal_Error err;

    err = whal_Clock_Init(&clock);
    if (err) {
        goto loop;
    }

    err = whal_Gpio_Init(&gpio);
    if (err) {
        goto loop;
    }

    whal_Gpio_Set(&gpio, 0, 1);

loop:
    while (1);
}
