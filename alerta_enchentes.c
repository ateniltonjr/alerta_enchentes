/**************************************************************
* Projeto: Monitoramento de Nível de Água e Volume de Chuva
*
* Descrição:
*   Este código inicializa os periféricos do sistema e cria 
*   tarefas FreeRTOS para ler os valores dos joysticks, 
*   controlar LEDs, atualizar o display, tocar buzzer e 
*   desenhar na matriz de LEDs. Utiliza filas para 
*   comunicação entre tarefas.
*************************************************************/

#include "tarefas_utilizadas.h" // Inclusão de definições e variáveis compartilhadas
#include "lib/buttons.h"       // Inclusão de funções para controle dos botões

int main() 
{  
    controle(PINO_MATRIZ); // Inicializa a matriz de LEDs WS2812
    iniciar_botoes();     // Configura o botão B com interrupção
    iniciar_buzzer();    // Configura o buzzer no pino definido
    stdio_init_all();   // Inicializa a saída padrão (monitor serial via USB)
    init_display();    // Inicializa o display OLED
    iniciar_rgb();    // Inicializa o LED RGB vermelho

    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); 
    // Habilita interrupção para o botão B com callback definido (modo BOOTSEL)

    ssd1306_fill(&ssd, !cor); // Limpa o display (fundo branco ou preto invertido)
    escrever(&ssd, "INICIANDO!!!", 10, 10, cor); // Mensagem de inicialização
    sleep_ms(1000); // Aguarda 1 segundo
    ssd1306_fill(&ssd, !cor); // Limpa o display novamente

    sleep_ms(1000); // Espera adicional antes de iniciar o sistema

    // Criação das filas para os valores dos eixos X e Y do joystick
    xQueueJoyX = xQueueCreate(10, sizeof(uint16_t));  // Fila para eixo X
    xQueueJoyY = xQueueCreate(10, sizeof(uint16_t)); // Fila para eixo Y

    // Criação das tarefas do sistema
    xTaskCreate(vJoy_y_Task, "Joystick Y", 256, NULL, 1, NULL);          // Lê valor Y do joystick
    xTaskCreate(vJoy_x_task, "Joystick X", 256, NULL, 1, NULL);         // Lê valor X do joystick
    xTaskCreate(vLedBlueTask, "LED Blue", 256, NULL, 1, NULL);         // Controla LED azul via PWM
    xTaskCreate(vLedGreenTask, "LED Green", 256, NULL, 1, NULL);      // Controla LED verde via PWM
    xTaskCreate(vLedRedTask, "LED Red", 256, NULL, 1, NULL);         // Controla LED vermelho
    xTaskCreate(vDesenhoTask, "matriz alerta", 256, NULL, 3, NULL); // Desenha na matriz WS2812
    xTaskCreate(vDisplayTask, "Display", 512, NULL, 2, NULL);      // Atualiza display com dados
    xTaskCreate(vAlertTask, "Alert", 256, NULL, 3, NULL);         // Gerencia a lógica de alerta
    xTaskCreate(vBuzzerTask, "Buzzer", 256, NULL, 2, NULL);      // Controla buzzer em modo de alerta
    
    vTaskStartScheduler(); // Inicia o agendador do FreeRTOS
    panic_unsupported();  // Caso o RTOS termine (não deveria acontecer), entra em pânico
}
