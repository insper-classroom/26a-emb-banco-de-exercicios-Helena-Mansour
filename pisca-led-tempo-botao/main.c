
#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"

const int BTN_PIN_R = 28;

const int LED_PIN_R = 5;


//botao amarelo precionado
volatile bool btn_press= false;



//hora do botao piscar
volatile bool pisca = false;

//tempo acabou
volatile bool alarme= false;

volatile absolute_time_t tempo_inicial = 0;
volatile absolute_time_t tempo_final = 0;
volatile int64_t tempo = 0;




void btn_callback(uint gpio, uint32_t events) {
    if(events == 0x4){ //se o botao é precionado
        if(gpio == BTN_PIN_R){
            tempo_inicial = get_absolute_time();
        }
        
    }
    if(events == 0x8){ //se o botao é precionado
        if(gpio == BTN_PIN_R){
            btn_press = true; //marca a flag
            tempo_final = get_absolute_time();
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

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);
    gpio_set_irq_enabled_with_callback(
    BTN_PIN_R, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &btn_callback);


    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

  

    repeating_timer_t time;
    bool led_estado = false;


  
    while (1) {
        if(btn_press){ //precionou botao verde
            btn_press = false;

            tempo = (tempo_final - tempo_inicial)/1000;

            add_repeating_timer_ms(200, timer_callback, NULL, &time);

            add_alarm_in_ms(tempo, alarm_callback, NULL, false);
                   
        }

        if(pisca){
            pisca = false;
            led_estado = !led_estado;
            gpio_put(LED_PIN_R, led_estado);
        } 
        if(alarme){
            alarme = false;
            gpio_put(LED_PIN_R, 0);
            cancel_repeating_timer(&time);
            
        } 

           
        



     
        } 

    }



