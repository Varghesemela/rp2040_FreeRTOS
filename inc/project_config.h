#ifndef CONFIG_H
#define CONFIG_H

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <pico/multicore.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "hardware/uart.h"
#include "pico/bootrom.h"


#define FW_TYPE "FreeRTOS"
#define FW_VERSION ""
#define AUTHOR      "SNJY"

// #define _DEBUG
#ifdef _DEBUG
#define printf_debug  printf
#else
#define printf_debug(s, ...) ((void)0)
#endif


#define SECRET_CODE 64209

#define HIGH        1
#define LOW         0

#define CW          HIGH
#define CCW         LOW

#define TICK_RATE   500.0

#endif