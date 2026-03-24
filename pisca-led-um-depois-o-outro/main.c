#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"

const int BTN_PIN_B = 18;

const int BTN_PIN_Y = 19;
const int LED_PIN_B = 17;
const int LED_PIN_Y = 14;

//flag de interrupcao 
volatile bool btn_b_press = false;
volatile bool btn_y_press = false;

//flags dos timers repetitivos 
volatile bool pisca_b = false;
volatile bool pisca_y = false;

//flags setadas pelos alarmes
volatile bool alarme_b = false;
volatile bool alarme_y = false;

//impede um novo clique de botao antes de outra sequencia
volatile bool ocupado = false;

repeating_timer_t timer_b;
repeating_timer_t timer_y;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_B) {
            btn_b_press = true;
        }
        if (gpio == BTN_PIN_Y){
             btn_y_press = true;
        }
    }
}

bool timer_b_callback(repeating_timer_t *rt) {
    pisca_b = true;
    return true; //manter timer repetitivo
}

bool timer_y_callback(repeating_timer_t *rt) {
    pisca_y = true;
    return true;
}

int64_t alarm_b_callback(alarm_id_t id, void *user_data) {
    alarme_b = true;
    return 0; //return 0 não repetir
}

int64_t alarm_y_callback(alarm_id_t id, void *user_data) {
    alarme_y = true;
    return 0;
}

int main() {
    stdio_init_all();

    gpio_init(BTN_PIN_B);
    gpio_set_dir(BTN_PIN_B, GPIO_IN);
    gpio_pull_up(BTN_PIN_B);
    gpio_set_irq_enabled_with_callback(BTN_PIN_B, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_init(BTN_PIN_Y);
    gpio_set_dir(BTN_PIN_Y, GPIO_IN);
    gpio_pull_up(BTN_PIN_Y);
    gpio_set_irq_enabled_with_callback(BTN_PIN_Y, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);
    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);

    bool led_b = false;
    bool led_y = false;

    while (1) {

        //só entra se o botao azul for precionado e se nao tiver mais nenhuma sequencia
        if (btn_b_press && !ocupado) { 
            btn_b_press = false;
            ocupado = true;
            led_b = false;

            // Fase 1: LED azul pisca por 2s a 2Hz (toggle a cada 250ms)
            // Timer repetitivo a cada 250ms → toggle do LED = período de 500ms = 2Hz ✓
            add_repeating_timer_ms(250, timer_b_callback, NULL, &timer_b);
            add_alarm_in_ms(2000, alarm_b_callback, NULL, false);

            //Fica preso aqui até o alarme de 2s disparar
            while (!alarme_b) {
                if (pisca_b) {
                    pisca_b = false;
                    led_b = !led_b;
                    gpio_put(LED_PIN_B, led_b);
                }
            }
            //apaga led e cancela timer
            alarme_b = false;
            cancel_repeating_timer(&timer_b);
            gpio_put(LED_PIN_B, 0);
            led_b = false;

            // Fase 2: LED amarelo pisca por 1s a 5Hz (toggle a cada 100ms)
            //Timer repetitivo a cada 100ms → toggle do LED = período de 200ms = 5Hz ✓
            add_repeating_timer_ms(100, timer_y_callback, NULL, &timer_y);
            add_alarm_in_ms(1000, alarm_y_callback, NULL, false);

            while (!alarme_y) {
                if (pisca_y) {
                    pisca_y = false;
                    led_y = !led_y;
                    gpio_put(LED_PIN_Y, led_y);
                }
            }
            alarme_y = false;
            cancel_repeating_timer(&timer_y);
            gpio_put(LED_PIN_Y, 0);
            led_y = false;

            btn_b_press = false;
            btn_y_press = false;
            ocupado = false;
        }

        // === BOTÃO AMARELO: amarelo 1s → azul 2s ===
        if (btn_y_press && !ocupado) {
            btn_y_press = false;
            ocupado = true;
            led_y = false;

            // Fase 1: LED amarelo pisca por 1s a 5Hz (toggle a cada 100ms)
            add_repeating_timer_ms(100, timer_y_callback, NULL, &timer_y);
            add_alarm_in_ms(1000, alarm_y_callback, NULL, false);

            while (!alarme_y) {
                if (pisca_y) {
                    pisca_y = false;
                    led_y = !led_y;
                    gpio_put(LED_PIN_Y, led_y);
                }
            }
            alarme_y = false;
            cancel_repeating_timer(&timer_y);
            gpio_put(LED_PIN_Y, 0);
            led_y = false;

            // Fase 2: LED azul pisca por 2s a 2Hz (toggle a cada 250ms)
            add_repeating_timer_ms(250, timer_b_callback, NULL, &timer_b);
            add_alarm_in_ms(2000, alarm_b_callback, NULL, false);

            while (!alarme_b) {
                if (pisca_b) {
                    pisca_b = false;
                    led_b = !led_b;
                    gpio_put(LED_PIN_B, led_b);
                }
            }
            alarme_b = false;
            cancel_repeating_timer(&timer_b);
            gpio_put(LED_PIN_B, 0);
            led_b = false;

            btn_b_press = false;
            btn_y_press = false;
            ocupado = false;
        }
    }

    return 0;
}