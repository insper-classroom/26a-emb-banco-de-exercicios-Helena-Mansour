/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h> 
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"

const int BTN_PIN = 28;
const int LED_PIN = 5;

volatile bool pisca = false;
volatile bool btn_press = false;
volatile uint32_t t_inicial = 0;
volatile uint32_t t_pressionado = 500;
volatile bool atualiza = false;

void btn_callback(uint gpio, uint32_t events) {
    if(events == 0x4){
        btn_press = true;
        t_inicial = to_ms_since_boot(get_absolute_time());
    }
    if(events == 0x8){
        uint32_t t_final = 0;
        t_final = to_ms_since_boot(get_absolute_time());
        t_pressionado = t_final - t_inicial;
        btn_press = false;
        atualiza = true;
    }
}

bool timer_callback(repeating_timer_t *rt) {
    if(!btn_press){
        pisca = true;

    }
    return true;
}


int main() {
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
    
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL|GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    repeating_timer_t timer;
    add_repeating_timer_ms(t_pressionado, timer_callback, NULL, &timer);

    bool led_estado = false;
    while (true) {
        if(btn_press){
            gpio_put(LED_PIN,0);
            btn_press = false;
        }
        if(pisca && !btn_press){
            led_estado = !led_estado;
            gpio_put(LED_PIN, led_estado);
            pisca = false;
        }
        if(atualiza){
            cancel_repeating_timer(&timer);
            add_repeating_timer_ms(t_pressionado, timer_callback, NULL, &timer);
            printf("Novo periodo: %u ms | Freq: %.2f Hz\n", t_pressionado, 1000.0/t_pressionado);
            atualiza = false;
        }

    }

    return 0;
}