#include <stdio.h>
#include <string.h> 
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"

const int LED_PIN_R = 16;
const int BTN_PIN_R = 21;

// Sinaliza pro main que o botão foi solto e o timer pode começar
volatile bool button_flag = false;

struct repeating_timer timer;

// Acumula o tempo decorrido durante o piscar
volatile int tempo = 0;

// Guarda o instante em que o botão foi pressionado
volatile absolute_time_t press_start;

// "Contabilizar o tempo que o botão vermelho se mantém pressionado"
volatile int32_t press_duration_ms = 0;


// "Deve trabalhar com interrupções nos botões"
void btn_callback(uint gpio, uint32_t events) {

    // Botão pressionado (FALL = nível caiu de 1 para 0, pois usa pull-up)
    if (events & GPIO_IRQ_EDGE_FALL) {

        // Marca o instante inicial para medir o tempo pressionado
        press_start = get_absolute_time();

        // "O LED deve apagar sempre que o botão estiver pressionado"
        gpio_put(LED_PIN_R, 0);

        // Interrompe qualquer piscar em andamento enquanto botão está segurado
        // "O LED não deve piscar enquanto o botão estiver sendo pressionado"
        cancel_repeating_timer(&timer);
        tempo = 0;
    }

    // Botão solto (RISE = nível subiu de 0 para 1)
    if (events & GPIO_IRQ_EDGE_RISE) {

        // "Contabilizar o tempo que o botão vermelho se mantém pressionado"
        press_duration_ms = absolute_time_diff_us(press_start, get_absolute_time()) / 1000;

        // Avisa o main que tem um novo tempo medido e pode iniciar o piscar
        button_flag = true;
    }
}


// "Deve trabalhar com TIMER" — dispara a cada 200ms
bool timer_callback(struct repeating_timer *t) {

    // "Manter o LED piscando" — inverte o estado atual do LED
    gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));

    // Acumula o tempo já piscado
    tempo += 200;

    // "Manter o LED piscando com o tempo que o botão foi pressionado"
    // Quando o tempo piscando atingir o tempo que o botão ficou pressionado, para
    if (tempo >= press_duration_ms && press_duration_ms > 0) {
        gpio_put(LED_PIN_R, 0);
        tempo = 0;
        return false;
    }


    // Continua piscando
    return true;
}


int main() {
    stdio_init_all();

    // Configura botão com pull-up (lê 1 em repouso, 0 quando pressionado)
    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    // Configura LED como saída, começa apagado
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);

    // "Deve trabalhar com interrupções nos botões"
    // Registra a interrupção para detectar pressionar (FALL) e soltar (RISE)
    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R,
        GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
        true,
        &btn_callback
    );

    while (true) {

        // Quando o botão foi solto, button_flag é true
        // "Sempre que o botão vermelho for pressionado, o tempo de pisca deve ser atualizado"
        if (button_flag) {
            button_flag = false;

            // Garante que não há timer anterior rodando antes de iniciar novo
            cancel_repeating_timer(&timer);
            tempo = 0;

            // Acende imediatamente ao soltar o botão e começa a piscar
            gpio_put(LED_PIN_R, 1);

            // "Não é permitido usar sleep_ms()/sleep_us()" — usa TIMER no lugar
            // Dispara timer_callback a cada 200ms
            add_repeating_timer_ms(200, timer_callback, NULL, &timer);
        }
    }
}