# gpio_wrapper

Wrapper minimale per l'utilizzo dei GPIO della Raspberry Pi in C, basato
su **libgpiod v2**.

Progetto pensato per uso didattico: semplifica l'API ufficiale di
libgpiod nascondendo la complessità di strutture, configurazioni e
puntatori.

------------------------------------------------------------------------

## 🎯 Obiettivo

Permettere agli studenti di utilizzare i GPIO con poche funzioni
semplici:

-   `gpio_out()` → configura un pin come uscita\
-   `gpio_in()` → configura un pin come ingresso\
-   `gpio_write()` → scrive 0/1 su un pin\
-   `gpio_read()` → legge 0/1 da un pin

Senza dover gestire direttamente strutture complesse del linguaggio C 


------------------------------------------------------------------------

## 📂 Struttura del progetto

    gpio_wrapper/
    ├── gpio.h
    ├── gpio.c
    ├── led.c
    ├── led_button.c
    └── README.md

------------------------------------------------------------------------

## 🔧 Requisiti

-   Raspberry Pi\
-   Linux (Raspberry Pi OS / Debian)\
-   `libgpiod` installata\
-   GCC

Verifica che libgpiod sia disponibile:

    gpiodetect

Se compare `gpiochip0`, il sistema è pronto.

------------------------------------------------------------------------

## ⚙️ Compilazione

Esempio LED:

    gcc led.c gpio.c -o led -lgpiod

Esempio LED + Button:

    gcc led_button.c gpio.c -o led_button -lgpiod

Esecuzione (serve accesso ai GPIO):

    sudo ./led

Oppure aggiungere l'utente al gruppo `gpio`.

------------------------------------------------------------------------

## 💡 Esempio: LED lampeggiante

``` c
#include "gpio.h"
#include <unistd.h>

#define LED 17

int main() {

    gpio_out(LED, 0);

    while (1) {
        gpio_write(LED, 1);
        sleep(1);
        gpio_write(LED, 0);
        sleep(1);
    }

    gpio_close();
    return 0;
}
```

------------------------------------------------------------------------

## 🔘 Esempio: LED + Pulsante

``` c
#include "gpio.h"
#include <unistd.h>

#define LED 17
#define BTN 23

int main() {

    gpio_out(LED, 0);
    gpio_in(BTN);

    while (1) {

        if (gpio_read(BTN))
            gpio_write(LED, 1);
        else
            gpio_write(LED, 0);

        usleep(10000);
    }

    gpio_close();
    return 0;
}
```

------------------------------------------------------------------------

## ⚠️ Note importanti

-   I numeri dei pin sono in **numerazione BCM**, non numerazione
    fisica.
-   Il wrapper è pensato per semplicità didattica, non per applicazioni
    real-time.
-   Non include:
    -   PWM
    -   Interrupt
    -   Pull-up/down interni
    -   Gestione avanzata degli errori

------------------------------------------------------------------------

## 📚 Scopo del progetto

Questo progetto nasce per:

-   Corsi introduttivi di programmazione in C
-   Laboratori di sistemi embedded
-   Introduzione al concetto di I/O digitale
-   Attività pratiche con Raspberry Pi

L'obiettivo è permettere agli studenti di concentrarsi su:

-   strutture di controllo (`if`, `while`)
-   logica booleana
-   lettura e scrittura digitale
-   interazione hardware base

senza essere distratti dalla complessità dell'API ufficiale di libgpiod.

------------------------------------------------------------------------

## 📜 Licenza

Uso libero per scopi didattici.
