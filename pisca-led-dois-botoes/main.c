#include <stdio.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"  // necessário para timers e alarmes
#include "pico/stdlib.h"

// Pinos dos botões (pull-up, ativados em nível baixo)
const int BTN_PIN_G = 28;  // botão verde
const int BTN_PIN_Y = 26;  // botão amarelo

// Pinos dos LEDs
const int LED_PIN_G = 5;   // LED verde
const int LED_PIN_Y = 9;   // LED amarelo
const int LED_PIN_R = 13;  // LED vermelho (não usado neste exercício)

// Flag que guarda qual botão foi pressionado (preenchida na interrupção)
volatile int btn_f = 0;

// Flags de disparo dos timers de piscar (setadas nos callbacks dos timers)
volatile int g_timer_g = 0;  // timer verde disparou
volatile int g_timer_y = 0;  // timer amarelo disparou
volatile int g_timer_r = 0;  // timer vermelho (não usado)

// Flags de disparo dos alarmes de tempo total (setadas nos callbacks dos alarmes)
volatile int g_fired_g = 0;  // alarme verde disparou (1000ms acabou)
volatile int g_fired_y = 0;  // alarme amarelo disparou (2000ms acabou)

// Callback da interrupção dos botões
// Detecta borda de descida (botão pressionado) e salva qual pino foi acionado
void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {       // 0x4 = GPIO_IRQ_EDGE_FALL (borda de descida)
        btn_f = gpio;          // salva o pino do botão pressionado
    } else if (events == 0x8) { // 0x8 = GPIO_IRQ_EDGE_RISE (borda de subida, ignorada)
    }
}

// Callback do timer de piscar do LED verde (dispara a cada 200ms)
// Apenas seta a flag — o toggle do LED acontece no loop principal
bool timer_g_callback(repeating_timer_t *rt) {
    g_timer_g = 1;
    return true;  // retorna true para continuar repetindo
}

// Callback do timer de piscar do LED amarelo (dispara a cada 500ms)
bool timer_y_callback(repeating_timer_t *rt) {
    g_timer_y = 1;
    return true;  // retorna true para continuar repetindo
}

// Callback do timer vermelho (não usado neste exercício)
bool timer_r_callback(repeating_timer_t *rt) {
    g_timer_r = 1;
    return true;
}

// Callback do alarme do LED verde (dispara uma vez após 1000ms)
// Seta flag indicando que o tempo total do verde acabou
int64_t alarm_g_callback(alarm_id_t id, void *user_data) {
    g_fired_g = 1;
    return 0;  // retorna 0 para não reagendar o alarme
}

// Callback do alarme do LED amarelo (dispara uma vez após 2000ms)
// Seta flag indicando que o tempo total do amarelo acabou
int64_t alarm_y_callback(alarm_id_t id, void *user_data) {
    g_fired_y = 1;
    return 0;  // retorna 0 para não reagendar o alarme
}

int main() {
    stdio_init_all();  // inicializa comunicação serial (usado nos printf de erro)

    // Configura botão verde: entrada com pull-up + interrupção na borda de descida
    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);
    gpio_set_irq_enabled_with_callback(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    // Configura botão amarelo: entrada com pull-up + interrupção na borda de descida
    gpio_init(BTN_PIN_Y);
    gpio_set_dir(BTN_PIN_Y, GPIO_IN);
    gpio_pull_up(BTN_PIN_Y);
    gpio_set_irq_enabled_with_callback(BTN_PIN_Y, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    // Configura LEDs como saída
    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    // Inicia timer de piscar do LED verde: dispara a cada 200ms (f = 5Hz)
    // Fica rodando sempre — o LED só pisca quando alarm_enable_g == 1
    repeating_timer_t timer_g;
    if (!add_repeating_timer_ms(200, timer_g_callback, NULL, &timer_g)) {
        printf("Failed to add timer\n");
    }

    // Inicia timer de piscar do LED amarelo: dispara a cada 500ms (f = 2Hz)
    repeating_timer_t timer_y;
    if (!add_repeating_timer_ms(500, timer_y_callback, NULL, &timer_y)) {
        printf("Failed to add timer\n");
    }

    // Estado atual dos LEDs (0 = apagado, 1 = aceso)
    int led_g = 0;
    int led_y = 0;

    // Habilitação dos LEDs (0 = desabilitado, 1 = piscando)
    int alarm_enable_g = 0;
    int alarm_enable_y = 0;

    // IDs dos alarmes (necessários para cancelar se precisar)
    alarm_id_t alarm_g = 0;
    alarm_id_t alarm_y = 0;

    while (1) {
        // Se o timer verde disparou E o LED verde está habilitado → faz toggle
        if (g_timer_g && alarm_enable_g) {
            led_g = !led_g;
            gpio_put(LED_PIN_G, led_g);
            g_timer_g = 0;  // limpa a flag
        } else if (alarm_enable_g == 0) {
            // Se LED verde desabilitado → garante que fica apagado
            gpio_put(LED_PIN_G, 0);
        }

        // Se o timer amarelo disparou E o LED amarelo está habilitado → faz toggle
        if (g_timer_y && alarm_enable_y) {
            led_y = !led_y;
            gpio_put(LED_PIN_Y, led_y);
            g_timer_y = 0;  // limpa a flag
        } else if (alarm_enable_y == 0) {
            // Se LED amarelo desabilitado → garante que fica apagado
            gpio_put(LED_PIN_Y, 0);
        }

        // Se botão verde foi pressionado e LED verde está parado → inicia
        if (btn_f == BTN_PIN_G) {
            if (alarm_enable_g == 0) {
                // Cria alarme de 1000ms — após esse tempo o LED verde para
                alarm_g = add_alarm_in_ms(1000, alarm_g_callback, NULL, false);
                alarm_enable_g = 1;  // habilita o piscar do LED verde
            }
            btn_f = 0;  // limpa a flag do botão
        }

        // Se botão amarelo foi pressionado e LED amarelo está parado → inicia
        if (btn_f == BTN_PIN_Y) {
            if (alarm_enable_y == 0) {
                // Cria alarme de 2000ms — após esse tempo o LED amarelo para
                alarm_y = add_alarm_in_ms(2000, alarm_y_callback, NULL, false);
                alarm_enable_y = 1;  // habilita o piscar do LED amarelo
            }
            btn_f = 0;  // limpa a flag do botão
        }

        // Regra especial: se qualquer alarme disparou (um LED terminou o tempo)
        // ambos os LEDs devem parar imediatamente
        if (g_fired_g == 1 || g_fired_y == 1) {
            g_fired_g = 0;
            alarm_enable_g = 0;  // desabilita LED verde

            g_fired_y = 0;
            alarm_enable_y = 0;  // desabilita LED amarelo

            cancel_alarm(alarm_y);  // cancela alarme amarelo (caso ainda esteja ativo)
            cancel_alarm(alarm_g);  // cancela alarme verde (caso ainda esteja ativo)
        }
    }

    return 0;
}