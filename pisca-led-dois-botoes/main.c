#include <stdio.h>              
#include "hardware/gpio.h"     
#include "pico/stdlib.h"       


// LEDs 
const int LED_amarelo = 14;
const int LED_verde = 15;

// Botões 
const int BTN_amarelo = 19;
const int BTN_verde = 20;


volatile bool button_flag_a = false;
volatile bool button_flag_v = false;

struct repeating_timer timer_v;
struct repeating_timer timer_a;

//para definir tempo que led fica piscando
volatile bool verde_ativo = false;
volatile bool amarelo_ativo = false;

volatile int tempo_verde = 0;
volatile int tempo_amarelo = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {
        if (gpio == BTN_amarelo){
            button_flag_a = true;
        }
        if (gpio == BTN_verde){
            button_flag_v = true;
        }
    }
}

bool timer_callback_a(struct repeating_timer *t) {

    gpio_put(LED_amarelo, !gpio_get_out_level(LED_amarelo));

    tempo_amarelo += 500;

    if (tempo_amarelo >= 2000) {

        gpio_put(LED_amarelo, 0);

        amarelo_ativo = false;
        tempo_amarelo = 0;

        // quando um acaba os dois vao cabar
        if (verde_ativo) {
            cancel_repeating_timer(&timer_v); 
            gpio_put(LED_verde, 0);
            verde_ativo = false;
            tempo_verde = 0;
        }

        return false;
    }

    return true;
}

bool timer_callback_v(struct repeating_timer *t) {

    gpio_put(LED_verde, !gpio_get_out_level(LED_verde));

    tempo_verde += 200;

    if (tempo_verde >= 1000) {

        gpio_put(LED_verde, 0);
        

        verde_ativo = false;
        tempo_verde = 0;

        // quando um acaba os dois acabam
        if (amarelo_ativo) {
            cancel_repeating_timer(&timer_a);
            gpio_put(LED_amarelo, 0);
            amarelo_ativo = false;
            tempo_amarelo = 0;
        }

        return false;
    }

    return true;
}



int main(){
    stdio_init_all();

 

    gpio_init(LED_amarelo);
    gpio_set_dir(LED_amarelo, GPIO_OUT);
    gpio_put(LED_amarelo, 0);

    gpio_init(LED_verde);
    gpio_set_dir(LED_verde, GPIO_OUT);
    gpio_put(LED_verde, 0);

    gpio_init(BTN_verde);
    gpio_set_dir(BTN_verde, GPIO_IN);
    gpio_pull_up(BTN_verde);

    gpio_init(BTN_amarelo);
    gpio_set_dir(BTN_amarelo, GPIO_IN);
    gpio_pull_up(BTN_amarelo);

    gpio_set_irq_enabled_with_callback(BTN_amarelo, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    gpio_set_irq_enabled(BTN_verde, GPIO_IRQ_EDGE_FALL, true);


    while (true) {
        if (button_flag_v){
            button_flag_v = false;
        
            if (!verde_ativo){
                tempo_verde = 0;

                gpio_put(LED_verde, 1);

                add_repeating_timer_ms(
                        200,
                        timer_callback_v,
                        NULL,
                        &timer_v
                    );
                    verde_ativo = true;
            }

        }
        if (button_flag_a){
            button_flag_a = false;
        
            if (!amarelo_ativo){
                tempo_amarelo = 0;

                gpio_put(LED_amarelo, 1);

                add_repeating_timer_ms(
                        500,
                        timer_callback_a,
                        NULL,
                        &timer_a
                    );
                    amarelo_ativo = true;
            }

        }
}
}