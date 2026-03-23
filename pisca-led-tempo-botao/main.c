#include <stdio.h>
#include <string.h> 
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"


const int LED_PIN_R  = 16;
const int BTN_PIN_R = 21;

volatile bool button_flag = false;
struct repeating_timer timer;

volatile int tempo = 0;
volatile absolute_time_t press_start;
volatile int32_t press_duration_ms = 0;





void btn_callback(uint gpio, uint32_t events) {

    if (events & GPIO_IRQ_EDGE_FALL) {
       
        press_start = get_absolute_time();
        gpio_put(LED_PIN_R, 0);
        cancel_repeating_timer(&timer); 
        tempo = 0;
    }
 
    if (events & GPIO_IRQ_EDGE_RISE) {
       

    // calcula quanto tempo ficou pressionado
    press_duration_ms = absolute_time_diff_us(press_start, get_absolute_time()) / 1000;
    
    button_flag = true;  // sinaliza pro main começar a piscar
}

}


bool timer_callback(struct repeating_timer *t) {

    // Inverte o estado do LED → produz o piscar de 200ms
    gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));

    tempo += 200;  // acumula 200ms a cada disparo do timer

    // "LED Verde: permanece ativo por 1000ms"
    if (tempo >= press_duration_ms) {

        // "Os LEDs devem parar de piscar apagados"
        gpio_put(LED_PIN_R, 0);
        tempo = 0;

        return false;   // ← adiciona isso para parar o timer

    }

    return true;  // continua repetindo
}

int main() {

    stdio_init_all();

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);


    gpio_set_irq_enabled_with_callback(
    BTN_PIN_R, 
    GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
    true,
    &btn_callback
);

    while (true) {
    if (button_flag) {
        button_flag = false;
        cancel_repeating_timer(&timer);
        tempo = 0;
        gpio_put(LED_PIN_R, 1);
        add_repeating_timer_ms(200, timer_callback, NULL, &timer);  // ← faltou isso
    }
}
    }



