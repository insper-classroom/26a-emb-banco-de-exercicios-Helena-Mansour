
#include <stdio.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"


const int BTN_PIN_G = 18;

const int LED_PIN_B = 17;


volatile bool btn_press= false;
volatile bool alarme= false;
volatile bool luz_acesa = false;

alarm_id_t id_alarme_luz = -1;


int64_t alarm_callback(alarm_id_t id, void *user_data) {
    alarme = true;
    return 0;

}

void btn_callback(uint gpio, uint32_t events) {
    if(events == 0x4){ //se o botao é precionado
        if(gpio == BTN_PIN_G){
            btn_press = true; 

            if (!luz_acesa){
                id_alarme_luz = add_alarm_in_ms(5000, alarm_callback, NULL, false);
              

            }else{
                cancel_alarm(id_alarme_luz);
                id_alarme_luz = add_alarm_in_ms(5000, alarm_callback, NULL, false);

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

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);


    
  
    while (1) {        
        if(btn_press){
            btn_press = false;
            luz_acesa = true;
            gpio_put(LED_PIN_B, 1);
      
        }
        if (alarme){
            alarme = false;
            
            alarme = false;
            luz_acesa = false; // <-- reseta o estado
            gpio_put(LED_PIN_B, 0);
        
        }
     

        

    }
    }



