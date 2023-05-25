#include "../../inc/pins_picofeeder_board.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/spi.h"
#include "../lib/magnetic_encoder/encoder.h"

#define HIGH    true
#define LOW     false

void GPIO_init();
void pwm_init_interrupt();
void step_incrementer_routine();
void spi_init();
void encoder_init();