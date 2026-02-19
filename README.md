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

-   `gpio_init_out()` → configura un pin come uscita
-   `gpio_init_in()` → configura un pin come ingresso
-   `gpio_write()` → scrive 0/1 su un pin
-   `gpio_read()` → legge 0/1 da un pin

Senza dover gestire direttamente strutture complesse del linguaggio C 

La libreria contiene anche funzioni per gestire i segnali PWM ed è in aggiornamento

------------------------------------------------------------------------

## 📂 Struttura del progetto

    gpio_wrapper/
    ├── esempi/
    |   ├── led.c
    |   ├── led_button.c    
    |   └── esempio_led_bottone.md
    ├── gpio.h
    ├── gpio.c
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

Esempio:

    gcc mio_file.c gpio.c -o mio_eseguibile -lgpiod

Esecuzione:

    ./mio_eseguibile


------------------------------------------------------------------------

## 💡 Esempio: LED lampeggiante led.c

------------------------------------------------------------------------

## 🔘 Esempio: LED + Pulsante led_button.c

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

-   Corsi introduttivi di programmazione in C che usano ibrerie esterne
-   Laboratori di sistemi embedded
-   Introduzione al concetto di I/O digitale
-   Attività pratiche con Raspberry Pi

L'obiettivo è permettere agli studenti di concentrarsi su:

-   logica booleana
-   lettura e scrittura digitale
-   interazione hardware base

senza essere distratti dalla complessità dell'API ufficiale di libgpiod.

------------------------------------------------------------------------

## 📜 Licenza

Uso libero per scopi didattici.
