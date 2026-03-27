#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"
#define BTN_PIN_Y 28
#define BTN_PIN_B 22
#define BTN_PIN_G 18

#define LED_PIN_Y 6
#define LED_PIN_B 10
#define LED_PIN_G 14

volatile int flag_btn_y = 0;
volatile int flag_btn_b = 0;
volatile int flag_btn_g = 0;

void btn_callback(uint gpio, uint32_t events) {
  if(gpio==BTN_PIN_Y){
    if (events & GPIO_IRQ_EDGE_FALL){
      flag_btn_y = !flag_btn_y;
    }
  }
  if(gpio == BTN_PIN_B){
    if (events & GPIO_IRQ_EDGE_RISE){
      flag_btn_b = !flag_btn_b;
    }
  }
  if(gpio == BTN_PIN_G){
    if (events & GPIO_IRQ_EDGE_FALL){
      flag_btn_g = 1;
    }
    if (events & GPIO_IRQ_EDGE_RISE){
      flag_btn_g = 0;
    }
  }
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

  gpio_init(BTN_PIN_G);
  gpio_set_dir(BTN_PIN_G, GPIO_IN);
  gpio_pull_up(BTN_PIN_G);
  gpio_set_irq_enabled_with_callback(
      BTN_PIN_G, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &btn_callback);
      
  gpio_init(LED_PIN_Y);
  gpio_set_dir(LED_PIN_Y, GPIO_OUT);
  gpio_init(LED_PIN_B);
  gpio_set_dir(LED_PIN_B, GPIO_OUT);

  gpio_init(LED_PIN_G);
  gpio_set_dir(LED_PIN_G, GPIO_OUT);


  int led_status_y = 0;
  int led_status_b = 0;
  int led_status_g = 0;

  
  while (true) {
    if(flag_btn_b){
      led_status_b = !led_status_b;
      gpio_put(LED_PIN_B, led_status_b);
    }else{
      gpio_put(LED_PIN_B ,0);
    }

    if(flag_btn_g){
      led_status_g =  !led_status_g;
      gpio_put(LED_PIN_G, led_status_g);
    }else{
      gpio_put(LED_PIN_G ,0);
    }

    if(flag_btn_y){
      led_status_y = !led_status_y;
      gpio_put(LED_PIN_Y ,led_status_y);
    }else{
      gpio_put(LED_PIN_Y ,0);
    }
    
    sleep_ms(200);
  }
}
