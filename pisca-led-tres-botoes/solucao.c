#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

const int BTN_PIN_G = 20;
const int BTN_PIN_Y = 19;
const int BTN_PIN_B = 18;

const int LED_PIN_G = 15;
const int LED_PIN_Y = 14;
const int LED_PIN_B = 17;

//flag da hora que cada um deve piscar
volatile bool ativo_g = false;
volatile bool ativo_y = false;
volatile bool ativo_b = false;

// CALLBACK DOS BOTÕES
void btn_callback(uint gpio, uint32_t events){

    // Verde aperta liga e quando solta desliga
    if(gpio == BTN_PIN_G){ 
        if(events & GPIO_IRQ_EDGE_FALL){
            ativo_g = true;
        }
        if(events & GPIO_IRQ_EDGE_RISE){
            ativo_g = false;
        }
    }

    // amarelo aprtou inverte o estado
    if(gpio == BTN_PIN_Y){
        if(events & GPIO_IRQ_EDGE_FALL){
            ativo_y = !ativo_y;
        }
    }

    // azul apertou inverte o estado
    if(gpio == BTN_PIN_B){
        if(events & GPIO_IRQ_EDGE_RISE){
            ativo_b = !ativo_b;
        }
    }
}

int main() {
    stdio_init_all();

    // BOTÕES
    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_init(BTN_PIN_Y);
    gpio_set_dir(BTN_PIN_Y, GPIO_IN);
    gpio_pull_up(BTN_PIN_Y);

    gpio_init(BTN_PIN_B);
    gpio_set_dir(BTN_PIN_B, GPIO_IN);
    gpio_pull_up(BTN_PIN_B);

    // IMPORTANTE: callback em TODOS os botões
    gpio_set_irq_enabled_with_callback(
        BTN_PIN_G,
        GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
        true,
        &btn_callback
    );

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_Y,
        GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
        true,
        &btn_callback
    );

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_B,
        GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
        true,
        &btn_callback
    );

    // LEDS
    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);

    bool estado = false;

    while(1){

        // pisca global sincronizado
        estado = !estado;

        // VERDE
        if(ativo_g){
            gpio_put(LED_PIN_G, estado);
        } else {
            gpio_put(LED_PIN_G, 0);
        }

        // AMARELO
        if(ativo_y){
            gpio_put(LED_PIN_Y, estado);
        } else {
            gpio_put(LED_PIN_Y, 0);
        }

        // AZUL
        if(ativo_b){
            gpio_put(LED_PIN_B, estado);
        } else {
            gpio_put(LED_PIN_B, 0);
        }

        sleep_ms(200);
    }
}

