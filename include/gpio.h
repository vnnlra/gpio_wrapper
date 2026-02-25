#pragma once

// GPIO base
int gpio_init_out(unsigned int line, int initial_value);
int gpio_write(unsigned int line, int value);
int gpio_init_in(unsigned int line);
int gpio_read(unsigned int line);
void gpio_close(void);

// PWM software per controllo luminosità LED (stile WiringPi softPwm)
// value: 0..range
// range tipico: 100
// Frequenza PWM ~100Hz (adatta per LED, NON per servo)
/*
------------------------------------------------------------------------------
PWM (Pulse Width Modulation) - Controllo luminosità LED
------------------------------------------------------------------------------

Il PWM permette di controllare la luminosità di un LED senza modificare
la tensione, ma variando il tempo in cui il segnale rimane HIGH rispetto
al tempo in cui rimane LOW.

Il principio è il seguente:

    Periodo fisso (es. 10 ms = 100 Hz)
    Durante ogni periodo il segnale viene:
        - acceso (HIGH) per un certo tempo
        - spento (LOW) per il tempo restante

La percentuale di tempo in cui il segnale è HIGH si chiama DUTY CYCLE.

Esempio (periodo = 10 ms):

    Duty 0%   -> sempre LOW  -> LED spento
    Duty 50%  -> 5 ms HIGH + 5 ms LOW -> luminosità media
    Duty 100% -> sempre HIGH -> luminosità massima

Poiché la commutazione avviene molto velocemente (decine o centinaia di volte
al secondo), l'occhio umano non percepisce lo sfarfallio ma una luminosità
media proporzionale al duty cycle.

Nel nostro caso:
    - "range" definisce il valore massimo (es. 100)
    - "value" va da 0 a range
    - duty cycle = value / range

Questa implementazione usa un thread che genera un PWM software
(con periodo fisso ~100 Hz).

Limiti:
    - Non è real-time
    - Può avere jitter se la CPU è sotto carico
    - Adatto per LED
    - Non consigliato per servo o segnali temporizzati critici

------------------------------------------------------------------------------
*/
int gpio_init_pwm(unsigned int line, int initial_value, int range);
int gpio_write_pwm(unsigned int line, int value);
int gpio_stop_pwm(unsigned int line);
