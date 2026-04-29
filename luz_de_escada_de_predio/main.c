
#include <stdio.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"


const int BTN_PIN_G = 18;

const int LED_PIN_Y = 17;
const int LED_PIN_R = 14;


volatile bool btn_press= false;
volatile bool alarme= false;

volatile bool luz_acesa = false;

volatile bool pisca_r = false;

alarm_id_t id_alarme_luz = -1;
repeating_timer_t time_r;


int64_t alarm_callback(alarm_id_t id, void *user_data) {
    alarme = true;
    return 0;

}

bool timer_r_callback(repeating_timer_t *rt) {
    pisca_r= true;
    return true;
}


void btn_callback(uint gpio, uint32_t events) {
    if(events == 0x4){ //se o botao é precionado
        if(gpio == BTN_PIN_G){
            if (!luz_acesa){
                btn_press = true; 
                id_alarme_luz = add_alarm_in_ms(8000, alarm_callback, NULL, false);
              
            }else{

                luz_acesa = false;
                cancel_alarm(id_alarme_luz);
                gpio_put(LED_PIN_Y, 0);
                gpio_put(LED_PIN_R, 0);
                cancel_repeating_timer(&time_r);

            }
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

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);


   

    bool led_estado_r = false;
    
  
    while (1) {        
        if(btn_press){
            btn_press = false;
            luz_acesa = true;

            gpio_put(LED_PIN_Y, 1);

            gpio_put(LED_PIN_R, 1);
            add_repeating_timer_ms(400, timer_r_callback, NULL, &time_r);

        }

        if(pisca_r){
            pisca_r = false;
            led_estado_r = !led_estado_r;
            gpio_put(LED_PIN_R, led_estado_r);
        }

        if (alarme){
            alarme = false;
            luz_acesa = false; 

            gpio_put(LED_PIN_Y, 0);

            gpio_put(LED_PIN_R, 0);
            cancel_repeating_timer(&time_r);
        
        }
     

        

    }
    }



