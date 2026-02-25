#pragma once

// Inizializza encoder: clk, dt e (opzionale) sw
int encoder_init(unsigned int clk, unsigned int dt, unsigned int sw);

// Ritorna: -1 (sinistra), +1 (destra), 0 (nessun movimento)
int encoder_step(void);

// Ritorna 1 quando il pulsante viene premuto (con debounce), altrimenti 0
int encoder_button_pressed(void);