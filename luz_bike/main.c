#include <stdio.h>
#include "hardware/gpio.h"
#include "pico/stdlib.h"

const int BTN_PIN_G = 18;
const int LED_PIN_R = 14;
const int LED_PIN_B = 17;

volatile int btn_press = 1;
volatile bool alarme = false;
volatile bool pisca_r = false;

repeating_timer_t time_r;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_G) {
            btn_press += 1;
            if (btn_press > 3) btn_press = 1; // reseta o ciclo
        }
    }
}

bool timer_r_callback(repeating_timer_t *rt) {
    pisca_r = true;
    return true;
}

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    alarme = true;
    return 0;
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

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);

    bool led_estado_r = false;
    int btn_rodando = 0; // << controla se o setup já rodou

    while (1) {

        // Alarme estourou: volta pro estado 1
        if (alarme) {
            alarme = false;
            btn_press = 2;
        }

        // Só entra no setup quando o estado MUDOU
        if (btn_press != btn_rodando) {
            btn_rodando = btn_press;

            // Limpeza antes de qualquer setup
            cancel_repeating_timer(&time_r);
            gpio_put(LED_PIN_B, 0);
            gpio_put(LED_PIN_R, 0);
            led_estado_r = false;

            if (btn_press == 1) {
                gpio_put(LED_PIN_B, 1);
            }

            if (btn_press == 2) {
                add_repeating_timer_ms(1000, timer_r_callback, NULL, &time_r);
            }

            if (btn_press == 3) {
                add_repeating_timer_ms(100, timer_r_callback, NULL, &time_r);
                add_alarm_in_ms(5000, alarm_callback, NULL, false);
            }
        }

        if (pisca_r) {
            pisca_r = false;
            led_estado_r = !led_estado_r;
            gpio_put(LED_PIN_R, led_estado_r);
        }
    }
}