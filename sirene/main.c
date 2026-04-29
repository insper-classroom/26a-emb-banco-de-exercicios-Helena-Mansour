#include <stdio.h>
#include "hardware/gpio.h"
#include "pico/stdlib.h"

const int BTN_PIN_G = 18;
const int LED_PIN_R = 14;
const int LED_PIN_B = 17;

volatile bool ligado = false;
volatile bool pisca = false;
repeating_timer_t timer;

bool timer_callback(repeating_timer_t *rt) {
    pisca = true;
    return true;
}

void btn_callback(uint gpio, uint32_t events) {
    ligado = !ligado;
    if (ligado) {
        gpio_put(LED_PIN_R, 1);
        gpio_put(LED_PIN_B, 0);
        add_repeating_timer_ms(300, timer_callback, NULL, &timer);
    } else {
        cancel_repeating_timer(&timer);
        gpio_put(LED_PIN_R, 0);
        gpio_put(LED_PIN_B, 0);
    }
}

int main() {
    stdio_init_all();

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);
    gpio_set_irq_enabled_with_callback(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);

    bool estado = true; // true = vermelho aceso

    while (1) {
        if (pisca) {
            pisca = false;
            estado = !estado;
            gpio_put(LED_PIN_R, estado);
            gpio_put(LED_PIN_B, !estado);
        }
    }
}