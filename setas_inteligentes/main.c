#include <stdio.h>
#include "hardware/gpio.h"
#include "pico/stdlib.h"

const int BTN_PIN_Y = 21;
const int BTN_PIN_G = 20;

const int LED_PIN_G = 17;
const int LED_PIN_Y = 14;


volatile bool btn_press_g= false;
volatile bool btn_press_y= false;

volatile bool alarme_y= false;
volatile bool alarme_g= false;

volatile bool pisca_g = false;
volatile bool pisca_y = false;

volatile alarm_id_t id_alarme_y = -1;
volatile alarm_id_t id_alarme_g = -1;

repeating_timer_t time_g;
repeating_timer_t time_y;

volatile bool pressed_y = false;
volatile bool pressed_g = false;

void btn_callback(uint gpio, uint32_t events) {
    if(events == 0x4){ //se o botao é precionado
        if(gpio == BTN_PIN_G){

            if (!pressed_g){
                btn_press_g = true;
                pressed_g = true;

            }else{
                alarme_g =  false;
                gpio_put(LED_PIN_G, 0);
                cancel_repeating_timer(&time_g);
                cancel_alarm(id_alarme_g);
            }
            if(pressed_y){
                btn_press_g = true;
                pressed_y = false;
                
                alarme_y = false;
                gpio_put(LED_PIN_Y, 0);
                cancel_repeating_timer(&time_y);
                cancel_alarm(id_alarme_y);

            }
             
        }
        if(gpio == BTN_PIN_Y){
            if (!pressed_y){
                btn_press_y = true;
                pressed_y = true;

            }else{
                gpio_put(LED_PIN_Y, 0);
                cancel_repeating_timer(&time_y);
                cancel_alarm(id_alarme_y);
            }
            if(pressed_g){
                btn_press_y = true;
                pressed_g = false;

                alarme_g = false;
                gpio_put(LED_PIN_G, 0);
                cancel_repeating_timer(&time_g);
                cancel_alarm(id_alarme_g);

            }
        }
    }
}

bool timer_y_callback(repeating_timer_t *rt) {
    pisca_y = true;
    return true;

}

bool timer_g_callback(repeating_timer_t *rt) {
    pisca_g = true;
    return true;
}


int64_t alarm_callback_g(alarm_id_t id, void *user_data) {
    alarme_g = true;
    return 0;

}

int64_t alarm_callback_y(alarm_id_t id, void *user_data) {
    alarme_y = true;
    return 0;

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

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);



  
    bool led_estado_y = false;
    bool led_estado_g = false;

  
    while (1) {        
        if(btn_press_g){
            btn_press_g = false;
           

            add_repeating_timer_ms(200, timer_g_callback, NULL, &time_g);
            id_alarme_g = add_alarm_in_ms(8000, alarm_callback_g, NULL, false);
         
        }
        if (alarme_g){
            pressed_g = false;
            alarme_g = false;
            gpio_put(LED_PIN_G, 0);

            cancel_repeating_timer(&time_g);
        
        }
        if(pisca_g){
            pisca_g = false;
            led_estado_g = !led_estado_g;
            gpio_put(LED_PIN_G, led_estado_g);
        }

        if(btn_press_y){
            btn_press_y = false;
            add_repeating_timer_ms(200, timer_y_callback, NULL, &time_y);
            id_alarme_y = add_alarm_in_ms(8000, alarm_callback_y, NULL, false);
         
        }
        if (alarme_y){
            pressed_y = false;
            alarme_y = false;
            gpio_put(LED_PIN_Y, 0);

            cancel_repeating_timer(&time_y);
        
        }
        if(pisca_y){
            pisca_y = false;
            led_estado_y = !led_estado_y;
            gpio_put(LED_PIN_Y, led_estado_y);
        }

        

    }
    }