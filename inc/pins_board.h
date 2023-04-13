#ifndef PINS_BOARD_H
#define PINS_BOARD_H


#define BOARD_TYPE "Waveshare"


/*
* GPIO Pins declaration for strobing
*/

#define i2c0_sda_pin    4
#define i2c0_scl_pin    5
#define i2c1_sda_pin    6
#define i2c1_scl_pin    7

#define SPI_PORT    spi1
#define MOSI        11
#define SCK         10
#define CS          13

#define UART_ID     uart0
#define UART_RX_PIN   17
#define UART_TX_PIN   16

#define LED_STAT        25
#define LED_SIG         28


#endif