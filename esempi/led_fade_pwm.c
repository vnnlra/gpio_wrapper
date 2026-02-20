#include "gpio.h"
#include <unistd.h>

// LED su BCM17
#define LED 17

int main() {


    // range 100, parte spento
    gpio_init_pwm(LED, 0, 50);

   

    while (1) {
        // fade in
        for (int v = 0; v <= 50; v++) {
            gpio_write_pwm(LED, v);
            usleep(15000);
        }
        sleep(2);
        // fade out
        for (int v = 50; v >= 0; v--) {
            gpio_write_pwm(LED, v);
            usleep(15000);
        }
        
    }

    gpio_close();
    return 0;
}
