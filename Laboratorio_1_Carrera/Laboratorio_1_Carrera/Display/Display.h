/*
 * Display.h
 *
 * Created: 11/07/2026 18:45:04
 *  Author: Diego Cardona
 */ 

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

// Configura los pines del display como salida
void Display_Init(void);

// Muestra un número (0 al 9) en el display de 7 segmentos
void Display_Mostrar(uint8_t numero);

// Apaga todos los segmentos del display
void Display_Apagar(void);

#endif
