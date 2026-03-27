#include <stdio.h>
#include <string.h> 
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"


const int LED_PIN_B = 13;

const int BTN_PIN_G = 20;
const int BTN_PIN_R = 21;

volatile int btn_g = 0;
volatile int btn_r = 0;



volatile bool pisca_b = false;


void btn_callback(uint gpio, uint32_t events) {
    if (events & 0x4) {
        if (gpio == BTN_PIN_G){
            btn_g = 1;

        }
        if (gpio == BTN_PIN_R){
            btn_r = 1;

        }
        
    }
    
}

bool timer_b_callback(repeating_timer_t *rt) {
    pisca_b = 1;
    return true;
}



int main() {
    stdio_init_all();
    
    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);
    gpio_set_irq_enabled_with_callback(BTN_PIN_G, 0x4, true, &btn_callback);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);
    gpio_set_irq_enabled_with_callback(BTN_PIN_R, 0x4 , true, &btn_callback);

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);

    struct repeating_timer timer_b;

    uint32_t time_start = 0;
    uint32_t time_finish = 0;

    bool led_estado = false;

   

    while (1) {
        

        if (btn_g) {
            btn_g = 0;
            time_start = to_ms_since_boot(get_absolute_time());

            gpio_put(LED_PIN_B, 1);
            add_repeating_timer_ms(250, timer_b_callback, NULL, &timer_b);
            
        } 
        
        if (btn_r) {
            btn_r = 0;
            time_finish = to_ms_since_boot(get_absolute_time());

            

            gpio_put(LED_PIN_B, 0);
            cancel_repeating_timer(&timer_b);

            printf("Tempo: %d\n", (time_finish - time_start)/1000);
            
            
            
        }

        if(pisca_b){
            pisca_b= false;
            led_estado = !led_estado;
            gpio_put(LED_PIN_B, led_estado);
        }

    }

    return 0;
}