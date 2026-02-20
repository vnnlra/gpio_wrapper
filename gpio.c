#include "gpio.h"
#include <gpiod.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define CHIP_PATH "/dev/gpiochip0"
#define MAX_LINES 64

// -----------------------------------------------------------------------------
// GPIO base (libgpiod v2)
// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------
// PWM software per LED (stile WiringPi softPwm)
// - Un thread per pin PWM
// - Periodo fisso ~10ms (~100Hz)
// Limiti: non real-time, può avere jitter se il sistema è carico.
// -----------------------------------------------------------------------------

typedef struct {
    int active;
    int running;
    unsigned int line;
    int range;          // es. 100
    int value;          // 0..range
    pthread_t th;
    pthread_mutex_t mtx;
} pwm_chan_t;

static pwm_chan_t pwm[MAX_LINES] = {0};

static void sleep_us(int us) {
    struct timespec ts;
    ts.tv_sec = us / 1000000;
    ts.tv_nsec = (long)(us % 1000000) * 1000L;
    nanosleep(&ts, NULL);
}

static void *pwm_thread(void *arg) {
    pwm_chan_t *c = (pwm_chan_t *)arg;
    const int period_us = 10000; // 10ms => 100Hz

    while (c->running) {
        int v, r;

        pthread_mutex_lock(&c->mtx);
        v = c->value;
        r = c->range;
        pthread_mutex_unlock(&c->mtx);

        if (r <= 0) r = 100;
        if (v < 0) v = 0;
        if (v > r) v = r;

        int on_us  = (period_us * v) / r;
        int off_us = period_us - on_us;

        if (on_us > 0) {
            gpio_write(c->line, 1);
            sleep_us(on_us);
        }
        if (off_us > 0) {
            gpio_write(c->line, 0);
            sleep_us(off_us);
        }
    }

    // a fine PWM, spegne
    gpio_write(c->line, 0);
    return NULL;
}

int gpio_init_pwm(unsigned int line, int initial_value, int range) {
    if (line >= MAX_LINES) return -1;
    if (range <= 0) range = 100;

    // Richiede pin come output (parte LOW: il thread gestisce il duty)
    if (gpio_init_out(line, 0) < 0) return -1;

    // Se già attivo, fermalo e riparti
    if (pwm[line].active) {
        gpio_stop_pwm(line);
    }

    pwm[line].active = 1;
    pwm[line].running = 1;
    pwm[line].line = line;
    pwm[line].range = range;
    pwm[line].value = initial_value;

    if (pthread_mutex_init(&pwm[line].mtx, NULL) != 0) {
        perror("pthread_mutex_init");
        pwm[line].active = 0;
        pwm[line].running = 0;
        return -1;
    }

    if (pthread_create(&pwm[line].th, NULL, pwm_thread, &pwm[line]) != 0) {
        perror("pthread_create");
        pthread_mutex_destroy(&pwm[line].mtx);
        pwm[line].active = 0;
        pwm[line].running = 0;
        return -1;
    }

    return 0;
}

int gpio_write_pwm(unsigned int line, int value) {
    if (line >= MAX_LINES) return -1;
    if (!pwm[line].active) return -1;

    pthread_mutex_lock(&pwm[line].mtx);

    if (value < 0) value = 0;
    if (value > pwm[line].range) value = pwm[line].range;
    pwm[line].value = value;

    pthread_mutex_unlock(&pwm[line].mtx);
    return 0;
}

int gpio_stop_pwm(unsigned int line) {
    if (line >= MAX_LINES) return -1;
    if (!pwm[line].active) return 0;

    pwm[line].running = 0;
    pthread_join(pwm[line].th, NULL);
    pthread_mutex_destroy(&pwm[line].mtx);

    pwm[line].active = 0;
    // lascia il pin in output e LOW
    gpio_write(line, 0);
    return 0;
}

void gpio_close(void) {
    // ferma eventuali PWM attivi
    for (int i = 0; i < MAX_LINES; i++) {
        if (pwm[i].active) gpio_stop_pwm((unsigned int)i);
    }

    for (int i = 0; i < MAX_LINES; i++) {
        release_req(&req_out[i]);
        release_req(&req_in[i]);
    }
    if (chip) {
        gpiod_chip_close(chip);
        chip = NULL;
    }
}
