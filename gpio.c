#include "gpio.h"
#include <gpiod.h>
#include <stdio.h>

#define CHIP_PATH "/dev/gpiochip0"
#define MAX_LINES 64

static struct gpiod_chip *chip = NULL;
static struct gpiod_line_request *req_out[MAX_LINES] = {0};
static struct gpiod_line_request *req_in[MAX_LINES]  = {0};

static int ensure_chip(void) {
    if (chip) return 0;

    chip = gpiod_chip_open(CHIP_PATH);
    if (!chip) {
        perror("gpiod_chip_open");
        return -1;
    }
    return 0;
}

static void release_req(struct gpiod_line_request **r) {
    if (*r) {
        gpiod_line_request_release(*r);
        *r = NULL;
    }
}

int gpio_init_out(unsigned int line, int initial_value) {
    if (line >= MAX_LINES) {
        fprintf(stderr, "gpio_init_out: line %u out of range (max %d)\n", line, MAX_LINES - 1);
        return -1;
    }
    if (ensure_chip() < 0) return -1;

    // If already requested, release and re-request.
    release_req(&req_out[line]);

    struct gpiod_line_settings *settings = gpiod_line_settings_new();
    struct gpiod_line_config *cfg = gpiod_line_config_new();
    struct gpiod_request_config *rcfg = gpiod_request_config_new();
    if (!settings || !cfg || !rcfg) {
        fprintf(stderr, "gpio_init_out: allocation failed\n");
        gpiod_line_settings_free(settings);
        gpiod_line_config_free(cfg);
        gpiod_request_config_free(rcfg);
        return -1;
    }

    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
    gpiod_line_settings_set_output_value(settings, initial_value ? 1 : 0);

    unsigned int offsets[] = { line };
    if (gpiod_line_config_add_line_settings(cfg, offsets, 1, settings) < 0) {
        perror("gpiod_line_config_add_line_settings");
        gpiod_line_settings_free(settings);
        gpiod_line_config_free(cfg);
        gpiod_request_config_free(rcfg);
        return -1;
    }

    gpiod_request_config_set_consumer(rcfg, "gpio-out");

    req_out[line] = gpiod_chip_request_lines(chip, rcfg, cfg);
    if (!req_out[line]) {
        perror("gpiod_chip_request_lines(out)");
        gpiod_line_settings_free(settings);
        gpiod_line_config_free(cfg);
        gpiod_request_config_free(rcfg);
        return -1;
    }

    gpiod_line_settings_free(settings);
    gpiod_line_config_free(cfg);
    gpiod_request_config_free(rcfg);
    return 0;
}

int gpio_write(unsigned int line, int value) {
    if (line >= MAX_LINES) return -1;
    if (!req_out[line]) return -1;
    return gpiod_line_request_set_value(req_out[line], line, value ? 1 : 0);
}

int gpio_init_in(unsigned int line) {
    if (line >= MAX_LINES) {
        fprintf(stderr, "gpio_init_in: line %u out of range (max %d)\n", line, MAX_LINES - 1);
        return -1;
    }
    if (ensure_chip() < 0) return -1;

    // If already requested, release and re-request.
    release_req(&req_in[line]);

    struct gpiod_line_settings *settings = gpiod_line_settings_new();
    struct gpiod_line_config *cfg = gpiod_line_config_new();
    struct gpiod_request_config *rcfg = gpiod_request_config_new();
    if (!settings || !cfg || !rcfg) {
        fprintf(stderr, "gpio_init_in: allocation failed\n");
        gpiod_line_settings_free(settings);
        gpiod_line_config_free(cfg);
        gpiod_request_config_free(rcfg);
        return -1;
    }

    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);

    unsigned int offsets[] = { line };
    if (gpiod_line_config_add_line_settings(cfg, offsets, 1, settings) < 0) {
        perror("gpiod_line_config_add_line_settings");
        gpiod_line_settings_free(settings);
        gpiod_line_config_free(cfg);
        gpiod_request_config_free(rcfg);
        return -1;
    }

    gpiod_request_config_set_consumer(rcfg, "gpio-in");

    req_in[line] = gpiod_chip_request_lines(chip, rcfg, cfg);
    if (!req_in[line]) {
        perror("gpiod_chip_request_lines(in)");
        gpiod_line_settings_free(settings);
        gpiod_line_config_free(cfg);
        gpiod_request_config_free(rcfg);
        return -1;
    }

    gpiod_line_settings_free(settings);
    gpiod_line_config_free(cfg);
    gpiod_request_config_free(rcfg);
    return 0;
}

int gpio_read(unsigned int line) {
    if (line >= MAX_LINES) return -1;
    if (!req_in[line]) return -1;
    return gpiod_line_request_get_value(req_in[line], line);
}

void gpio_close(void) {
    for (int i = 0; i < MAX_LINES; i++) {
        release_req(&req_out[i]);
        release_req(&req_in[i]);
    }
    if (chip) {
        gpiod_chip_close(chip);
        chip = NULL;
    }
}