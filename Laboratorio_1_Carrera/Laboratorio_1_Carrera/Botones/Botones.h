/*
 * Botones.h
 *
 * Created: 11/07/2026 18:45:17
 *  Author: Diego Cardona
 */ 
#ifndef BOTONES_H
#define BOTONES_H

#include <stdint.h>

// Configura los 3 botones como entrada (con pull-up FÍSICO en
// la protoboard, no interno) y activa el aviso por interrupción.
// El botón de inicio (PB5) queda listo desde ya. Los botones de
// los jugadores (PC4, PC5) empiezan apagados: se encienden con
// Botones_HabilitarJugadores()
void Botones_Init(void);

// Enciende el aviso de los botones de los jugadores.
// Llamar justo cuando termina la cuenta regresiva.
void Botones_HabilitarJugadores(void);

// Apaga el aviso de los botones de los jugadores.
// Llamar justo cuando empieza la cuenta regresiva.
void Botones_DeshabilitarJugadores(void);

// Devuelven 1 SOLO en el instante en que se confirma una
// pulsación válida (ya filtrado el rebote), 0 en cualquier
// otro momento. No bloquean el programa.
uint8_t BotonInicioPresionado(void);
uint8_t BotonJugador1Presionado(void);
uint8_t BotonJugador2Presionado(void);

#endif