#include <stdio.h>
#include "hardware/gpio.h"
#include "pico/stdlib.h"

const int BTN_PIN_Y = 19;
const int BTN_PIN_G = 18;
const int LED_PIN_G = 16;
const int LED_PIN_Y = 15;

volatile bool btn_press_g = false;
volatile bool btn_press_y = false;
volatile bool pisca_g = false;
volatile bool pisca_y = false;

repeating_timer_t timer_g;
repeating_timer_t timer_y;

bool timer_g_callback(repeating_timer_t *rt) {
    pisca_g = true;
    return true;
}

bool timer_y_callback(repeating_timer_t *rt) {
    pisca_y = true;
    return true;
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_G) {
            btn_press_g = !btn_press_g;
            btn_press_y = false;
            cancel_repeating_timer(&timer_y);
            gpio_put(LED_PIN_Y, 0);
            if (btn_press_g)
                add_repeating_timer_ms(100, timer_g_callback, NULL, &timer_g);
            else {
                cancel_repeating_timer(&timer_g);
                gpio_put(LED_PIN_G, 0);
            }
        }
        if (gpio == BTN_PIN_Y) {
            btn_press_y = !btn_press_y;
            btn_press_g = false;
            cancel_repeating_timer(&timer_g);
            gpio_put(LED_PIN_G, 0);
            if (btn_press_y)
                add_repeating_timer_ms(250, timer_y_callback, NULL, &timer_y);
            else {
                cancel_repeating_timer(&timer_y);
                gpio_put(LED_PIN_Y, 0);
            }
        }
    }
}

int main() {
    stdio_init_all();

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);
    gpio_set_irq_enabled_with_callback(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_init(BTN_PIN_Y);
    gpio_set_dir(BTN_PIN_Y, GPIO_IN);
    gpio_pull_up(BTN_PIN_Y);
    gpio_set_irq_enabled_with_callback(BTN_PIN_Y, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);

    bool led_estado_g = false;
    bool led_estado_y = false;

    while (1) {
        if (pisca_g) {
            pisca_g = false;
            led_estado_g = !led_estado_g;
            gpio_put(LED_PIN_G, led_estado_g);
        }
        if (pisca_y) {
            pisca_y = false;
            led_estado_y = !led_estado_y;
            gpio_put(LED_PIN_Y, led_estado_y);
        }
    }
}