#include <stdio.h>
#include <unistd.h>
#include "gpio.h"
#include "encoder.h"

// BCM
#define CLK 16
#define DT  20
#define SW  21

int main() {

    // Inizializza encoder
    if (encoder_init(CLK, DT, SW) < 0) {
        printf("Errore inizializzazione encoder\n");
        return 1;
    }

    int counter = 0;

    printf("Encoder pronto...\n");

    while (1) {

        // Controllo rotazione
        int step = encoder_step();
        if (step != 0) {
            counter += step;
            printf("Valore: %d\n", counter);
        }

        // Controllo pulsante
        if (encoder_button_pressed()) {
            printf("CLICK! Reset contatore\n");
            counter = 0;
        }

        // Piccola pausa per non saturare CPU
        usleep(1000);
    }

    gpio_close();
    return 0;
}