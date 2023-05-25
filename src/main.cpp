#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "ws2812.hpp"
#include "hardware/adc.h"
// Private includes
#include "../lib/private_functions/private_functions.h"
////////////////////////////////////////////////////////////////////////////////
#define mainTEMPERATURE_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define mainTEMPERATURE_TASK_FREQUENCY_MS (500 / portTICK_PERIOD_MS)
#define mainTASK_LED (PICO_DEFAULT_LED_PIN)
#define mainBLINK_TASK_PRIORITY (tskIDLE_PRIORITY +1)
#define mainBLINK_TASK_FREQUENCY_MS (300 / portTICK_PERIOD_MS)
#define TEMPERATURE_UNITS 'C'

// Defines
#define LED_PIN             25
#define PRIORITY            1
#define SYSTEM_INITIALIZED  1

////////////////////////////////////////////////////////////////////////////////
TaskHandle_t led_task_handle = NULL;
TaskHandle_t motor_task_handle = NULL;
// TaskHandle_t can_bus_task_handle = NULL;
////////////////////////////////////////////////////////////////////////////////
WS2812 ledStrip(
    16,            // Data line is connected to pin 0. (GP0)
    1,         // Strip is 6 LEDs long.
    pio0,               // Use PIO 0 for creating the state machine.
    0,                  // Index of the state machine that will be created for controlling the LED strip
                        // You can have 4 state machines per PIO-Block up to 8 overall.
                        // See Chapter 3 in: https://datasheets.raspberrypi.org/rp2040/rp2040-datasheet.pdf
    WS2812::FORMAT_RGB  // Pixel format used by the LED strip
);
////////////////////////////////////////////////////////////////////////////////
bool blink_flag = false;
////////////////////////////////////////////////////////////////////////////////
float read_onboard_temperature(const char unit)
{
    
    /* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
    const float conversionFactor = 3.3f / (1 << 12);

    float adc = (float)adc_read() * conversionFactor;
    float tempC = 27.0f - (adc - 0.706f) / 0.001721f;

    if (unit == 'C')
    {
        return tempC;
    }
    else if (unit == 'F')
    {
        return tempC * 9 / 5 + 32;
    }

    return -1.0f;
}



void system_init()
{
    // Debug Init
    stdio_init_all();
    sleep_ms(3000);
    printf("System initializing\n");

    // GPIO Init
    GPIO_init();

    // Motor PWM control Init
    pwm_init_interrupt();

    // Encoder SPI communication Init
    encoder_init();

    // Initialize hardware AD converter, for the temperature sensor
    adc_init();

    blink_flag = true;
}

void led_task(void *params)
{
    uint32_t notification_value = 0;
    for(;;)
    {
        // printf("debug1\n");
        if(xTaskNotifyWait(0, 0, &notification_value, portMAX_DELAY))
        {
            if(notification_value == SYSTEM_INITIALIZED)
            {
                
                gpio_put(LED_PIN, true);
                vTaskDelay(1000/portTICK_PERIOD_MS);
                gpio_put(LED_PIN, false);
            }
                xTaskNotify(motor_task_handle, 1, eSetValueWithoutOverwrite);
            

            // if(notification_value)
        }
    }
}

void motor_task(void *params)
{
    uint32_t notification_value = 0;


    for(;;)
    {
        if(xTaskNotifyWait(0, 0, &notification_value, portMAX_DELAY))
        {
            xTaskNotify(led_task_handle, SYSTEM_INITIALIZED, eSetValueWithoutOverwrite);
        }
            
    }
}

// void can_bus_task(void *params)
// {
    
//     for(;;)
//     {
        
//     }
// }

////////////////////////////////////////////////////////////////////////////////
int main()
{
    /* Configure the hardware ready to run the demo. */
    system_init();

    // mutexLED = xSemaphoreCreateMutex();

    // Tasks Init
    if(xTaskCreate(led_task, "blink", 1024, NULL, PRIORITY, &led_task_handle) != pdPASS)
        printf("Task Creation Failed!\n");
    if(xTaskCreate(motor_task, "temperature", 2048, NULL, PRIORITY, &motor_task_handle) != pdPASS)
        printf("Motor task init failed\n");
    // xTaskCreate(can_bus_task, "blink", 1024, NULL, PRIORITY, &can_bus_task_handle);

    /* Start the tasks and timer running. */
    xTaskNotify(led_task_handle, SYSTEM_INITIALIZED, eSetValueWithoutOverwrite);
    vTaskStartScheduler();

    // if(blink_flag)
    
            
    // vTaskEndScheduler();

    printf("Program complete\n");
    while(1)
    {
        // if(blink_flag)
        //     xTaskNotify(led_task_handle, SYSTEM_INITIALIZED, eSetValueWithoutOverwrite);
            // xTaskNotifyGive
        // printf("Program complete\n");
        char c = getchar_timeout_us(0);
        if(c == '#')
            // reset_usb_boot(0,0);
        sleep_ms(1000);
    }
    

}