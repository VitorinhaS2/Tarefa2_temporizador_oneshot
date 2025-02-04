// inclusão das bibliotecas necessarias
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"

// definição dos pinos GPIO dos LEDs e botão
#define LED_GREEN_PIN 11
#define LED_BLUE_PIN 12
#define LED_RED_PIN 13
#define BUTTON_PIN 5

// estado atual do LED e controle de repetição
static uint current_led_pin = LED_GREEN_PIN;
static bool leds_active = false;

// callback para desligar cada LED e iniciar o próximo
int64_t led_off_callback(alarm_id_t id, void *user_data) {
    gpio_put(current_led_pin, 0); // desliga o LED atual

    if (current_led_pin < LED_RED_PIN) {
        current_led_pin++; // avança para o próximo LED
        return 3000 * 1000; // repete em 3 segundos
    } else {
        // reinicia para a próxima execução
        leds_active = false;
        current_led_pin = LED_GREEN_PIN;
        return 0; 
    }
}

void configure_gpio_pins() {
    // configura cada LED como saída
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);

    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);

    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);

    // configura o botão como entrada com pull-up
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
}

int main() {
    // inicializa a comunicação serial e configura GPIO
    stdio_init_all();
    configure_gpio_pins();

    while (true) {
        // verifica se o botão foi pressionado e os LEDs não estão ativos
        if (gpio_get(BUTTON_PIN) == 0 && !leds_active) {
            sleep_ms(50); // debouncing

            if (gpio_get(BUTTON_PIN) == 0) {
                // liga todos os LEDs inicialmente
                gpio_put(LED_GREEN_PIN, 1);
                gpio_put(LED_BLUE_PIN, 1);
                gpio_put(LED_RED_PIN, 1);

                leds_active = true;
                // inicia sequência para desligar os LEDs
                add_alarm_in_ms(3000, led_off_callback, NULL, false);
            }
        }

        // pequeno atraso para otimizar o loop
        sleep_ms(10);
    }

    return 0;
}
