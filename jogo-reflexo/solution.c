#include <stdio.h>              
#include "hardware/gpio.h"     
#include "pico/stdlib.h"       


// LEDs 
const int LED_vermelho = 13;
const int LED_amarelo = 9;
const int LED_verde = 5;

// Botões 
const int BTN_vermelho = 20;
const int BTN_amarelo = 26;
const int BTN_verde = 28;


// MAPEAMENTO DE CORES
// Associa índices às cores:
// 0 = amarelo
// 1 = verde
// 2 = vermelho
const int LEDS[3] = {LED_amarelo, LED_verde, LED_vermelho};
const int BTNS[3] = {BTN_amarelo, BTN_verde, BTN_vermelho};

// =====================
// SEQUÊNCIA FIXA
// =====================

// Exigência do enunciado:
// "sequência fixa (não aleatória) com 10 passos"

// Convertido:
// amarelo=0, verde=1, vermelho=2
const int sequencia[10] = {0,1,2, 0,1,0, 2,0,1,0};


// VARIÁVEL GLOBAL (IRQ)


// Guarda qual botão foi pressionado
// -1 significa: nenhum botão pressionado ainda
volatile int btn_pressionado = -1;


// INTERRUPÇÃO DOS BOTÕES


// Função chamada automaticamente quando um botão é pressionado
// Requisito do enunciado: usar interrupção (não pode usar gpio_get)
void btn_callback(uint gpio, uint32_t events) {

    // Detecta borda de descida (botão pressionado com pull-up)
    if (events & GPIO_IRQ_EDGE_FALL) {

        // Identifica qual botão foi pressionado
        // (sem usar loop, pois ISR deve ser rápida)
        if (gpio == BTN_amarelo) btn_pressionado = 0;
        else if (gpio == BTN_verde) btn_pressionado = 1;
        else if (gpio == BTN_vermelho) btn_pressionado = 2;
    }
}


// FUNÇÕES AUXILIARES

// Apaga todos os LEDs
void apaga_todos() {
    for (int i = 0; i < 3; i++)
        gpio_put(LEDS[i], 0);
}

// Acende um LED específico com temporização
void acende_led(int idx) {

    gpio_put(LEDS[idx], 1);   // liga LED

    // Regra do enunciado:
    // "LED aceso por 300 ms"
    sleep_ms(300);

    gpio_put(LEDS[idx], 0);   // desliga LED

    // Regra do enunciado:
    // "intervalo de 300 ms"
    sleep_ms(300);
}

// Mostra a sequência até a rodada atual
void mostra_sequencia(int rodada) {

    sleep_ms(500); // pequena pausa antes de começar

    // Regra:
    // sequência cumulativa (cada rodada aumenta 1 passo)
    for (int i = 0; i < rodada; i++) {
        acende_led(sequencia[i]);
    }
}


// MAIN (LÓGICA DO JOGO)


int main() {

    stdio_init_all(); // inicializa comunicação (para printf)

    
    // CONFIGURAÇÃO DOS LEDs
    
    for (int i = 0; i < 3; i++) {
        gpio_init(LEDS[i]);            // inicializa pino
        gpio_set_dir(LEDS[i], GPIO_OUT); // define como saída
        gpio_put(LEDS[i], 0);          // começa apagado
    }

    
    // CONFIGURAÇÃO DOS BOTÕES

    for (int i = 0; i < 3; i++) {
        gpio_init(BTNS[i]);           // inicializa botão
        gpio_set_dir(BTNS[i], GPIO_IN); // define como entrada
        gpio_pull_up(BTNS[i]);        // ativa pull-up
    }

   
    // CONFIGURAÇÃO DAS INTERRUPÇÕES
    

    // Primeiro botão registra o callback
    gpio_set_irq_enabled_with_callback(
        BTNS[0], GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    // Os demais só habilitam IRQ
    for (int i = 1; i < 3; i++) {
        gpio_set_irq_enabled(BTNS[i], GPIO_IRQ_EDGE_FALL, true);
    }

   
    // INÍCIO DO JOGO
    

    // Regra do enunciado:
    // "O jogo é iniciado pelo botão verde"

    while (1) {
        if (btn_pressionado == 1) { // índice do verde
            btn_pressionado = -1;
            break;
        }
        sleep_ms(10);
    }

    int pnts = 0; // pontuação

    
    // LOOP PRINCIPAL DO JOGO
    

    for (int rodada = 1; rodada <= 10; rodada++) {

        // mostra sequência atual
        mostra_sequencia(rodada);

        int acertou = 1;

        // jogador repete sequência
        for (int i = 0; i < rodada; i++) {

            btn_pressionado = -1;

            // espera até pressionar botão
            while (btn_pressionado == -1) {
                tight_loop_contents();
            }

            int b = btn_pressionado;
            btn_pressionado = -1;

            // verifica erro
            if (b != sequencia[i]) {
                acertou = 0;
                break;
            }

            // feedback visual (acendeu o botão correto)
            acende_led(b);
        }

        // se errou, termina jogo
        if (!acertou) break;

        // atualiza pontuação
        pnts = rodada;
    }

  
    // FINAL DO JOGO


    // Regra do enunciado:
    // "print exatamente nesse formato"
    printf("Points %d\n", pnts);

    // feedback visual final (piscar LEDs)
    for (int i = 0; i < pnts; i++) {
        for (int j = 0; j < 3; j++)
            gpio_put(LEDS[j], 1);

        sleep_ms(200);

        apaga_todos();

        sleep_ms(200);
    }

    // fim do programa (loop infinito)
    while (1) {}
}