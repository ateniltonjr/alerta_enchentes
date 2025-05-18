#include "tarefas_utilizadas.h"
#include "lib/buttons.h"

int main() {  
    controle(PINO_MATRIZ);
    iniciar_buzzer();
    iniciar_rgb(); 
    init_display();
    ssd1306_fill(&ssd, !cor);
    escrever(&ssd, "INICIANDO!!!", 10, 10, cor);
    sleep_ms(1000);
    ssd1306_fill(&ssd, !cor);

    iniciar_botoes();
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
    xTaskCreate(vLedRedTask, "LED Red", 256, NULL, 1, NULL);
    xTaskCreate(vDesenhoTask, "Desenho na matriz", 256, NULL, 3, NULL);
    xTaskCreate(vDisplayTask, "Display", 512, NULL, 2, NULL);
    xTaskCreate(vAlertTask, "Alert", 256, NULL, 3, NULL);
    xTaskCreate(vBuzzerTask, "Buzzer", 256, NULL, 2, NULL);
    
    vTaskStartScheduler();
    panic_unsupported();
}