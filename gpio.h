#pragma once

int gpio_init_out(unsigned int line, int initial_value);
int gpio_write(unsigned int line, int value);
int gpio_init_in(unsigned int line);
int gpio_read(unsigned int line);
void gpio_close(void);
