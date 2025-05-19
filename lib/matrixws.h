#ifndef MATRIXWS_H
#define MATRIXWS_H

#include "pico/stdlib.h"

// Definição de constantes
#define PINO_MATRIZ 7  // Pino de controle da matriz de LEDs
#define NUM_LEDS 25   // Número total de LEDs na matriz
#define BRILHO 255   // Brilho dos LEDs (30% do máximo)

// Definição da estrutura de cor para cada LED
typedef struct 
{
    uint8_t R, G, B;       // Componentes de cor: vermelho, verde e azul
} npLED_t;

extern npLED_t leds[NUM_LEDS];  // Vetor que armazena as cores de todos os LEDs

// Funções
void controle(uint pino);       // Função de controle inicial da matriz de LEDs
void bf();                      // Função para atualizar os LEDs da matriz
void cores(const uint indice, const uint8_t r, const uint8_t g, const uint8_t b);  // Função para configurar a cor de um LED
void desliga();                 // Função para desligar todos os LEDs
void desenhaMatriz(int mat[5][5][3]);  // Função para desenhar a matriz de LEDs
void desenho_alerta();

#endif 