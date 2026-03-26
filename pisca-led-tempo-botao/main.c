#include <stdio.h>

#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"

const int BTN_PIN= 28;

const int LED_PIN= 5;



volatile int btn_press = 0;


//hora do botao piscar
volatile bool pisca = false;


//tempo acabou
volatile bool alarme= false;




void btn_callback(uint gpio, uint32_t events) {
    if(events == 0x4){ //se o botao é precionado
        if(gpio == BTN_PIN){
            btn_press = 1; //marca a flag
        }
        
    }
    if (events == 0x8){
        if (gpio == BTN_PIN){
            btn_press = 2;
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
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

   

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);


  

    repeating_timer_t time;
  
    bool led_estado = false;

    absolute_time_t tempo_inicial;
    

  
    while (1) {
        if(btn_press == 1){ //precionou botao verde
            btn_press = 0;
            gpio_put(LED_PIN, 0);            // apaga LED enquanto pressiona
            cancel_repeating_timer(&time);  // cancela pisca anterior se houver
            alarme = false;                  // reseta alarme pendente
            tempo_inicial = get_absolute_time();           

                   
        }

        if (btn_press == 2){
            btn_press = 0;

            int64_t tempo_alarme = absolute_time_diff_us(tempo_inicial, get_absolute_time()) / 1000;


            cancel_repeating_timer(&time);  // garante que não há timer duplicado
            alarme = false;


            add_repeating_timer_ms(200, timer_callback, NULL, &time);
            add_alarm_in_ms(tempo_alarme, alarm_callback, NULL, false);

        }
        if(pisca){
            pisca= false;
            led_estado = !led_estado;
            gpio_put(LED_PIN, led_estado);
        } 
        if(alarme){
            alarme = false;
            gpio_put(LED_PIN, 0);
            cancel_repeating_timer(&time);
        

            
        } 

    }

}

