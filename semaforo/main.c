#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"

const int BTN_PIN_G = 20;


const int LED_PIN_Y = 14;
const int LED_PIN_G = 15;
const int LED_PIN_R = 16;

volatile bool btn_press_g = false;


volatile bool alarme_y = false;
volatile bool alarme_g = false;
volatile bool alarme_r = false;


bool encadeado_y = false;
bool encadeado_g = false;
bool encadeado_r = false;


void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_G) btn_press_g = true;
    }
}


int64_t alarm_y_callback(alarm_id_t id, void *user_data) {
    alarme_y = true;
    return 0;
}

int64_t alarm_g_callback(alarm_id_t id, void *user_data) {
    alarme_g = true;
    return 0;
}

int64_t alarm_r_callback(alarm_id_t id, void *user_data) {
    alarme_r = true;
    return 0;
}

int main() {
    stdio_init_all();

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);
    gpio_set_irq_enabled_with_callback(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true, &btn_callback);


    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);

    

    while (1) {

        // === BOTÃO AMARELO: inicia LED amarelo (5Hz, 1s) ===
        if (btn_press_g) {
            btn_press_g = false;
            encadeado_g = false; 
            gpio_put(LED_PIN_G, 1);
            add_alarm_in_ms(2000, alarm_g_callback, NULL, false);
        }


        if (alarme_g) {
            alarme_g = false;
            gpio_put(LED_PIN_G, 0);

            if (!encadeado_g) {
                // amarelo era o primeiro → encadeia azul
                encadeado_y = true;
                gpio_put(LED_PIN_Y, 1);
                add_alarm_in_ms(1000, alarm_y_callback, NULL, false);
            }
            // se encadeado_y == true, amarelo era o segundo → para tudo
        }

        if (alarme_y) {
            alarme_y = false;
            gpio_put(LED_PIN_Y, 0);

            if (!encadeado_y) {                
                gpio_put(LED_PIN_R, 1);
                add_alarm_in_ms(2000, alarm_r_callback, NULL, false);
            }
            // se encadeado_y == true, amarelo era o segundo → para tudo
        }


        if (alarme_r) {
            alarme_r = false;
            gpio_put(LED_PIN_R, 0);
        }
    }
}