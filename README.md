# GPIO Wrapper -- Raspberry Pi

Mini wrapper didattico per utilizzare i GPIO del Raspberry Pi con
**libgpiod**.

Il progetto è pensato per studenti di informatica: 
- struttura semplice e leggibile 
- compilazione manuale con `gcc` 
- nessun Makefile 
- esempi indipendenti

------------------------------------------------------------------------

## Struttura del progetto

    gpio_wrapper/
    ├── docs/
    ├── examples/
    ├── include/
    ├── src/
    └── tools/

-   `src/` → implementazione della libreria (GPIO + encoder)
-   `include/` → header pubblici
-   `examples/` → programmi di esempio
-   `docs/` → documentazione aggiuntiva
-   `tools/` → script di utilità (es. reset GPIO)

------------------------------------------------------------------------

## Requisiti

Sistema: Raspberry Pi OS / Debian

Installazione dipendenze:

``` bash
sudo apt update
sudo apt install build-essential libgpiod-dev
```

------------------------------------------------------------------------

## Compilazione

La compilazione avviene direttamente con `gcc`.

Regola generale:

``` bash
gcc -Iinclude examples/<file>.c src/<moduli necessari>.c \
    -lgpiod -pthread -o nome_programma
```
> Nota 1: il progetto utilizza thread (`pthread`) per il PWM, quindi è
> necessario compilare con l'opzione `-pthread`.

> Nota 2: non è detto che `-pthread` sia necessario ma comunque non guasta niente

Esempio semplice:

``` bash
gcc -Iinclude examples/led.c src/gpio.c \
    -lgpiod -pthread -o led
```

Esecuzione:

``` bash
sudo ./led
```

Esempio con modulo per encoder:

``` bash
gcc -Iinclude examples/led_encoder.c \
    src/gpio.c src/encoder.c \
    -lgpiod -pthread -o led_encoder
```

Esecuzione:

``` bash
sudo ./led_encoder
```


------------------------------------------------------------------------

## Reset dei GPIO

Se un programma viene interrotto con `CTRL+C` e i pin restano occupati:

``` bash
sudo ./tools/gpio_reset.sh
```

------------------------------------------------------------------------

## Obiettivo didattico

Questo progetto permette di comprendere:

-   compilazione multi-file con `gcc`
-   uso di librerie esterne
-   gestione dei GPIO con `libgpiod`
-   utilizzo dei thread (`pthread`)
-   organizzazione di un progetto C semplice ma strutturato
