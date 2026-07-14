/*
 * Botones.c
 *
 * Created: 11/07/2026 18:45:30
 *  Author: Diego Cardona
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Botones.h"
#include "../Timer/Timer0.h"

/*-------------------------------------------------------------
    Botón Inicio de Carrera -> PD7 (PCINT23)
    Botón Jugador 1          -> PC4 (PCINT12)
    Botón Jugador 2          -> PC5 (PCINT13)

    NOTA: el botón de inicio se movió de PB5 (D13) a PD7 (D7)
    porque D13 tiene soldado el LED integrado del Arduino Nano
    en ese mismo pin. Ese LED, junto con su resistencia, "carga"
    la línea hacia GND y no deja que el pull-up externo suba el
    voltaje lo suficiente como para leerse como un "alto" real,
    así que el pin queda leyendo "presionado" todo el tiempo,
    sin importar el estado del botón. PD7 no tiene ese problema.

    Los 3 botones tienen pull-up FÍSICO (resistencia externa en
    la protoboard), por lo que NO se activa el pull-up interno
    del microcontrolador. El pin queda en alto mientras el botón
    está suelto, y en bajo cuando se presiona.
    Por eso se usa "!" al leerlos, para que en el código
    "presionado" sea 1 y "suelto" sea 0.

    Tiempo de antirrebote: 30ms
-------------------------------------------------------------*/
#define TIEMPO_ANTIRREBOTE_MS 30

// Datos del botón de inicio
volatile uint8_t avisoInicio = 0;   // lo levanta la interrupción
volatile uint32_t horaInicio = 0;   // a qué hora llegó ese aviso
uint8_t estadoInicio = 0;           // último estado ya confirmado

// Datos del botón del jugador 1
volatile uint8_t avisoJugador1 = 0;
volatile uint32_t horaJugador1 = 0;
uint8_t estadoJugador1 = 0;

// Datos del botón del jugador 2
volatile uint8_t avisoJugador2 = 0;
volatile uint32_t horaJugador2 = 0;
uint8_t estadoJugador2 = 0;

void Botones_Init(void)
{
    // Botón de inicio (PD7): entrada. Pull-up FÍSICO, no interno.
    DDRD &= ~(1<<PD7);

    // Botón jugador 1 (PC4): entrada. Pull-up FÍSICO, no interno.
    DDRC &= ~(1<<PC4);

    // Botón jugador 2 (PC5): entrada. Pull-up FÍSICO, no interno.
    DDRC &= ~(1<<PC5);

    // Activa el aviso (interrupción) para el grupo de pines
    // PD (botón de inicio) y para el grupo de pines PC (jugadores)
    PCICR |= (1<<PCIE2) | (1<<PCIE1);

    // El botón de inicio queda avisando desde ya (PCINT23 = PD7)
    PCMSK2 |= (1<<PCINT23);

    // Los botones de los jugadores empiezan sin avisar
    PCMSK1 &= ~((1<<PCINT12) | (1<<PCINT13));

    // Anotamos el estado real de cada pin al arrancar
    estadoInicio = !(PIND & (1<<PD7));
    estadoJugador1 = !(PINC & (1<<PC4));
    estadoJugador2 = !(PINC & (1<<PC5));
}

void Botones_HabilitarJugadores(void)
{
    // Antes de encender el aviso, descartamos cualquier cosa
    // pendiente y releemos el estado actual, para que una
    // pulsación hecha mientras estaban apagados no cuente ahora
    avisoJugador1 = 0;
    avisoJugador2 = 0;
    estadoJugador1 = !(PINC & (1<<PC4));
    estadoJugador2 = !(PINC & (1<<PC5));

    PCMSK1 |= (1<<PCINT12) | (1<<PCINT13);
}

void Botones_DeshabilitarJugadores(void)
{
    PCMSK1 &= ~((1<<PCINT12) | (1<<PCINT13));
}

uint8_t BotonInicioPresionado(void)
{
    uint8_t detectado = 0;

    if (!avisoInicio)
    {
        return 0; // nada nuevo que revisar
    }

    if ((Timer0_Millis() - horaInicio) > TIEMPO_ANTIRREBOTE_MS)
    {
        uint8_t estadoActual = !(PIND & (1<<PD7));
        avisoInicio = 0;

        if (estadoActual != estadoInicio)
        {
            estadoInicio = estadoActual;
            if (estadoInicio == 1)
            {
                detectado = 1;
            }
        }
    }

    return detectado;
}

uint8_t BotonJugador1Presionado(void)
{
    uint8_t detectado = 0;

    if (!avisoJugador1)
    {
        return 0;
    }

    if ((Timer0_Millis() - horaJugador1) > TIEMPO_ANTIRREBOTE_MS)
    {
        uint8_t estadoActual = !(PINC & (1<<PC4));
        avisoJugador1 = 0;

        if (estadoActual != estadoJugador1)
        {
            estadoJugador1 = estadoActual;
            if (estadoJugador1 == 1)
            {
                detectado = 1;
            }
        }
    }

    return detectado;
}

uint8_t BotonJugador2Presionado(void)
{
    uint8_t detectado = 0;

    if (!avisoJugador2)
    {
        return 0;
    }

    if ((Timer0_Millis() - horaJugador2) > TIEMPO_ANTIRREBOTE_MS)
    {
        uint8_t estadoActual = !(PINC & (1<<PC5));
        avisoJugador2 = 0;

        if (estadoActual != estadoJugador2)
        {
            estadoJugador2 = estadoActual;
            if (estadoJugador2 == 1)
            {
                detectado = 1;
            }
        }
    }

    return detectado;
}

/*-------------------------------------------------------------
    Interrupciones: el microcontrolador salta aquí solo, apenas
    detecta un cambio en alguno de los pines vigilados.
-------------------------------------------------------------*/

// Aviso del grupo PD. Solo PD7 (inicio) está activado aquí.
ISR(PCINT2_vect)
{
    horaInicio = milisegundosTranscurridos;
    avisoInicio = 1;
}

// Aviso del grupo PC. Pueden estar activados PC4 y PC5 al mismo
// tiempo, y el microcontrolador no distingue cuál de los dos
// cambió dentro de este mismo aviso. Por eso revisamos ambos:
// el que en realidad no cambió, no generará detección más
// adelante en BotonJugadorXPresionado().
ISR(PCINT1_vect)
{
    horaJugador1 = milisegundosTranscurridos;
    avisoJugador1 = 1;

    horaJugador2 = milisegundosTranscurridos;
    avisoJugador2 = 1;
}