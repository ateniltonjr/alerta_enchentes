#ifndef TAREFAS_UTILIZADAS_H  // Verifica se o cabeçalho já foi incluído, evita redefinições
#define TAREFAS_UTILIZADAS_H // Define o cabeçalho para controle de inclusão única

#include "hardware/adc.h"// Biblioteca para controlar ADC (conversor analógico-digital) do joystick
#include "FreeRTOS.h"   // Biblioteca principal do FreeRTOS
#include <string.h>    // Biblioteca padrão de manipulação de strings
#include <stdio.h>    // Biblioteca padrão para entrada e saída (ex: printf)
#include "queue.h"   // Biblioteca do FreeRTOS para uso de filas
#include "task.h"   // Biblioteca do FreeRTOS para manipulação de tarefas

#include "lib/matrixws.h"// Biblioteca personalizada para controlar matriz de LEDs WS2812
#include "lib/display.h"// Biblioteca personalizada para controle do display OLED
#include "lib/buzzer.h"// Biblioteca personalizada para controle de buzzer
#include "lib/font.h" // Biblioteca que define os caracteres (fontes) do display

#define joy_y 26 // Define o pino do joystick vertical (ADC1)
#define joy_x 27// Define o pino do joystick horizontal (ADC0)

volatile uint16_t last_x = 0;  // Armazena o último valor lido no eixo X (ADC0) — 'volatile' pois pode ser alterado por interrupções/tarefas
volatile uint16_t last_y = 0; // Armazena o último valor lido no eixo Y (ADC1) — idem

volatile bool alert_mode = false;// Indica se o sistema está em modo de alerta

#define ALERTA_NIVEL_AGUA 1    // Constante para alerta de nível de água
#define ALERTA_VOLUME_CHUVA 2 // Constante para alerta de volume de chuva
#define ALERTA_AMBOS 3       // Constante para alerta duplo (nível + volume)

// Variáveis compartilhadas entre tarefas
volatile uint8_t tipo_alerta = 0;            // Tipo de alerta ativo (define qual dos três acima está ocorrendo)
volatile bool alerta_sonoro_ativo = false;  // Indica se o buzzer está ativo
volatile bool alert_triggered = false;     // Indica se o alerta já foi disparado
volatile bool needs_display_clear = false;// Indica se o display precisa ser limpo
volatile bool buzzer_active = false;     // Indica se o buzzer está atualmente tocando

#define green 11 // Pino do LED verde (PWM)
#define blue 12 // Pino do LED azul (PWM)
#define red 13 // Pino do LED vermelho

QueueHandle_t xQueueJoyX;  // Fila para comunicação do valor do joystick X entre tarefas
QueueHandle_t xQueueJoyY; // Fila para comunicação do valor do joystick Y entre tarefas

// Enumeração para os modos do sistema
typedef enum 
{
    NORMAL_MODE, // Modo normal de funcionamento
    ALERT_MODE  // Modo de alerta
} system_mode_t;

system_mode_t current_mode = NORMAL_MODE; // Variável que armazena o modo atual do sistema

void iniciar_rgb() 
{
    gpio_init(red);
    gpio_set_dir(red, GPIO_OUT);
}

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

//Tarefa do pwm do led azul
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
            float nivel_agua = (last_x / 4095.0f) * 100.0f;
            float volume_chuva = (y_pos / 4095.0f) * 100.0f;
            
            // Só ajusta o PWM do LED azul se ambos os valores estiverem abaixo dos limites
            if (nivel_agua < 70.0f && volume_chuva < 80.0f) {
                uint16_t pwm_y_level = (y_pos << 4); // rápido e simples
                pwm_set_gpio_level(blue, pwm_y_level);
                printf("[Tarefa: %s] PWM ajustado para: %u\n", pcTaskGetName(NULL), pwm_y_level);
            } else {
                pwm_set_gpio_level(blue, 0); // Desliga o LED azul
            }
            last_y = y_pos;
        }
    }
}

//Tarefa do pwm do led verde
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
            float nivel_agua = (x_pos / 4095.0f) * 100.0f;
            float volume_chuva = (last_y / 4095.0f) * 100.0f;
            
            // Só ajusta o PWM do LED verde se ambos os valores estiverem abaixo dos limites
            if (nivel_agua < 70.0f && volume_chuva < 80.0f) {
                uint16_t pwm_x_level = (x_pos << 4); // rápido e simples
                pwm_set_gpio_level(green, pwm_x_level);
                printf("[Tarefa: %s] PWM ajustado para: %u\n", pcTaskGetName(NULL), pwm_x_level);
            } else {
                pwm_set_gpio_level(green, 0); // Desliga o LED verde
            }
            last_x = x_pos;
        }
    }
}

// Task led de alerta
void vLedRedTask(void *params)
{
    while (true)
    {
        float nivel_agua = (last_x / 4095.0f) * 100.0f;
        float volume_chuva = (last_y / 4095.0f) * 100.0f;
        
        // Aciona o LED vermelho se qualquer um dos valores ultrapassar os limites
        if (nivel_agua >= 70.0f || volume_chuva >= 80.0f) {
            gpio_put(red, 1); // Liga o LED vermelho
        } else {
            gpio_put(red, 0); // Desliga o LED vermelho
        }
        
        vTaskDelay(pdMS_TO_TICKS(200)); // Verifica a cada 200ms
    }
}

// Tarefa do desenho de alerta na matriz
void vDesenhoTask(void *params)
{
    while (true)
    {
        float nivel_agua = (last_x / 4095.0f) * 100.0f;
        float volume_chuva = (last_y / 4095.0f) * 100.0f;
        
        // Aciona o LED vermelho se qualquer um dos valores ultrapassar os limites
        if (nivel_agua >= 70.0f || volume_chuva >= 80.0f) {
            desenho_alerta();
            vTaskDelay(pdMS_TO_TICKS(500));
            desliga();
            vTaskDelay(pdMS_TO_TICKS(500));
        } else {
            desliga();
        }
        
        vTaskDelay(pdMS_TO_TICKS(200)); // Verifica a cada 200ms
    }
}

// Task de Alerta
void vAlertTask(void *params) {
    while (true) {
        float nivel_agua = (last_x / 4095.0f) * 100.0f;
        float volume_chuva = (last_y / 4095.0f) * 100.0f;
        
        bool alerta_agua = (nivel_agua > 70.0f);
        bool alerta_chuva = (volume_chuva > 80.0f);
        
        if (alerta_agua && alerta_chuva) {
            tipo_alerta = ALERTA_AMBOS;
        } else if (alerta_agua) {
            tipo_alerta = ALERTA_NIVEL_AGUA;
        } else if (alerta_chuva) {
            tipo_alerta = ALERTA_VOLUME_CHUVA;
        } else {
            tipo_alerta = 0;
        }
        
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// Task de exibição no Display 
void vDisplayTask(void *params) {
    char displayText[50];
    uint16_t prev_x = 0xFFFF, prev_y = 0xFFFF;
    uint8_t prev_alert_type = 0;
    bool needs_full_clear = true;

    while (true) {
        // Verifica se houve mudança no estado de alerta
        if (tipo_alerta != prev_alert_type) {
            needs_full_clear = true;
            prev_alert_type = tipo_alerta;
        }

        // Limpeza do display quando necessário
        if (needs_full_clear) {
            ssd1306_fill(&ssd, !cor);
            needs_full_clear = false;
        }

        if (tipo_alerta == 0) {
            // MODO NORMAL - Exibe valores dos sensores
            if (last_x != prev_x) {
                prev_x = last_x;
                float Nivel_agua = (last_x / 4095.0f) * 100.0f;
                limpar_area(0, 0, 128, 16); // Limpa área maior
                escrever(&ssd, "Nivel da agua", 10, 2, cor);
                snprintf(displayText, sizeof(displayText), "%.0f%%", Nivel_agua);
                escrever(&ssd, displayText, 60, 14, cor);
            }

            if (last_y != prev_y) {
                prev_y = last_y;
                float volume_chuva = (last_y / 4095.0f) * 100.0f;
                limpar_area(0, 30, 128, 16); // Limpa área maior
                escrever(&ssd, "Volume de Chuva", 5, 26, cor);
                snprintf(displayText, sizeof(displayText), "%.0f%%", volume_chuva);
                escrever(&ssd, displayText, 60, 38, cor);
            }
        } else {
            // MODO ALERTA - Exibe mensagem de alerta
            char alert_title[20];
            char alert_detail_chuva[30];
            char alert_detail_agua[30];
            
            switch(tipo_alerta) {
                case ALERTA_NIVEL_AGUA:
                    strcpy(alert_title, "ALERTA DE AGUA");
                    snprintf(alert_detail_agua, sizeof(alert_detail_agua), "Agua: %.0f%% MAX", (last_x / 4095.0f) * 100.0f);
                    snprintf(alert_detail_chuva, sizeof(alert_detail_chuva), "Chuva: %.0f%% MAX", (last_y / 4095.0f) * 100.0f);
                    break;
                    
                case ALERTA_VOLUME_CHUVA:
                    strcpy(alert_title, "ALERTA DE CHUVA");
                    snprintf(alert_detail_agua, sizeof(alert_detail_agua), "Agua: %.0f%% MAX", (last_x / 4095.0f) * 100.0f);
                    snprintf(alert_detail_chuva, sizeof(alert_detail_chuva), "Chuva: %.0f%% MAX", (last_y / 4095.0f) * 100.0f);
                    break;
                    
                case ALERTA_AMBOS:
                    strcpy(alert_title, "ALERTA MAXIMO");
                    snprintf(alert_detail_agua, sizeof(alert_detail_agua), "Agua: %.0f%% MAX", (last_x / 4095.0f) * 100.0f);
                    snprintf(alert_detail_chuva, sizeof(alert_detail_chuva), "Chuva: %.0f%% MAX", (last_y / 4095.0f) * 100.0f);
                    break;
            }

            // Exibe cabeçalho do alerta
            escrever(&ssd, alert_title, 5, 5, cor);
            
            // Linha divisória
            ssd1306_hline(&ssd, 0, 127, 15, cor);
            
            // Detalhes do alerta
            escrever(&ssd, alert_detail_chuva, 5, 30, cor);
            escrever(&ssd, alert_detail_agua, 5, 45, cor);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Task do Buzzer
void vBuzzerTask(void *params) {
    iniciar_buzzer();
    
    while (true) {
        if (tipo_alerta != 0) {
            switch(tipo_alerta) {
                case ALERTA_NIVEL_AGUA:
                    // Padrão para alerta de nível d'água (2 beeps longos)
                    tocar_nota(800, 400);
                    vTaskDelay(pdMS_TO_TICKS(300));
                    tocar_nota(800, 400);
                    break;
                    
                case ALERTA_VOLUME_CHUVA:
                    // Padrão para alerta de chuva (3 beeps curtos agudos)
                    tocar_nota(1200, 150);
                    vTaskDelay(pdMS_TO_TICKS(100));
                    tocar_nota(1200, 150);
                    vTaskDelay(pdMS_TO_TICKS(100));
                    tocar_nota(1200, 150);
                    break;
                    
                case ALERTA_AMBOS:
                    // Padrão para ambos alertas (alternância grave-agudo)
                    tocar_nota(600, 200);
                    vTaskDelay(pdMS_TO_TICKS(100));
                    tocar_nota(1200, 200);
                    vTaskDelay(pdMS_TO_TICKS(100));
                    tocar_nota(600, 200);
                    vTaskDelay(pdMS_TO_TICKS(100));
                    tocar_nota(1200, 200);
                    break;
            }
            // Pausa antes de repetir
            vTaskDelay(pdMS_TO_TICKS(1000));
        } else {
            gpio_put(buzzerA, 0);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
}

#endif