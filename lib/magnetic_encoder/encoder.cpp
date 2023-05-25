#include "encoder.h"

#define DATA_LENGTH     1

// Select DMA channels - GLOBAL DECLARTION
int req_chan = dma_claim_unused_channel(true);
int resp_chan = dma_claim_unused_channel(true);

// DMA variables and instances start
dma_channel_config c;
dma_channel_config c2;

// Vars for processing
uint16_t present_angle = 0;
uint16_t previous_angle = 0;
int32_t diff = 0;
uint64_t counter = 0;
uint16_t res = 0;

// DMA functions
void dma_irq_handler()
{
    diff = present_angle - previous_angle;
    previous_angle = present_angle;
    present_angle = res & 0x3FFF;
    
    if(diff < -8000)
        counter++;
    else if(diff > 8000)
        counter--;
    
    // Clear DMA channel interrupt
    dma_hw->ints0 = 1u << resp_chan;
    dma_channel_set_read_addr(req_chan, &angle_addr, true);
    // dma_channel_set_read_addr(req_chan, &cordic_magnitude_addr, true);
}

void dmaInit(void)
{
    // Setup the request channel
    c = dma_channel_get_default_config(req_chan);    // default configs
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);             // 16-bit txfers
    channel_config_set_read_increment(&c, false);                       // no read incrementing
    channel_config_set_write_increment(&c, false);                      // no write incrementing
    channel_config_set_chain_to(&c, resp_chan);                         // chain to response channel
    dma_channel_configure(
        req_chan,                                                       // Channel to be configured
        &c,                                                             // The configuration we just created
        &spi_get_hw(SPI_PORT)->dr,                                      // Write address (data channel read address)
        NULL,                                                           // Read address (POINTER TO AN ADDRESS)
        DATA_LENGTH,                                                    // Number of transfers
        false                                                           // Don't start immediately
    );

    // Setup the response channel
    c2 = dma_channel_get_default_config(resp_chan);  // Default configs
    channel_config_set_transfer_data_size(&c2, DMA_SIZE_16);            // 16-bit txfers
    channel_config_set_read_increment(&c2, false);                      // yes read incrementing
    channel_config_set_write_increment(&c2, false);                     // no write incrementing
    channel_config_set_dreq(&c2, spi_get_dreq(SPI_PORT, false));        // DREQ paced by timer 0
    dma_channel_configure(
        resp_chan,                                                      // Channel to be configured
        &c2,                                                            // The configuration we just created
        &res,                                                           // write address (SPI data register)
        &spi_get_hw(SPI_PORT)->dr,                                      // The initial read address
        DATA_LENGTH,                                                    // Number of transfers
        false                                                           // Don't start immediately.
    );
    
    // Tell the DMA to raise IRQ line 0 when the channel finishes a block
    dma_channel_set_irq0_enabled(resp_chan, true);

    // Configure the processor to run dma_handler() when DMA IRQ 0 is asserted
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
    irq_set_enabled(DMA_IRQ_0, true);
    
    dma_irq_handler();
}



// Encoder functions
uint16_t encoder::readAngle()
{
    uint16_t angle_value = 0;
    // Read Angle from sensor

    gpio_put(PIN_CS, false);
    spi_write16_blocking(SPI_PORT, &angle_addr, 1);
    gpio_put(PIN_CS, true);
    sleep_us(150);

    gpio_put(PIN_CS, false);
    spi_read16_blocking(SPI_PORT, 0, &angle_value, 1);
    sleep_us(50);

    gpio_put(PIN_CS, true);
    sleep_us(150);

    // printf("Present Angle value: %d\n", angle_value);

    return angle_value & 0x3FFF;
}

uint16_t encoder::readCordicMagnitude()
{
    uint16_t cordic_mag_value = 0;
    // Read Angle from sensor

    gpio_put(PIN_CS, false);
    spi_write16_blocking(SPI_PORT, &cordic_magnitude_addr, 1);
    gpio_put(PIN_CS, true);
    sleep_us(150);

    gpio_put(PIN_CS, false);
    spi_read16_blocking(SPI_PORT, 0, &cordic_mag_value, 1);
    sleep_us(50);

    gpio_put(PIN_CS, true);
    sleep_us(150);

    // printf("Present Angle value: %d\n", angle_value);

    return cordic_mag_value & 0x3FFF;
}

bool encoder::writeZeroPosition(uint16_t angle_value)
{   
    //------------------------------------------------------------------
    // ZPOSM write and read
    uint16_t reg = zposm_reg_addr;
    // Write ZPOSM register
    if(parity_check(zposm_reg_addr))
        reg = zposm_reg_addr | (1 << 15);
    // printf("ZPOSM address sent on SPI: %x\n", reg);
    gpio_put(PIN_CS, false);
    spi_write16_blocking(SPI_PORT, &reg, 1);
    gpio_put(PIN_CS, true);
    sleep_us(150);

    uint16_t zposm_reg_value = angle_value >> 6;
    // printf("ZPOSM value sent on SPI: %x\n", zposm_reg_value);
    if(parity_check(zposm_reg_value))
        zposm_reg_value = zposm_reg_value | (1 << 15);
    gpio_put(PIN_CS, false);
    spi_write16_blocking(SPI_PORT, &zposm_reg_value, 1);
    gpio_put(PIN_CS, true);
    sleep_us(150);
    //---------------------------------------
    // Read ZPOSM register
    reg = 0;
    reg = zposm_reg_addr | (1 << 14); // Read bit
    if(parity_check(reg))
        reg = reg | (1 << 15);
    gpio_put(PIN_CS, false);
    spi_write16_blocking(spi1, &reg, 1);
    gpio_put(PIN_CS, true);
    sleep_us(150);

    uint16_t res1;
    gpio_put(PIN_CS, false);
    spi_read16_blocking(spi1, 0, &res1, 1);
    sleep_us(50);
    gpio_put(PIN_CS, true);
    sleep_us(150);
    res1 = res1 & 0x00FF;
    // printf("ZPOSM value stored: %x\n", res1);

    //------------------------------------------------------------------
    // ZPOSL write and read
    reg = zposl_reg_addr;
    // Write POSL register
    if(parity_check(zposl_reg_addr))
        reg = zposl_reg_addr | (1 << 15);
    // printf("ZPOSL address sent on SPI: %x\n", reg);
    gpio_put(PIN_CS, false);
    spi_write16_blocking(SPI_PORT, &reg, 1);
    gpio_put(PIN_CS, true);
    sleep_us(150);

    uint16_t zposl_reg_value = angle_value & 0x003F;
    // printf("ZPOSL value sent on SPI: %x\n", zposl_reg_value);
    if(parity_check(zposl_reg_value))
        zposl_reg_value = zposl_reg_value | (1 << 15);
    gpio_put(PIN_CS, false);
    spi_write16_blocking(SPI_PORT, &zposl_reg_value, 1);
    gpio_put(PIN_CS, true);
    sleep_us(150);
    //---------------------------------------
    // Read ZPOSL register
    reg = 0;
    reg = zposl_reg_addr | (1 << 14); // Read bit
    if(parity_check(reg))
        reg = reg | (1 << 15);
    gpio_put(PIN_CS, false);
    spi_write16_blocking(spi1, &reg, 1);
    gpio_put(PIN_CS, true);
    sleep_us(150);

    uint16_t res2;
    gpio_put(PIN_CS, false);
    spi_read16_blocking(spi1, 0, &res2, 1);
    sleep_us(50);
    gpio_put(PIN_CS, true);
    sleep_us(150);
    res2 = res2 & 0x003F;
    // printf("ZPOSL value stored: %x\n", res2);

    uint16_t final_res = 0;
    final_res = (res1 << 6) | (res2);

    // printf("Zero Position set in sensor is: %d\n", final_res);

    zposm_reg_value = 0;
    zposl_reg_value = 0;

    dma_irq_handler();

    if(final_res == angle_value)
        return true;
    else
        return false;
}

uint8_t encoder::parity_check(uint16_t data)
{
	int count=0;
	// unsigned int b = 1;
	for (unsigned int i=0; i<15; i++) 
    {
		if (data & (1 << i)) 
			count++;
	}
	if (count%2==0)
		return 0;
    else 
		return 1;
}

