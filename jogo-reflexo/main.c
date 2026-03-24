#include <stdio.h>

#include "hardware/gpio.h"
#include "pico/stdlib.h"

const int LED_PIN_G = 5;
const int LED_PIN_Y = 9;
const int LED_PIN_R = 13;

const int BTN_PIN_G = 28;
const int BTN_PIN_Y = 26;
const int BTN_PIN_R = 20;

volatile int btn_f = 0;
volatile int g_timer_g = 0;
volatile int g_timer_r = 0;
volatile int g_timer_y = 0;


void btn_callback(uint gpio, uint32_t events) {

    
    if (events == 0x4) {  // fall edge
        btn_f = gpio;
    } else if (events == 0x8) {  // rise edge
    }
}

bool timer_g_callback(repeating_timer_t *rt){
    g_timer_g = 1;
    return true;
}

bool timer_r_callback(repeating_timer_t *rt){
    g_timer_r = 1;
    return true;
}

bool timer_y_callback(repeating_timer_t *rt){
    g_timer_y= 1;
    return true;
}

int main() {
    stdio_init_all();

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);
    gpio_set_irq_enabled_with_callback(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);

    gpio_init(BTN_PIN_Y);
    gpio_set_dir(BTN_PIN_Y, GPIO_IN);
    gpio_pull_up(BTN_PIN_Y);
    gpio_set_irq_enabled_with_callback(BTN_PIN_Y, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);
    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    while (true) {
    }
}