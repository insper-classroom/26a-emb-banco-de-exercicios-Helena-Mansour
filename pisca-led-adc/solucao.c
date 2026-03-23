/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


//  #include <stdio.h>
//  #include "pico/stdlib.h"
//  #include "hardware/gpio.h"
//  #include "hardware/timer.h"
//  #include "hardware/adc.h"
 
//  const int PIN_LED_B = 4;
 
//  const float conversion_factor = 3.3f / (1 << 12);
 
//  volatile int f_timer_b = 0 ;
 
//  bool timer_b_callback(repeating_timer_t *rt) {
//      f_timer_b = 1;
//   //   printf("a\n");
//      return true; // keep repeating
//  }
 
//  /**
//   * 0..1.0V: Desligado
//   * 1..2.0V: 150 ms
//   * 2..3.3V: 400 ms
//   */
//  int main() {
//      stdio_init_all();
 
//      gpio_init(PIN_LED_B);
//      gpio_set_dir(PIN_LED_B, GPIO_OUT);
 
//      adc_init();
//      adc_gpio_init(28);
//      adc_gpio_init(26);
 
//      int zone_old_b = 0;
 
//      int zone_b = 0;
 
//      repeating_timer_t timer_b;
 
//      int led_b = 0;
 
//      while (1) {
//          if (f_timer_b) {
//              f_timer_b = 0;
//              led_b = !led_b;
//              gpio_put(PIN_LED_B, led_b);
//          }
 
//          adc_select_input(2);
//          float result_b = adc_read() * conversion_factor;
//          //printf("%f\n", result_b);
//          if (result_b <= 1.0)
//              zone_b = 0;
//          else if (result_b <= 2.0)
//              zone_b = 1;
//          else
//              zone_b = 2;
         
//          if (zone_old_b != zone_b) {
//              zone_old_b = zone_b;
//              cancel_repeating_timer(&timer_b);
//              printf("Zone: %d \n", zone_b);

//              f_timer_b = 0;

//              if (zone_b ==  0) 
//                  gpio_put(PIN_LED_B, 0);
//              else if (zone_b == 1) 
//                  add_repeating_timer_ms(300, timer_b_callback, NULL, &timer_b);
//              else
//                  add_repeating_timer_ms(500, timer_b_callback, NULL, &timer_b);
//          }
//      }
//  }