
#include <stdio.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

const int BTN_PIN_B = 20;

const int LED_PIN_B = 17;



volatile bool btn_press_b= false;


volatile bool alarme_b= false;
volatile bool led_estado_b = false;
volatile alarm_id_t id_alarme = -1;



volatile int estado = 0;

void btn_callback(uint gpio, uint32_t events) {
    if(events == 0x4){ //se o botao é precionado
        if(gpio == BTN_PIN_B){
            btn_press_b = true;
            if (estado != 3){
                estado += 1; 

            }else{
                estado = 0;
                gpio_put(LED_PIN_B, 0);

            }
            
        }

       
    }
}



bool timer_b_callback(repeating_timer_t *rt) {
    led_estado_b = !led_estado_b;
    gpio_put(LED_PIN_B, led_estado_b);
    return true;
}


int64_t alarm_callback_b(alarm_id_t id, void *user_data) {
    alarme_b = true;
    return 0;

}





int main() {
    stdio_init_all();

    gpio_init(BTN_PIN_B);
    gpio_set_dir(BTN_PIN_B, GPIO_IN);
    gpio_pull_up(BTN_PIN_B);
    gpio_set_irq_enabled_with_callback(
    BTN_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &btn_callback);



    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);



    repeating_timer_t time_b;
  
    

  
  
    while (1) {        


        if(btn_press_b){
            btn_press_b = false;

            if (estado == 1){

                add_repeating_timer_ms(250, timer_b_callback, NULL, &time_b);
                id_alarme = add_alarm_in_ms(5000, alarm_callback_b, NULL, false);

            }
            if (estado == 2){
                estado = 3;

                gpio_put(LED_PIN_B, 0);
                cancel_repeating_timer(&time_b);
                cancel_alarm(id_alarme);
            }

            
         
        }
        if (alarme_b){

            estado = 3;

            alarme_b = false;
            cancel_repeating_timer(&time_b);
            gpio_put(LED_PIN_B, 1);
            
        

        }

  

        }
    }



