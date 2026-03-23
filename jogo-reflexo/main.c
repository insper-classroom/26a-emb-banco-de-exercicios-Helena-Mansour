#include <stdio.h>
#include "hardware/gpio.h"
#include "pico/stdlib.h"

// Pinos dos LEDs
const int LED_vermelho = 16;
const int LED_amarelo = 14;
const int LED_verde = 15;

// Pinos dos botões
const int BTN_vermelho = 21;
const int BTN_amarelo = 19;
const int BTN_verde = 20;

// Arrays (mantidos)
const int LEDS[3] = {LED_amarelo, LED_verde, LED_vermelho};
const int BTNS[3] = {BTN_amarelo, BTN_verde, BTN_vermelho};

// Sequência correta
const int sequencia[10] = {0,1,2, 0,1,0, 2,0,1,0};

volatile int btn_pressionado = -1;

// ✅ CORREÇÃO PRINCIPAL (sem loop na ISR)
void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {

        if (gpio == BTN_amarelo) btn_pressionado = 0;
        else if (gpio == BTN_verde) btn_pressionado = 1;
        else if (gpio == BTN_vermelho) btn_pressionado = 2;
    }
}

void apaga_todos() {
    for (int i = 0; i < 3; i++) gpio_put(LEDS[i], 0);
}

void acende_led(int idx) {
    gpio_put(LEDS[idx], 1);
    sleep_ms(300);
    gpio_put(LEDS[idx], 0);
    sleep_ms(300);
}

void mostra_sequencia(int rodada) {
    sleep_ms(500);
    for (int i = 0; i < rodada; i++) {
        acende_led(sequencia[i]);
    }
}

int main() {
    stdio_init_all();

    for (int i = 0; i < 3; i++) {
        gpio_init(LEDS[i]);
        gpio_set_dir(LEDS[i], GPIO_OUT);
        gpio_put(LEDS[i], 0);
    }

    for (int i = 0; i < 3; i++) {
        gpio_init(BTNS[i]);
        gpio_set_dir(BTNS[i], GPIO_IN);
        gpio_pull_up(BTNS[i]);
    }

    gpio_set_irq_enabled_with_callback(BTNS[0], GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    for (int i = 1; i < 3; i++) {
        gpio_set_irq_enabled(BTNS[i], GPIO_IRQ_EDGE_FALL, true);
    }

    // Espera botão verde iniciar
    while (1) {
        if (btn_pressionado == 1) {
            btn_pressionado = -1;
            break;
        }
        sleep_ms(10);
    }

    int pnts = 0;

    for (int rodada = 1; rodada <= 10; rodada++) {
        mostra_sequencia(rodada);

        int acertou = 1;

        for (int i = 0; i < rodada; i++) {

            btn_pressionado = -1;

            while (btn_pressionado == -1) {
                tight_loop_contents();
            }

            int b = btn_pressionado;
            btn_pressionado = -1;

            if (b != sequencia[i]) {
                acertou = 0;
                break;
            }

            acende_led(b);
        }

        if (!acertou) break;

        pnts = rodada;
    }

    printf("Points %d\n", pnts);

    for (int i = 0; i < pnts; i++) {
        for (int j = 0; j < 3; j++) gpio_put(LEDS[j], 1);
        sleep_ms(200);
        apaga_todos();
        sleep_ms(200);
    }

    while (1) {}
}