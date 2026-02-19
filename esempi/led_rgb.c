#include "gpio.h"
#include <unistd.h>

#define R 17 //bianco
#define G 27 //giallo
#define B 22 //blu

void set_rgb(int r, int g, int b){
    gpio_write(R, r);
    gpio_write(G, g);
    gpio_write(B, b);
} 

int main(){

    gpio_init_out(R, 0);
    gpio_init_out(G, 0);
    gpio_init_out(B, 0);

    while(1){
        
        set_rgb(1,0,0); sleep(1); // rosso
        set_rgb(0,1,0); sleep(1); // verde
        set_rgb(0,0,1); sleep(1); // blu
        set_rgb(1,1,0); sleep(1); // giallo
        set_rgb(1,1,1); sleep(1); // bianco
        set_rgb(0,0,0); sleep(1); // spento
                
    }

    gpio_close();
    return 0;
}