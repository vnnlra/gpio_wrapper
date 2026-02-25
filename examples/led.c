#include "gpio.h"
#include <unistd.h>

int main() {
    gpio_init_out(17, 0);
    while (1) {
        gpio_write(17, 1);
        sleep(1);
        gpio_write(17, 0);
        sleep(1);
    }
    gpio_close();
}
