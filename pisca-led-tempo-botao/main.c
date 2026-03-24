#include <stdio.h>
#include <string.h> 
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"

const int BTN_PIN = 21;
const int LED_PIN = 16;

volatile bool pisca = false;

//botao precisonado
volatile bool btn_press = false;

//quando comecou a apertar
volatile uint32_t t_inicial = 0;

//quanto tempo segurou
volatile uint32_t t_pressionado = 500;

//botao solto
volatile bool atualiza = false;

//hora de parar
volatile bool parar = false;

//led rodando
volatile bool ativo = false; 

void btn_callback(uint gpio, uint32_t events) {
    if(events == 0x4){ //botao apertado
        btn_press = true;

        //horario atual
        t_inicial = to_ms_since_boot(get_absolute_time()); 
    }
    if(events == 0x8){ //botao solto
        uint32_t t_final = to_ms_since_boot(get_absolute_time());

        //horario final menos inicial
        t_pressionado = t_final - t_inicial;
        btn_press = false;
        atualiza = true;
    }
}

bool timer_callback(repeating_timer_t *rt) {
     //so pisca se nao tiver pressionado e se esta ativo
    if(!btn_press && ativo){
        pisca = true;
    }
    return true;
}

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    parar = true;
    return 0;
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
    
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);
    gpio_set_irq_enabled_with_callback(
        BTN_PIN,
        GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
        true,
        &btn_callback
    );

    repeating_timer_t timer;

    //timer pisca a cada 200ms
    add_repeating_timer_ms(200, timer_callback, NULL, &timer);

    bool led_estado = false;

    while (true) {

        // enquanto pressiona o LED apagado
        if(btn_press){ 
            gpio_put(LED_PIN,0);
        }

        // piscar LED (só se ativo)
        if(pisca && !btn_press && ativo){
            led_estado = !led_estado;
            gpio_put(LED_PIN, led_estado);
            pisca = false;
        }

        // quando solta botão reinicia inicia ciclo
        if(atualiza){
            //garante q o tempo minimo seja 50 ms
            if(t_pressionado < 50){
                t_pressionado = 50;
            }

            ativo = true; // ativa o sistema

            //led pisca por t_pressionado de tempo
            add_alarm_in_ms(t_pressionado, alarm_callback, NULL, false);

            atualiza = false;
        }

        // parar quando tempo acabar
        if(parar){
            parar = false;
            ativo = false;
            gpio_put(LED_PIN, 0);
        }
    }

    return 0;
}