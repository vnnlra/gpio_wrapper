#include <stdio.h>
#include <unistd.h>
#include "gpio.h"
#include "encoder.h"

// BCM
#define LED 17
#define CLK 16
#define DT  20
#define SW  21

#define RANGE 100     // come WiringPi softPwm: 0..100
#define STEP  2       // quanto cambia ogni scatto encoder (2% circa)

static int clamp(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

int main() {

    if (encoder_init(CLK, DT, SW) < 0) {
        printf("Errore inizializzazione encoder\n");
        return 1;
    }

    if (gpio_init_pwm(LED, 0, RANGE) < 0) {
        printf("Errore inizializzazione PWM\n");
        return 1;
    }

    int brightness = 0;
    gpio_write_pwm(LED, brightness);
    printf("Luminosita': %d/%d\n", brightness, RANGE);

    while (1) {
        int step = encoder_step();   // -1, 0, +1

        if (step != 0) {
            // Se il tuo encoder conta al contrario, inverti qui: brightness -= step * STEP;
            brightness += step * STEP;
            brightness = clamp(brightness, 0, RANGE);

            gpio_write_pwm(LED, brightness);
            printf("Luminosita': %d/%d\n", brightness, RANGE);
        }

        if (encoder_button_pressed()) {
            brightness = 0;
            gpio_write_pwm(LED, brightness);
            printf("CLICK: reset -> %d/%d\n", brightness, RANGE);
        }

        usleep(1000); // alleggerisce la CPU
    }

    gpio_close();
    return 0;
}