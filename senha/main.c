#include "pico/stdlib.h"

// ==========================================
// DEFINIÇÃO DOS PINOS (Ajuste para sua placa)
// ==========================================
#define BTN_1 10
#define BTN_2 11
#define BTN_3 12
#define BTN_4 13

#define LED_ROXO     15
#define LED_VERDE    16
#define LED_VERMELHO 17

// ==========================================
// VARIÁVEIS GLOBAIS (Voláteis para a ISR)
// ==========================================
volatile int estado_sistema = 0; // 0 = Configurando senha, 1 = Validando senha
volatile int contador_press = 0;

volatile uint32_t senha_salva[4];
volatile uint32_t senha_digitada[4];

// Contador para o LED Roxo (permite saber se ainda há algum botão segurado sem usar gpio_get)
volatile int botoes_segurados = 0; 

// Flags para o loop principal
volatile bool flag_acionar_verde = false;
volatile bool flag_acionar_vermelho = false;

// Variável para debounce
volatile uint32_t ultimo_tempo_irq = 0;

// ==========================================
// ROTINA DE INTERRUPÇÃO (ISR)
// ==========================================
void gpio_callback(uint gpio, uint32_t events) {
    
    // Se for borda de descida (botão pressionado)
    if (events & GPIO_IRQ_EDGE_FALL) {
        
        // Lógica do LED Roxo: Acende se apertar qualquer botão
        botoes_segurados++;
        gpio_put(LED_ROXO, 1);

        // Debounce de 200ms para evitar leituras fantasmas
        uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
        if (tempo_atual - ultimo_tempo_irq > 200) {
            ultimo_tempo_irq = tempo_atual;

            // ESTADO 0: Configurando a senha
            if (estado_sistema == 0) {
                senha_salva[contador_press] = gpio;
                contador_press++;
                
                if (contador_press == 4) {
                    estado_sistema = 1; // Muda para o modo de validação
                    contador_press = 0;
                    flag_acionar_verde = true; // Avisa o main para piscar o verde
                }
            } 
            // ESTADO 1: Validando a senha
            else if (estado_sistema == 1) {
                senha_digitada[contador_press] = gpio;
                contador_press++;
                
                // Só verifica quando apertar 4 vezes
                if (contador_press == 4) {
                    bool senha_correta = true;
                    
                    for (int i = 0; i < 4; i++) {
                        if (senha_salva[i] != senha_digitada[i]) {
                            senha_correta = false;
                            break;
                        }
                    }

                    if (senha_correta) {
                        flag_acionar_verde = true;
                    } else {
                        flag_acionar_vermelho = true;
                    }
                    
                    contador_press = 0; // Zera para a próxima tentativa
                }
            }
        }
    } 
    // Se for borda de subida (botão solto)
    else if (events & GPIO_IRQ_EDGE_RISE) {
        botoes_segurados--;
        if (botoes_segurados <= 0) {
            botoes_segurados = 0; // Previne números negativos por ruído
            gpio_put(LED_ROXO, 0); // Apaga o roxo apenas quando nenhum botão estiver pressionado
        }
    }
}

// ==========================================
// FUNÇÃO PRINCIPAL
// ==========================================
int main() {
    // Inicializações padrão
    stdio_init_all();

    // Configuração dos LEDs
    gpio_init(LED_ROXO);
    gpio_set_dir(LED_ROXO, GPIO_OUT);
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);

    // Configuração dos Botões com Pull-Up
    uint botoes[] = {BTN_1, BTN_2, BTN_3, BTN_4};
    for (int i = 0; i < 4; i++) {
        gpio_init(botoes[i]);
        gpio_set_dir(botoes[i], GPIO_IN);
        gpio_pull_up(botoes[i]);
    }

    // Configuração das Interrupções (Aciona tanto na descida quanto na subida)
    gpio_set_irq_enabled_with_callback(BTN_1, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled(BTN_2, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(BTN_3, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(BTN_4, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);

    // Loop infinito Baremetal
    while (true) {
        
        // Verifica se a flag verde foi levantada pela ISR
        if (flag_acionar_verde) {
            flag_acionar_verde = false; // Abaixa a flag imediatamente
            gpio_put(LED_VERDE, 1);
            sleep_ms(300);
            gpio_put(LED_VERDE, 0);
        }

        // Verifica se a flag vermelha foi levantada pela ISR
        if (flag_acionar_vermelho) {
            flag_acionar_vermelho = false; // Abaixa a flag imediatamente
            gpio_put(LED_VERMELHO, 1);
            sleep_ms(300);
            gpio_put(LED_VERMELHO, 0);
        }
        
        // Pequeno sleep para não fritar a CPU no while(1)
        sleep_ms(10); 
    }
}