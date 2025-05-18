#ifndef BUTTONS_H
#define BUTTONS_H

// Modo BOOTSEL com botão B
#include "pico/bootrom.h"
#include "pico/stdio.h"
#include <stdint.h>

#define botaoB 6

void iniciar_botoes()
{
    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);
}

void gpio_irq_handler(uint gpio, uint32_t events)
{
    reset_usb_boot(0, 0);
}

#endif