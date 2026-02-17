#include "gpio.h"
#include <gpiod.h>
#include <stdio.h>

#define CHIP_PATH "/dev/gpiochip0"

static struct gpiod_chip *chip = NULL;
static struct gpiod_line_request *req_out = NULL;
static struct gpiod_line_request *req_in  = NULL;

static int ensure_chip(void) {
    if (chip) return 0;
    chip = gpiod_chip_open(CHIP_PATH);
    if (!chip) { perror("gpiod_chip_open"); return -1; }
    return 0;
}

int gpio_init_out(unsigned int line, int initial_value) {
    if (ensure_chip() < 0) return -1;

    struct gpiod_line_settings *s = gpiod_line_settings_new();
    struct gpiod_line_config *cfg = gpiod_line_config_new();
    struct gpiod_request_config *rcfg = gpiod_request_config_new();
    if (!s || !cfg || !rcfg) return -1;

    gpiod_line_settings_set_direction(s, GPIOD_LINE_DIRECTION_OUTPUT);
    gpiod_line_settings_set_output_value(s, initial_value ? 1 : 0);

    unsigned int offsets[] = { line };
    gpiod_line_config_add_line_settings(cfg, offsets, 1, s);
    gpiod_request_config_set_consumer(rcfg, "class-gpio-out");

    if (req_out) gpiod_line_request_release(req_out);
    req_out = gpiod_chip_request_lines(chip, rcfg, cfg);
    if (!req_out) { perror("gpiod_chip_request_lines(out)"); return -1; }

    gpiod_line_settings_free(s);
    gpiod_line_config_free(cfg);
    gpiod_request_config_free(rcfg);
    return 0;
}

int gpio_write(unsigned int line, int value) {
    if (!req_out) return -1;
    return gpiod_line_request_set_value(req_out, line, value ? 1 : 0);
}

int gpio_init_in(unsigned int line) {
    if (ensure_chip() < 0) return -1;

    struct gpiod_line_settings *s = gpiod_line_settings_new();
    struct gpiod_line_config *cfg = gpiod_line_config_new();
    struct gpiod_request_config *rcfg = gpiod_request_config_new();
    if (!s || !cfg || !rcfg) return -1;

    gpiod_line_settings_set_direction(s, GPIOD_LINE_DIRECTION_INPUT);

    unsigned int offsets[] = { line };
    gpiod_line_config_add_line_settings(cfg, offsets, 1, s);
    gpiod_request_config_set_consumer(rcfg, "class-gpio-in");

    if (req_in) gpiod_line_request_release(req_in);
    req_in = gpiod_chip_request_lines(chip, rcfg, cfg);
    if (!req_in) { perror("gpiod_chip_request_lines(in)"); return -1; }

    gpiod_line_settings_free(s);
    gpiod_line_config_free(cfg);
    gpiod_request_config_free(rcfg);
    return 0;
}

int gpio_read(unsigned int line) {
    if (!req_in) return -1;
    return gpiod_line_request_get_value(req_in, line);
}

void gpio_close(void) {
    if (req_out) { gpiod_line_request_release(req_out); req_out = NULL; }
    if (req_in)  { gpiod_line_request_release(req_in);  req_in  = NULL; }
    if (chip)    { gpiod_chip_close(chip); chip = NULL; }
}
