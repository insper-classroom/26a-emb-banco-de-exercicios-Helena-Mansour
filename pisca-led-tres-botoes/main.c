
#include <stdio.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

const int BTN_PIN_Y = 28;
const int BTN_PIN_B = 22;
const int BTN_PIN_G = 18;

const int LED_PIN_G = 14;
const int LED_PIN_Y = 6;
const int LED_PIN_B = 10;


//botao amarelo precionado
volatile bool btn_press_g= false;
volatile bool btn_press_y= false;
volatile bool btn_press_b= false;




void btn_callback(uint gpio, uint32_t events) {
    if(events == 0x4){ //se o botao é precionado
        if(gpio == BTN_PIN_G){
            btn_press_g = true; 
        }
        if(gpio == BTN_PIN_Y){
            btn_press_y = !btn_press_y; //marca a flag
        }
    }
    if(events == 0x8){ 
        if(gpio == BTN_PIN_G){
            btn_press_g = false; 
        }
        if(gpio == BTN_PIN_B){
            btn_press_b = !btn_press_b; //marca a flag
        }
    }
}

bool timer_y_callback(repeating_timer_t *rt) {
    return true;

}

bool timer_g_callback(repeating_timer_t *rt) {
    return true;
}

bool timer_b_callback(repeating_timer_t *rt) {
    return true;
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

    gpio_init(BTN_PIN_B);
    gpio_set_dir(BTN_PIN_B, GPIO_IN);
    gpio_pull_up(BTN_PIN_B);
    gpio_set_irq_enabled_with_callback(
    BTN_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &btn_callback);


    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);

  


    bool led_estado = false;

  
    while (1) {
        led_estado = !led_estado;

        if(btn_press_y){

            gpio_put(LED_PIN_Y, led_estado);
                   
        }else{
            gpio_put(LED_PIN_Y, 0);

        }

        if(btn_press_g){

            gpio_put(LED_PIN_G, led_estado);
                   
        }else{
            gpio_put(LED_PIN_G, 0);

        }

        if(btn_press_b){

            gpio_put(LED_PIN_B, led_estado);
                   
        }else{
            gpio_put(LED_PIN_B, 0);

        }

        sleep_ms(200);

  

     
        } 

    }



