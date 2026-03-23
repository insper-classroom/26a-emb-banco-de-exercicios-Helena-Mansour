#include "pico/stdlib.h"
#include <stdio.h>


#include "hardware/gpio.h"

// #define FIRST_GPIO 2
// #define BUTTON_GPIO (FIRST_GPIO+7)

// const int BTN_PIN_G = 28;
// volatile int cnt = 0;

// // This array converts a number 0-9 to a bit pattern to send to the GPIOs
// int bits[10] = {
//         0x3f,  // 0
//         0x06,  // 1
//         0x5b,  // 2
//         0x4f,  // 3
//         0x66,  // 4
//         0x6d,  // 5
//         0x7d,  // 6
//         0x07,  // 7
//         0x7f,  // 8
//         0x67   // 9
// };

// void seven_seg_init() {
//     for (int gpio = FIRST_GPIO; gpio < FIRST_GPIO + 7; gpio++) {
//         gpio_init(gpio);
//         gpio_set_dir(gpio, GPIO_OUT);
//     }
// }

// void seven_seg_display(int val) {
//     for (int gpio = FIRST_GPIO; gpio < FIRST_GPIO + 7; gpio++) {
//         gpio_put(gpio, 0);
//     }
//     int32_t mask = bits[val] << FIRST_GPIO;
//     gpio_set_mask(mask);   
// }

// void btn_callback(uint gpio, uint32_t events) {
//     if (events == 0x4) {         // fall edge
//         if (++cnt > 9) {
//             cnt = 0;
//         }
//         seven_seg_display(cnt);
//     }
//     else if (events == 0x8) {  // rise edge
//     }
// }

// int main() {
//     stdio_init_all();

//     gpio_init(BTN_PIN_G);
//     gpio_set_dir(BTN_PIN_G, GPIO_IN);
//     gpio_pull_up(BTN_PIN_G);
//     gpio_set_irq_enabled_with_callback(BTN_PIN_G,
//         GPIO_IRQ_EDGE_FALL,
//         true,
//         &btn_callback);

//     cnt = 0;
//     seven_seg_init();
//     seven_seg_display(cnt);

//     while (true) {

//     }
// }

int main() {
    stdio_init_all();

    while (1) {
       
    }

    return 0;
}
