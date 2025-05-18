#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
#include "lib/display.h"
#include "lib/font.h"

#define joy_y 26
#define joy_x 27

volatile uint16_t last_x = 0;
volatile uint16_t last_y = 0;
volatile bool alert_mode = false;

#define green 11
#define blue 12
#define red 13

QueueHandle_t xQueueJoyX;
QueueHandle_t xQueueJoyY;

// System mode control
typedef enum {
    NORMAL_MODE,
    ALERT_MODE
} system_mode_t;

system_mode_t current_mode = NORMAL_MODE;

// Tarefa do potênciometro y
void vJoy_y_Task(void *params)
{
    adc_gpio_init(joy_y);
    adc_init();

    while (true)
    {
        adc_select_input(1); // GPIO 27 = ADC1
        uint16_t y_pos = adc_read();
        printf("[Tarefa: %s] Valor do joystick (Y): %u\n", pcTaskGetName(NULL), y_pos);
        xQueueSend(xQueueJoyY, &y_pos, portMAX_DELAY); // Envia o valor do joystick para a fila
        vTaskDelay(pdMS_TO_TICKS(200));               // 10 Hz de leitura
    }
}

// Tarefa do potênciometro x
void vJoy_x_task(void *params)
{
    adc_gpio_init(joy_x);
    adc_init();

    while (true)
    {
        adc_select_input(0); // GPIO 26 = ADC0
        uint16_t x_pos = adc_read();
        printf("[Tarefa: %s] Valor do joystick (X): %u\n", pcTaskGetName(NULL), x_pos);
        xQueueSend(xQueueJoyX, &x_pos, portMAX_DELAY); // Envia o valor do joystick para a fila
        vTaskDelay(pdMS_TO_TICKS(200));               // 10 Hz de leitura
    }
    
}

void vLedBlueTask(void *params)
{
    gpio_set_function(blue, GPIO_FUNC_PWM);   // Configura GPIO como PWM
    uint slice = pwm_gpio_to_slice_num(blue); // Obtém o slice de PWM
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f); 
    pwm_init(slice, &config, true);

    while (true)
    {
        uint16_t y_pos;
        if (xQueueReceive(xQueueJoyY, &y_pos, portMAX_DELAY))
        {
            // Converte de 0–4095 para 0–65535 (16 bits PWM)
            uint16_t pwm_y_level = (y_pos << 4); // rápido e simples
            pwm_set_gpio_level(blue, pwm_y_level);
            printf("[Tarefa: %s] PWM ajustado para: %u\n", pcTaskGetName(NULL), pwm_y_level);
            last_y = y_pos;
        }
    }
}

void vLedGreenTask(void *params)
{
    gpio_set_function(green, GPIO_FUNC_PWM);   // Configura GPIO como PWM
    uint slice = pwm_gpio_to_slice_num(green); // Obtém o slice de PWM
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f); 
    pwm_init(slice, &config, true);

    while (true)
    {
        uint16_t x_pos;
        if (xQueueReceive(xQueueJoyX, &x_pos, portMAX_DELAY))
        {
            // Converte de 0–4095 para 0–65535 (16 bits PWM)
            uint16_t pwm_x_level = (x_pos << 4); // rápido e simples
            pwm_set_gpio_level(green, pwm_x_level);
            printf("[Tarefa: %s] PWM ajustado para: %u\n", pcTaskGetName(NULL), pwm_x_level);
            last_x = x_pos;
        }
    }
}

// Variáveis globais atualizadas
volatile bool alert_triggered = false;
volatile bool needs_display_clear = false;

// Task de Alerta Modificada
void vAlertTask(void *params) {
    while (true) {
        float nivel_agua = (last_x / 4095.0f) * 100.0f;
        float volume_chuva = (last_y / 4095.0f) * 100.0f;
        
        if (nivel_agua > 70.0f || volume_chuva > 80.0f) {
            if (!alert_triggered) {
                alert_triggered = true;
                needs_display_clear = true;
            }
        } else {
            if (alert_triggered) {
                alert_triggered = false;
                needs_display_clear = true;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// Task de Display Reestruturada
void vDisplayTask(void *params) {
    char displayText[50];
    uint16_t prev_x = 0xFFFF, prev_y = 0xFFFF;
    bool current_display_mode = false; // false = normal, true = alert
    
    while (true) {
        // Verifica se precisa mudar o modo de exibição
        if (needs_display_clear) {
            ssd1306_fill(&ssd, !cor);
            current_display_mode = alert_triggered;
            needs_display_clear = false;
        }
        
        if (alert_triggered) {
            // Modo Alerta
            escrever(&ssd, "ALERTA!", 30, 20, cor);
            
            char buffer[20];
            snprintf(buffer, sizeof(buffer), "Agua: %.0f%%", (last_x / 4095.0f) * 100.0f);
            escrever(&ssd, buffer, 10, 40, cor);
            
            snprintf(buffer, sizeof(buffer), "Chuva: %.0f%%", (last_y / 4095.0f) * 100.0f);
            escrever(&ssd, buffer, 10, 55, cor);
        } else {
            // Modo Normal
            if (last_x != prev_x) {
                prev_x = last_x;
                float Nivel_agua = (last_x / 4095.0f)*100.0f;
                limpar_area(0, 0, 128, 16); // Limpa área maior
                escrever(&ssd, "Nivel da agua", 10, 2, cor);
                snprintf(displayText, sizeof(displayText), "%.0f%%", Nivel_agua);
                escrever(&ssd, displayText, 60, 14, cor);
            }

            if (last_y != prev_y) {
                prev_y = last_y;
                float volume_chuva = (last_y / 4095.0f)*100.0f;
                limpar_area(0, 30, 128, 16); // Limpa área maior
                escrever(&ssd, "Volume de Chuva", 5, 26, cor);
                snprintf(displayText, sizeof(displayText), "%.0f%%", volume_chuva);
                escrever(&ssd, displayText, 60, 38, cor);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Modo BOOTSEL com botão B
#include "pico/bootrom.h"
#define botaoB 6
void gpio_irq_handler(uint gpio, uint32_t events)
{
    reset_usb_boot(0, 0);
}

int main() {   
    init_display();
    ssd1306_fill(&ssd, !cor);
    escrever(&ssd, "INICIANDO!!!", 10, 10, cor);
    sleep_ms(1000);
    ssd1306_fill(&ssd, !cor);

    // BOOTSEL button setup
    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    stdio_init_all();
    sleep_ms(1000);

    // Create queues
    xQueueJoyX = xQueueCreate(10, sizeof(uint16_t));
    xQueueJoyY = xQueueCreate(10, sizeof(uint16_t));

    // Create tasks
    xTaskCreate(vJoy_y_Task, "Joystick Y", 256, NULL, 1, NULL);
    xTaskCreate(vJoy_x_task, "Joystick X", 256, NULL, 1, NULL);
    xTaskCreate(vLedBlueTask, "LED Blue", 256, NULL, 1, NULL);
    xTaskCreate(vLedGreenTask, "LED Green", 256, NULL, 1, NULL);
    xTaskCreate(vDisplayTask, "Display", 512, NULL, 2, NULL);
    xTaskCreate(vAlertTask, "Alert", 256, NULL, 3, NULL); // Higher priority
    
    vTaskStartScheduler();
    panic_unsupported();
}