#include <stdio.h>
#include "hardware/gpio.h"
#include "pico/stdlib.h"

const int BTN_PIN_G = 18;
const int LED_PIN_R = 14;

volatile bool btn_press = false;
volatile bool alarme = false;
volatile bool pisca_r = false;
volatile bool freio_acionado = false;
volatile alarm_id_t id_alarme = -1;

repeating_timer_t time_r;

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    alarme = true;
    return 0;
}

bool timer_r_callback(repeating_timer_t *rt) {
    pisca_r = true;
    return true;
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_G) {
            btn_press = true;
        }
    }
}

int main() {
    stdio_init_all();

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);
    gpio_set_irq_enabled_with_callback(
        BTN_PIN_G, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    // Modo Viagem: inicia piscando automaticamente
    bool led_estado_r = false;
    add_repeating_timer_ms(500, timer_r_callback, NULL, &time_r);

    while (1) {
        if (btn_press) {
            btn_press = false;

            if (!freio_acionado) {
                // Primeira frenagem: para o piscar, acende fixo
                freio_acionado = true;
                cancel_repeating_timer(&time_r);
                gpio_put(LED_PIN_R, 1);
                id_alarme = add_alarm_in_ms(4000, alarm_callback, NULL, false);

            } else {
                // Frenagem contínua: renova o alarme
                cancel_alarm(id_alarme);
                id_alarme = add_alarm_in_ms(4000, alarm_callback, NULL, false);
            }
        }

        if (pisca_r) {
            pisca_r = false;
            led_estado_r = !led_estado_r;
            gpio_put(LED_PIN_R, led_estado_r);
        }

        if (alarme) {
            // Retorno ao Normal: volta a piscar
            alarme = false;
            freio_acionado = false;
            gpio_put(LED_PIN_R, 0);
            add_repeating_timer_ms(500, timer_r_callback, NULL, &time_r);
        }
    }
}