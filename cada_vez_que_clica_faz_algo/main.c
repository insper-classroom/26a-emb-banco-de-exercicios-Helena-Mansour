#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"

const int BTN_PIN_B = 15;
const int BTN_PIN_G = 16;

const int LED_PIN_B = 11;
const int LED_PIN_W = 12;
const int LED_PIN_G = 18;

volatile bool alarme_ativo = false;

volatile int estado_atual = 0;
volatile bool led_toggle = false;
bool pisca_callback(repeating_timer_t *t){
    led_toggle = !led_toggle;

    if (estado_atual == 0){
        gpio_put(LED_PIN_G,led_toggle);
        gpio_put(LED_PIN_B,0);
    } else if (estado_atual == 1){
        gpio_put(LED_PIN_B,led_toggle);
        gpio_put(LED_PIN_G,0);

    } else if (estado_atual == 2){
        gpio_put(LED_PIN_G,led_toggle);
        gpio_put(LED_PIN_B, !led_toggle);

    }
    
    return true;
}

int64_t alarme_callback(alarm_id_t id, void *user_data){
    alarme_ativo = false;
    return 0;
}

void btn_callback(uint gpio, uint32_t events){
    if (alarme_ativo){
        return;
    }

    alarme_ativo = true;
    add_alarm_in_ms(50, alarme_callback, NULL, false);

    estado_atual++;
    if (estado_atual > 3){
        estado_atual = 0;
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_put(LED_PIN_G, 0);

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);
    gpio_put(LED_PIN_B, 0);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    

    gpio_set_irq_enabled_with_callback(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    repeating_timer_t timer_semaforo;
    int estado_rodando = -1;
    

    while(1){
        if (estado_atual != estado_rodando){
            if (estado_rodando != -1){
                cancel_repeating_timer(&timer_semaforo);
            }

            led_toggle = true;

            if (estado_atual == 0){
                gpio_put(LED_PIN_G,1);
                gpio_put(LED_PIN_B,0);
                add_repeating_timer_ms(500, pisca_callback, NULL, &timer_semaforo);
            } else if (estado_atual == 1){
                gpio_put(LED_PIN_G,0);
                gpio_put(LED_PIN_B,1);
                add_repeating_timer_ms(100, pisca_callback, NULL, &timer_semaforo);
            } else if (estado_atual == 2){
                gpio_put(LED_PIN_G,1);
                gpio_put(LED_PIN_B,0);
                add_repeating_timer_ms(250, pisca_callback, NULL, &timer_semaforo);
            } else if (estado_atual == 3){
                gpio_put(LED_PIN_B,0);
                gpio_put(LED_PIN_G,0);
            }

            estado_rodando = estado_atual;
        }


    }
    
    return 0;
}