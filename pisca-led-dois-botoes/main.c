#include <stdio.h>              
#include "hardware/gpio.h"     
#include "pico/stdlib.h"       

// ─── Pinos ───────────────────────────────────────────────────────────────────

// "LED Verde: pisca a cada 200ms" — pino 15
const int LED_amarelo = 14;
const int LED_verde   = 15;

// "Botão Verde → inicia o piscar do LED Verde" — pinos dos botões
const int BTN_amarelo = 19;
const int BTN_verde   = 20;

// ─── Flags de botão ──────────────────────────────────────────────────────────
// volatile: avisa o compilador que essas variáveis podem mudar fora do fluxo
// normal (dentro de interrupções), impedindo otimizações incorretas
volatile bool button_flag_a = false;
volatile bool button_flag_v = false;

// ─── Estruturas dos timers ───────────────────────────────────────────────────
// Um timer para cada LED, pois cada um tem frequência diferente:
// "LED Verde: pisca a cada 200ms / LED Amarelo: pisca a cada 500ms"
struct repeating_timer timer_v;
struct repeating_timer timer_a;

// ─── Estado dos LEDs ─────────────────────────────────────────────────────────
// Controla se cada LED está no seu período ativo de piscar:
// "LED Verde: 1000ms / LED Amarelo: 2000ms"
volatile bool verde_ativo   = false;
volatile bool amarelo_ativo = false;

// Acumula o tempo decorrido para saber quando parar de piscar
volatile int tempo_verde   = 0;
volatile int tempo_amarelo = 0;

// ─── Interrupção dos botões ──────────────────────────────────────────────────
// "Deve trabalhar com interrupções nos botões"
// Chamada automaticamente pelo hardware quando um botão é pressionado.
// Apenas levanta uma flag — o processamento real acontece no main,
// pois interrupções devem ser curtas e rápidas.
void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {  // borda de descida = botão pressionado
        if (gpio == BTN_amarelo) button_flag_a = true;
        if (gpio == BTN_verde)   button_flag_v = true;
    }
}

// ─── Timer do LED Amarelo ────────────────────────────────────────────────────
// "Utilizar timers para o controle de tempo" — chamado a cada 500ms pelo SDK.
// Cada chamada inverte o estado do LED (piscar) e acumula o tempo decorrido.
bool timer_callback_a(struct repeating_timer *t) {

    // Inverte o estado do LED → produz o piscar de 500ms
    gpio_put(LED_amarelo, !gpio_get_out_level(LED_amarelo));

    tempo_amarelo += 500;  // acumula 500ms a cada disparo do timer

    // "LED Amarelo: permanece ativo por 2000ms"
    if (tempo_amarelo >= 2000) {

        // "Os LEDs devem parar de piscar apagados"
        gpio_put(LED_amarelo, 0);
        amarelo_ativo = false;
        tempo_amarelo = 0;

        // "Se dois LEDs estiverem piscando simultaneamente, quando o tempo
        //  de um deles terminar, ambos devem parar imediatamente"
        if (verde_ativo) {
            cancel_repeating_timer(&timer_v);  // para o timer do verde
            gpio_put(LED_verde, 0);            // apaga o verde
            verde_ativo  = false;
            tempo_verde  = 0;
        }

        return false;  // retornar false cancela este timer automaticamente
    }

    return true;  // continua repetindo
}

// ─── Timer do LED Verde ──────────────────────────────────────────────────────
// Mesma lógica do amarelo, mas com "200ms de período e 1000ms de duração"
bool timer_callback_v(struct repeating_timer *t) {

    // Inverte o estado do LED → produz o piscar de 200ms
    gpio_put(LED_verde, !gpio_get_out_level(LED_verde));

    tempo_verde += 200;  // acumula 200ms a cada disparo do timer

    // "LED Verde: permanece ativo por 1000ms"
    if (tempo_verde >= 1000) {

        // "Os LEDs devem parar de piscar apagados"
        gpio_put(LED_verde, 0);
        verde_ativo = false;
        tempo_verde = 0;

        // "Se dois LEDs estiverem piscando simultaneamente, quando o tempo
        //  de um deles terminar, ambos devem parar imediatamente"
        if (amarelo_ativo) {
            cancel_repeating_timer(&timer_a);  // para o timer do amarelo
            gpio_put(LED_amarelo, 0);          // apaga o amarelo
            amarelo_ativo = false;
            tempo_amarelo = 0;
        }

        return false;  // retornar false cancela este timer automaticamente
    }

    return true;  // continua repetindo
}

// ─── Main ────────────────────────────────────────────────────────────────────
int main() {
    stdio_init_all();

    // Configura LEDs como saída, iniciando apagados
    gpio_init(LED_amarelo);
    gpio_set_dir(LED_amarelo, GPIO_OUT);
    gpio_put(LED_amarelo, 0);

    gpio_init(LED_verde);
    gpio_set_dir(LED_verde, GPIO_OUT);
    gpio_put(LED_verde, 0);

    // Configura botões como entrada com pull-up interno
    // (sem pull-up o pino flutuaria entre 0 e 1 aleatoriamente)
    gpio_init(BTN_verde);
    gpio_set_dir(BTN_verde, GPIO_IN);
    gpio_pull_up(BTN_verde);

    gpio_init(BTN_amarelo);
    gpio_set_dir(BTN_amarelo, GPIO_IN);
    gpio_pull_up(BTN_amarelo);

    // "Deve trabalhar com interrupções nos botões"
    // Registra btn_callback para ser chamado em borda de descida dos botões
    gpio_set_irq_enabled_with_callback(BTN_amarelo, GPIO_IRQ_EDGE_FALL,
                                       true, &btn_callback);
    gpio_set_irq_enabled(BTN_verde, GPIO_IRQ_EDGE_FALL, true);

    // ─── Loop principal ───────────────────────────────────────────────────
    // "Baremetal (sem RTOS)" — o main fica em loop verificando as flags
    // levantadas pelas interrupções dos botões
    while (true) {

        // "Botão Verde → inicia o piscar do LED Verde"
        if (button_flag_v) {
            button_flag_v = false;

            if (!verde_ativo) {  // só inicia se não estiver piscando já
                tempo_verde = 0;
                verde_ativo = true;
                // Inicia timer que dispara a cada 200ms para piscar o LED
                add_repeating_timer_ms(200, timer_callback_v, NULL, &timer_v);
            }
        }

        // "Botão Amarelo → inicia o piscar do LED Amarelo"
        if (button_flag_a) {
            button_flag_a = false;

            if (!amarelo_ativo) {  // só inicia se não estiver piscando já
                tempo_amarelo = 0;
                amarelo_ativo = true;
                // Inicia timer que dispara a cada 500ms para piscar o LED
                add_repeating_timer_ms(500, timer_callback_a, NULL, &timer_a);
            }
        }
    }
}