#include "encoder.h"
#include "gpio.h"
#include <unistd.h>

static unsigned int PIN_CLK, PIN_DT, PIN_SW;
static int last_clk = 0;
static int last_sw  = 1; // spesso pull-up -> non premuto = 1

int encoder_init(unsigned int clk, unsigned int dt, unsigned int sw) {
    PIN_CLK = clk;
    PIN_DT  = dt;
    PIN_SW  = sw;

    if (gpio_init_in(PIN_CLK) < 0) return -1;
    if (gpio_init_in(PIN_DT)  < 0) return -1;
    if (gpio_init_in(PIN_SW)  < 0) return -1;

    last_clk = gpio_read(PIN_CLK);
    last_sw  = gpio_read(PIN_SW);
    return 0;
}

int encoder_step(void) {
    int clk = gpio_read(PIN_CLK);
    if (clk < 0) return 0;

    // Consideriamo un "click" quando CLK passa da 0->1
    if (last_clk == 0 && clk == 1) {
        int dt = gpio_read(PIN_DT);
        if (dt < 0) return 0;

        // Se DT è diverso da CLK, una direzione; altrimenti l’altra
        // (con molti encoder: dt==0 quando clk sale => una direzione)
        int step = (dt == 0) ? -1 : +1;

        // debounce semplice per evitare rimbalzi
        usleep(2000); // 2ms

        last_clk = clk;
        return step;
    }

    last_clk = clk;
    return 0;
}

int encoder_button_pressed(void) {
    int sw = gpio_read(PIN_SW);
    if (sw < 0) return 0;

    // rileva transizione 1->0 (premuto) con debounce
    if (last_sw == 1 && sw == 0) {
        usleep(10000);            // 10ms debounce
        sw = gpio_read(PIN_SW);
        if (sw == 0) {
            last_sw = sw;
            return 1;
        }
    }

    last_sw = sw;
    return 0;
}