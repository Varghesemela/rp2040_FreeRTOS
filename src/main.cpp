#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "ws2812.hpp"

#include "hardware/adc.h"

const int taskDelay = 100;
const int taskSize = 128;
SemaphoreHandle_t mutex, mutexLED;


#define mainTEMPERATURE_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define mainTEMPERATURE_TASK_FREQUENCY_MS (500 / portTICK_PERIOD_MS)
#define mainTASK_LED (PICO_DEFAULT_LED_PIN)
#define mainBLINK_TASK_PRIORITY (tskIDLE_PRIORITY +1)
#define mainBLINK_TASK_FREQUENCY_MS (300 / portTICK_PERIOD_MS)
#define TEMPERATURE_UNITS 'C'
WS2812 ledStrip(
    16,            // Data line is connected to pin 0. (GP0)
    1,         // Strip is 6 LEDs long.
    pio0,               // Use PIO 0 for creating the state machine.
    0,                  // Index of the state machine that will be created for controlling the LED strip
                        // You can have 4 state machines per PIO-Block up to 8 overall.
                        // See Chapter 3 in: https://datasheets.raspberrypi.org/rp2040/rp2040-datasheet.pdf
    WS2812::FORMAT_RGB  // Pixel format used by the LED strip
);



void vSafePrint(char *out)
{
    xSemaphoreTake(mutex, portMAX_DELAY);
    printf(out);
    xSemaphoreGive(mutex);
}

void vSafeLED(uint8_t red, uint8_t green, uint8_t blue){
    xSemaphoreTake(mutexLED, portMAX_DELAY);
    ledStrip.fill( WS2812::RGB(red, green, blue) );
    ledStrip.show();
    xSemaphoreGive(mutexLED);
}

/* References for this implementation:
 * raspberry-pi-pico-c-sdk.pdf, Section '4.1.1. hardware_adc'
 * pico-examples/adc/adc_console/adc_console.c */
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

static void prvBlinkTask(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xNextWakeTime;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();
    int count=0;
    char out[32];


    for (;;)
    {
        vSafeLED(count++, 0, 0);
        // sprintf(out, "Helloo boy %d\n", count++);
        // vSafePrint(out);
        // printf("Helloo boy %d\n", count++);
        xTaskDelayUntil(&xNextWakeTime, mainBLINK_TASK_FREQUENCY_MS);
    }
}

static void prvTemperatureTask(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xNextWakeTime;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    /* Enable onboard temperature sensor and
     *   select its channel (do this once for efficiency, but beware that this
     *   is a global operation). */
    adc_set_temp_sensor_enabled(true);
    uint8_t counter;

    for (;;)
    {
        // TODO set a semaphore
        vSafeLED(0, 0, counter++);
        // switch to the temperature mux if needed
        if (adc_get_selected_input() != 4)
        {
            adc_select_input(4);
        }
        float temperature = read_onboard_temperature(TEMPERATURE_UNITS);
        // sprintf(out , "Onboard temperature = %.02f %c\n", temperature, TEMPERATURE_UNITS);
        // vSafePrint(out);
        printf("Onboard temperature = %.02f %c\n", temperature, TEMPERATURE_UNITS);
        xTaskDelayUntil(&xNextWakeTime, mainTEMPERATURE_TASK_FREQUENCY_MS);
    }
}

static void prvSetupHardware( void )
{
    stdio_init_all();
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, 1);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    // Initialize hardware AD converter, for the temperature sensor
    adc_init();
}

int main( void )
{
    /* Configure the hardware ready to run the demo. */
    stdio_init_all();
    prvSetupHardware();

    mutexLED = xSemaphoreCreateMutex();


    xTaskCreate(prvBlinkTask, "blink", configMINIMAL_STACK_SIZE, NULL, mainBLINK_TASK_PRIORITY, NULL);
    xTaskCreate(prvTemperatureTask, "temperature", configMINIMAL_STACK_SIZE, NULL, mainTEMPERATURE_TASK_PRIORITY, NULL);

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
    // ...

}