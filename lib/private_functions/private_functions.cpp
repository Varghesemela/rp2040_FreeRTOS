#include "private_functions.h"

uint64_t step_counter = 0;

encoder mag_encoder;

// GPIO Init Routine
void GPIO_init() 
{
    gpio_init(LED_SIG);
    gpio_set_dir(LED_SIG, GPIO_OUT);
    gpio_put(LED_SIG, LOW);

    gpio_init(LED_STAT);
    gpio_set_dir(LED_STAT, GPIO_OUT);
    gpio_put(LED_STAT, LOW);

    gpio_init(BOOST_EN);
    gpio_set_dir(BOOST_EN, GPIO_OUT);
    gpio_put(BOOST_EN, HIGH);

    gpio_init(LED_DRV_PIN);
    gpio_set_function(LED_DRV_PIN, GPIO_FUNC_PWM);

    gpio_init(MOTOR_DIR);
    gpio_set_dir(MOTOR_DIR, GPIO_OUT);
    gpio_put(MOTOR_DIR, LOW);

    gpio_init(MOTOR_STEP);
    gpio_set_dir(MOTOR_STEP, GPIO_OUT);
    gpio_set_function(MOTOR_STEP, GPIO_FUNC_PWM);

    gpio_init(MOTOR_ENA);
    gpio_set_dir(MOTOR_ENA, GPIO_OUT);
    gpio_put(MOTOR_ENA, LOW);

    gpio_init(LS1);
    gpio_set_dir(LS1, GPIO_IN);
    gpio_init(LS2);
    gpio_set_dir(LS2, GPIO_IN);
}

//////////////////////////////////////////////////////////////////////////////////
// Stepper PWM functions init
//////////////////////////////////////////////////////////////////////////////////
void pwm_init_interrupt(){
    pwm_clear_irq(pwm_gpio_to_slice_num(MOTOR_STEP));
    pwm_set_irq_enabled(pwm_gpio_to_slice_num(MOTOR_STEP),true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, step_incrementer_routine);
    irq_set_priority(PWM_IRQ_WRAP, PICO_DEFAULT_IRQ_PRIORITY-1);
    irq_set_enabled(PWM_IRQ_WRAP, true);
}

void step_incrementer_routine()
{
    step_counter++;
    pwm_clear_irq(pwm_gpio_to_slice_num(MOTOR_STEP));
}

//////////////////////////////////////////////////////////////////////////////////
// Encoder Functions
//////////////////////////////////////////////////////////////////////////////////
void encoder_init()
{
    // Initialize communication
    spi_init();
    // Set zero angle for first time
    set_zero_angle_routine();
}

void spi_init()
{
    // Initialize SPI channel (channel, baud rate set to 20MHz)
    spi_init(SPI_PORT, 1* 1000000) ;

    // Format SPI channel (channel, data bits per transfer, polarity, phase, order)
    spi_set_format(SPI_PORT, 16, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);

   
    // Map SPI signals to GPIO ports, acts like framed SPI with this CS mapping
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS, GPIO_FUNC_SPI) ;
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
}

void set_zero_angle_routine()
{
    uint16_t new_zero_angle = 0;
    uint16_t final_angle = 0;
    do
    {
        uint16_t angle_read = 0;
        angle_read = mag_encoder.readAngle();
        new_zero_angle += angle_read;    
        mag_encoder.writeZeroPosition(new_zero_angle);
        final_angle = mag_encoder.readAngle();
    } while (final_angle != 0);    
}