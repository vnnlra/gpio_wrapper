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
int gpio_init_pwm(unsigned int line, int initial_value, int range);
int gpio_write_pwm(unsigned int line, int value);
int gpio_stop_pwm(unsigned int line);
