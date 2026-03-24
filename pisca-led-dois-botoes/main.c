

#include <stdio.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"

const int BTN_PIN_G = 20;
const int BTN_PIN_Y = 19;

const int LED_PIN_G = 15;
const int LED_PIN_Y = 14;

volatile bool btn_g_press = false;
volatile bool btn_y_press = false;
volatile bool pisca_y = false;
volatile bool pisca_g = false;
volatile bool alarme_g = false;
volatile bool alarme_y = false;


void btn_callback(uint gpio, uint32_t events) {
    if(events == 0x4){
        if(gpio == BTN_PIN_G){
            btn_g_press = true;
        }
        if(gpio == BTN_PIN_Y){
            btn_y_press = true;
        }
    }
}

bool timer_y_callback(repeating_timer_t *rt) {
    pisca_y = true;
    return true;

}
bool timer_g_callback(repeating_timer_t *rt) {
    pisca_g = true;
    return true;
}

int64_t alarm_g_callback(alarm_id_t id, void *user_data) {
    alarme_g = true;
    return 0;

}

int64_t alarm_y_callback(alarm_id_t id, void *user_data) {
    alarme_y = true;
    return 0;
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

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);

    //volatile alarm_id_t alarm_g;
    //volatile alarm_id_t alarm_y;

    repeating_timer_t time_g;
    repeating_timer_t time_y;

    bool led_estado_g = false;
    bool led_estado_y = false;

    bool rodando_g = false;
    bool rodando_y = false;

    while (1) {
        if(btn_g_press && !rodando_g){
            btn_g_press = false;
            rodando_g = true;
            gpio_put(LED_PIN_G, 1);
            add_alarm_in_ms(1000, alarm_g_callback, NULL, false);
            add_repeating_timer_ms(200, timer_g_callback, NULL, &time_g);
        }
        if(pisca_g){
            pisca_g = false;
            led_estado_g = !led_estado_g;
            gpio_put(LED_PIN_G, led_estado_g);
        }
        if(alarme_g){
            alarme_g = false;
            gpio_put(LED_PIN_G, 0);
            cancel_repeating_timer(&time_g);
            rodando_g = false;
            if(rodando_y){
                rodando_y = false;
                gpio_put(LED_PIN_Y,0);
                cancel_repeating_timer(&time_y);
            }
        }

        if(btn_y_press && !rodando_y){
            rodando_y = true;
            btn_y_press = false;
            gpio_put(LED_PIN_Y, 1);
            add_alarm_in_ms(2000, alarm_y_callback, NULL, false);
            add_repeating_timer_ms(500, timer_y_callback, NULL, &time_y);
        }
        if(pisca_y){
            pisca_y = false;
            led_estado_y = !led_estado_y;
            gpio_put(LED_PIN_Y, led_estado_y);
        }
        if(alarme_y){
            alarme_y = false;
            gpio_put(LED_PIN_Y, 0);
            cancel_repeating_timer(&time_y);
            rodando_y = false;
            if(rodando_g){
                rodando_g = false;
                gpio_put(LED_PIN_G,0);
                cancel_repeating_timer(&time_g);
            }
        }

    }

    return 0;
}
