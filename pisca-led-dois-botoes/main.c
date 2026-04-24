
#include <stdio.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

const int BTN_PIN_Y = 19;
const int BTN_PIN_G = 18;

const int LED_PIN_G = 16;
const int LED_PIN_Y = 15;


volatile bool btn_press_g= false;
volatile bool btn_press_y= false;


void btn_callback(uint gpio, uint32_t events) {
    if(events == 0x4){ //se o botao é precionado
        if(gpio == BTN_PIN_G){
            btn_press_g = true; 
        }
        if(gpio == BTN_PIN_Y){
            btn_press_y = true; 
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

    gpio_init(BTN_PIN_Y);
    gpio_set_dir(BTN_PIN_Y, GPIO_IN);
    gpio_pull_up(BTN_PIN_Y);
    gpio_set_irq_enabled_with_callback(
    BTN_PIN_Y, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);


    repeating_timer_t time_g;
    repeating_timer_t time_y;
  
    bool led_estado_y = false;
    bool led_estado_g = false;

  
    while (1) {        
        if(btn_press_g){

            btn_press_g = false;

            gpio_put(LED_PIN_G, 1);

         
        }

        if(btn_press_y){

            btn_press_y = false;
            gpio_put(LED_PIN_Y, 1);

         
        }

        

    }
    }



