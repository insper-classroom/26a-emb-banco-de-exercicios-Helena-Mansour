

#include <stdio.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"

const int BTN_PIN_G = 20;

const int LED_PIN_B = 14;
const int LED_PIN_Y = 17;


//botao amarelo precionado
volatile bool btn_g_press = false;

//hora do botao piscar
volatile bool pisca_y = false;
volatile bool pisca_b = false;

//tempo acabou
volatile bool alarme= false;



void btn_callback(uint gpio, uint32_t events) {
    if(events == 0x4){ //se o botao é precionado
        if(gpio == BTN_PIN_G){
            btn_g_press = true; //marca a flag
        }
    }
}

bool timer_y_callback(repeating_timer_t *rt) {
    pisca_y = true;
    return true;

}
bool timer_b_callback(repeating_timer_t *rt) {
    pisca_b = true;
    return true;
}

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    alarme = true;
    return 0;

}


int main() {
    stdio_init_all();

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);
    gpio_set_irq_enabled_with_callback(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);

  

    repeating_timer_t time_b;
    repeating_timer_t time_y;

    bool led_estado_b = false;
    bool led_estado_y = false;

  
    while (1) {
        if(btn_g_press){ //precionou botao verde
            btn_g_press = false;

            

            gpio_put(LED_PIN_Y, 1);
            add_repeating_timer_ms(500, timer_y_callback, NULL, &time_y);
            add_alarm_in_ms(5000, alarm_callback, NULL, false);
            

            gpio_put(LED_PIN_B, 1);
            add_repeating_timer_ms(150, timer_b_callback, NULL, &time_b);
            add_alarm_in_ms(5000, alarm_callback, NULL, false);

            
            
          
        }
        if(pisca_b){
            pisca_b = false;
            led_estado_b = !led_estado_b;
            gpio_put(LED_PIN_B, led_estado_b);
        } //para piscar
        if(pisca_y){
            pisca_y = false;
            led_estado_y = !led_estado_y;
            gpio_put(LED_PIN_Y, led_estado_y);
        }

        if(alarme){
            alarme = false;
            gpio_put(LED_PIN_Y, 0);
            cancel_repeating_timer(&time_y);
        

            gpio_put(LED_PIN_B, 0);
            cancel_repeating_timer(&time_b);
          
        }

    }

    return 0;
}

