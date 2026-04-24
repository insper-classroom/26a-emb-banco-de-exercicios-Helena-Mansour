
#include <stdio.h>


#include "hardware/gpio.h"
#include "pico/stdlib.h"


const int BTN_PIN = 28;
const int LED_PIN = 5;


volatile bool btn_press= false;

volatile bool alarme= false;

volatile bool pisca= false;

volatile int tempo_inicial = 0;
volatile int tempo_final = 0;
volatile int tempo= 0;



void btn_callback(uint gpio, uint32_t events) {
    if(events == 0x4){ //se o botao é precionado
        if(gpio == BTN_PIN){
            tempo_inicial = get_absolute_time();
        }
    }

    if(events == 0x8){ //se o botao é solto
        if(gpio == BTN_PIN){
            tempo_final = get_absolute_time();
            btn_press = true; 
        }
    }
}

bool timer_callback(repeating_timer_t *rt) {
    pisca = true;
    return true;

}



int64_t alarm_callback(alarm_id_t id, void *user_data) {
    alarme = true;
    return 0;

}



int main() {
    stdio_init_all();

    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);
    gpio_set_irq_enabled_with_callback(
    BTN_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

  

    repeating_timer_t time;

    bool led_estado = false;
  

  
    while (1) {        
        if(btn_press){
            btn_press = false;

            tempo = absolute_time_diff_us(tempo_inicial, tempo_final)/1000;

            add_repeating_timer_ms(200, timer_callback, NULL, &time);


            add_alarm_in_ms(tempo, alarm_callback, NULL, false);
         
        }
        if (alarme){
            alarme = false;

            gpio_put(LED_PIN, 0);
            cancel_repeating_timer(&time);
        
        }
        if(pisca){
            pisca = false;
            led_estado = !led_estado;
            gpio_put(LED_PIN, led_estado);
        }
       

        

    }
    }



