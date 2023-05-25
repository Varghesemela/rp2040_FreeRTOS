#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"

//SPI pin configurations
#define SPI_PORT        spi1
// #define PIN_CS          13
// #define PIN_SCK         14
// #define PIN_MISO        12
// #define PIN_MOSI        15
#define PIN_CS          13
#define PIN_SCK         10
#define PIN_MISO        12
#define PIN_MOSI        11

// Register Addresses
static uint16_t cordic_magnitude_addr = 0xBFFD;
static uint16_t angle_addr = 0xFFFF; // Angle address with parity and read bit appended
static uint16_t zposm_reg_addr = 0x0016;
static uint16_t zposl_reg_addr = 0x0017;
static uint16_t settings1_reg_addr = 0x0018;

class encoder{
    public:
        uint16_t readAngle();
        uint16_t readCordicMagnitude();
        bool writeZeroPosition(uint16_t angle_value);
        uint8_t parity_check(uint16_t data);
    private:
};

void set_zero_angle_routine();
void dmaInit(void);
void dma_irq_handler();