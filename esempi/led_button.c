#include "gpio.h"
#include <unistd.h>
#include <stdio.h>

#define LED 17
#define BTN 23

int main() {
    int res;
    gpio_init_out(LED, 0);
    gpio_init_in(BTN);

    while (1) {
        res = gpio_read(BTN);
        DEBUG:printf("btn = %d\n",res);
        if (res == 1){
            gpio_write(LED, 1);
        }
        else{
            gpio_write(LED, 0);
        }
        
        sleep(1);
    }

    gpio_close();
    return 0;
}
