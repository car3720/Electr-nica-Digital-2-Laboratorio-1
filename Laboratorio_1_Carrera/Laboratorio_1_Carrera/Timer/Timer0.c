/*
 * Timer0.c
 *
 * Created: 11/07/2026 18:45:50
 *  Author: Diego Cardona
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Timer0.h"

// Se incrementa una vez cada 1ms dentro de la interrupción
volatile uint32_t milisegundosTranscurridos = 0;

void Timer0_Init(void)
{
	// Modo CTC: el Timer0 cuenta hasta OCR0A y se reinicia solo
	TCCR0A = (1<<WGM01);

	// Prescaler de 64 -> con F_CPU=16MHz, cada paso dura 4us
	TCCR0B = (1<<CS01) | (1<<CS00);

	// Queremos una interrupción cada 1000us (1ms).
	// 1000us / 4us = 250 pasos -> el valor de comparación es 250-1 = 249
	OCR0A = 249;

	// Habilita la interrupción por comparación
	TIMSK0 = (1<<OCIE0A);

	milisegundosTranscurridos = 0;
}

// Se ejecuta automáticamente cada 1ms
ISR(TIMER0_COMPA_vect)
{
	milisegundosTranscurridos++;
}

uint32_t Timer0_Millis(void)
{
	uint32_t copia;

	// Apagamos las interrupciones un instante para leer la
	// variable de 4 bytes sin riesgo de que se corrompa a la
	// mitad de la lectura
	cli();
	copia = milisegundosTranscurridos;
	sei();

	return copia;
}