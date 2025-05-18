#include "tarefas_utilizadas.h"
#include "lib/buttons.h"

int main() 
{  
    controle(PINO_MATRIZ); // Inicializa a matriz de leds
    iniciar_botoes();     // Inicializa o botão B
    iniciar_buzzer();    // Inicializa o buzzer A
    stdio_init_all();   // inicializa o monitor serial
    init_display();    // Inicializa o display
    iniciar_rgb();    // Inicializa o led vermelho
    
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    ssd1306_fill(&ssd, !cor);
    escrever(&ssd, "INICIANDO!!!", 10, 10, cor);
    sleep_ms(1000);
    ssd1306_fill(&ssd, !cor);

    sleep_ms(1000);

    // queues criadas
    xQueueJoyX = xQueueCreate(10, sizeof(uint16_t));
    xQueueJoyY = xQueueCreate(10, sizeof(uint16_t));

    // tasks criadas
    xTaskCreate(vJoy_y_Task, "Joystick Y", 256, NULL, 1, NULL);
    xTaskCreate(vJoy_x_task, "Joystick X", 256, NULL, 1, NULL);
    xTaskCreate(vLedBlueTask, "LED Blue", 256, NULL, 1, NULL);
    xTaskCreate(vLedGreenTask, "LED Green", 256, NULL, 1, NULL);
    xTaskCreate(vLedRedTask, "LED Red", 256, NULL, 1, NULL);
    xTaskCreate(vDesenhoTask, "Desenho na matriz", 256, NULL, 3, NULL);
    xTaskCreate(vDisplayTask, "Display", 512, NULL, 2, NULL);
    xTaskCreate(vAlertTask, "Alert", 256, NULL, 3, NULL);
    xTaskCreate(vBuzzerTask, "Buzzer", 256, NULL, 2, NULL);
    
    vTaskStartScheduler();
    panic_unsupported();
}