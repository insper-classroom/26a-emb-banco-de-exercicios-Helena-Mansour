
#include <stdio.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

const int BTN_PIN_Y = 26;
const int BTN_PIN_B = 19;

const int LED_PIN_B = 14;
const int LED_PIN_Y = 10;


volatile bool btn_press_b= false;
volatile bool btn_press_y= false;

volatile bool alarme_y= false;
volatile bool alarme_b= false;

volatile bool pisca_b = false;
volatile bool pisca_y = false;

void btn_callback(uint gpio, uint32_t events) {
    if(events == 0x4){ //se o botao é precionado
        if(gpio == BTN_PIN_B){
            btn_press_b = true; 
        }
        if(gpio == BTN_PIN_Y){
            btn_press_y = true; 
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


int64_t alarm_callback_b(alarm_id_t id, void *user_data) {
    alarme_b = true;
    return 0;

}

int64_t alarm_callback_y(alarm_id_t id, void *user_data) {
    alarme_y = true;
    return 0;

}




int main() {
    stdio_init_all();

    gpio_init(BTN_PIN_B);
    gpio_set_dir(BTN_PIN_B, GPIO_IN);
    gpio_pull_up(BTN_PIN_B);
    gpio_set_irq_enabled_with_callback(
    BTN_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_init(BTN_PIN_Y);
    gpio_set_dir(BTN_PIN_Y, GPIO_IN);
    gpio_pull_up(BTN_PIN_Y);
    gpio_set_irq_enabled_with_callback(
    BTN_PIN_Y, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);


    repeating_timer_t time_b;
    repeating_timer_t time_y;
  
    bool led_estado_y = false;
    bool led_estado_b = false;

    int b = 0;
    int y = 0;

  
    while (1) {        
        if(btn_press_y){
            y=1;
            btn_press_y = false;

            add_repeating_timer_ms(500, timer_y_callback, NULL, &time_y);
            add_alarm_in_ms(1000, alarm_callback_y, NULL, false);
         
        }
        if (alarme_y){
            alarme_y = false;
           
            gpio_put(LED_PIN_Y, 0);

            cancel_repeating_timer(&time_y);

            if (y==1){
                y=0;
                add_repeating_timer_ms(100, timer_b_callback, NULL, &time_b);
                add_alarm_in_ms(2000, alarm_callback_b, NULL, false);
            }

            
        }
        

        if(btn_press_b){
            b=1;
            
            btn_press_b = false;

            add_repeating_timer_ms(100, timer_b_callback, NULL, &time_b);
            add_alarm_in_ms(2000, alarm_callback_b, NULL, false);
         
        }
        if (alarme_b){
            alarme_b = false;
            

            gpio_put(LED_PIN_B, 0);

            cancel_repeating_timer(&time_b);

            if (b==1){
                b = 0;
                add_repeating_timer_ms(500, timer_y_callback, NULL, &time_y);
                add_alarm_in_ms(1000, alarm_callback_y, NULL, false);

            }

            

        }

        if(pisca_b){
            pisca_b = false;
            led_estado_b = !led_estado_b;
            gpio_put(LED_PIN_B, led_estado_b);
        }
        if(pisca_y){
            pisca_y = false;
            led_estado_y = !led_estado_y;
            gpio_put(LED_PIN_Y, led_estado_y);
        }


   
        

    }
    }



