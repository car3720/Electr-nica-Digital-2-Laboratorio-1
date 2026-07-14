/*
 * Timer0.h
 *
 * Created: 11/07/2026 18:46:00
 *  Author: Diego Cardona
 */ 


#ifndef TIMER0_H
#define TIMER0_H

#include <stdint.h>

// Contador de milisegundos de lectura directa. Se usa así
// (en vez de solo la función de abajo) desde DENTRO de otra
// interrupción, como la de Botones.c
extern volatile uint32_t milisegundosTranscurridos;

// Enciende el reloj de milisegundos (Timer0). Llamar una sola vez.
void Timer0_Init(void);

// Devuelve cuántos milisegundos han pasado. Usar desde el loop principal.
uint32_t Timer0_Millis(void);

#endif
