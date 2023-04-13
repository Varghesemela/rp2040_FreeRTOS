#define PICO_FLASH_SPI_CLKDIV   4

#include <cmath>
#include <cstring>
#include <string>
using namespace std;

#include "project_config.h"
#include "pins_board.h"
#include "functions.h"
// #include "pico/unique_id.h"

#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <math.h>
#include "hardware/structs/sio.h"

int global_count;

typedef struct can2040_msg CANMsg;
#define LOG( msg ) printf(( __FILE__, msg ))
// pico_unique_board_id_t id_out;

PIO pio;
struct repeating_timer timer1;

// vars


void GPIO_init() {
    gpio_init(LED_SIG);
    gpio_set_dir(LED_SIG, GPIO_OUT);
    gpio_put(LED_SIG, LOW);

    gpio_init(LED_STAT);
    gpio_set_dir(LED_STAT, GPIO_OUT);
    gpio_put(LED_STAT, LOW);

}


#define BUFFER_LENGTH   50
char buffer[BUFFER_LENGTH];
const char s[2] = ",";
const char start_of_frame = '<', end_of_frame = '>';
string command_str, subcommand_str;
string data_str;
uint16_t buffer_index = 0, EOF_index = 0;
bool SOF_flag = false, EOF_flag = false;

// Core 1 interrupt Handler
void core1_interrupt_handler() {
    // Receive Raw Value, Convert and Print Temperature Value
    // while (multicore_fifo_rvalid()){
    //     printf("Core 1");
    // }
    multicore_fifo_clear_irq(); // Clear interrupt
}

/**
 * \brief Core 1 main loop
 *
 * This function serves as the main loop for the secondary core when initialised in the primary core.
 *
 */
void core1_entry() {
    // Configure Core 1 Interrupt
    multicore_fifo_clear_irq();
    multicore_lockout_victim_init();
    // irq_set_exclusive_handler(SIO_IRQ_PROC1, core1_interrupt_handler);
    // irq_set_enabled(SIO_IRQ_PROC1, true);

    while (true) {
        tight_loop_contents();
        gpio_put(LED_STAT, true);
        sleep_ms(333);
        gpio_put(LED_STAT, false);
        sleep_ms(333);

    }
}

bool get_block(struct repeating_timer *t) {
    // buffer_index = 0;
    // while(true){
    int c = getchar_timeout_us(0);
    // printf("%c\n", c);
    if (c == PICO_ERROR_TIMEOUT){
        printf("[%s %d] Error! getchar timeout\n", __FUNCTION__, __LINE__);
        return true;
    }
    if((buffer_index > BUFFER_LENGTH)){
        printf("[%s %d] Error! Buffer filled\n", __FUNCTION__, __LINE__);
        buffer_index = 0;
        return true;
    }
    if(EOF_flag){
        printf("[%s %d] Error! EOF true\n",  __FUNCTION__, __LINE__);
        return true;
    }
    if (c == start_of_frame) {
        SOF_flag = true;
        command_str.clear();
        subcommand_str.clear();
//            data_str.clear();
        std::fill( std::begin(data_str), std::end(data_str), '\0');
        std::fill( std::begin(buffer), std::end(buffer), '\0' );
        buffer_index = 0;
    } else if (c == end_of_frame) {
        EOF_flag = true;
        EOF_index = buffer_index;

        command_str = strtok(buffer, s);
        if (command_str == "#RP64209"){
            reset_usb_boot(0, 0);
        }
        if(!is_only_alphabets(command_str)){
            printf("Invalid command, change later\n");
        }
    }
    if (SOF_flag && !EOF_flag && (c != start_of_frame)) {
        buffer[buffer_index++] = (c & 0xFF);
    }
    return true;
}


int main() {
    set_sys_clock_khz(250000, true);
    stdio_init_all();
    multicore_launch_core1(core1_entry);
    GPIO_init();

    sleep_ms(3000);

    add_repeating_timer_us(-10 * TICK_RATE, get_block, NULL, &timer1);

    printf("Hello\n");
    while (true) {
        if (!EOF_flag) {

        } else {
            if (command_str == "value") {
                data_str = strtok(nullptr, s);
                printf(data_str.c_str());
                printf("\n");
                double pos_in_mm;
                pos_in_mm = strtod(data_str.c_str(), NULL);
                printf("value: %f", pos_in_mm);
                memset(buffer, '\0', sizeof(buffer));

            } else if (command_str == "u2") {
                printf(BOARD_TYPE "\n");
                printf("ok\n");

            } else if (command_str == "help") {
                printCommandlist();
            }

            buffer_index = 0;
            EOF_index = 0;
            memset(buffer, '\0', sizeof(buffer));
            SOF_flag = false;
            EOF_flag = false;
        }
    }
}