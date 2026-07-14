/*
 * Laboratorio_1_Carrera.c
 *
 * Created: 11/07/2026 18:43:00
 * Author : Diego Cardona
 */ 

#define F_CPU 16000000UL // Definición de frecuencia a 16 MHz
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Display/Display.h"
#include "Timer/Timer0.h"
#include "Botones/Botones.h"
/*-------------------------------------------------------------
                Estados del juego
-------------------------------------------------------------*/
#define ESPERANDO      0
#define CUENTA         1
#define CARRERA        2
#define FINAL          3
/*-------------------------------------------------------------
                Meta de la carrera
-------------------------------------------------------------*/
#define META 9 // Valor del contador que determina al ganador
/*-------------------------------------------------------------
                Variables globales
-------------------------------------------------------------*/
uint8_t estadoJuego = ESPERANDO; // Variable para el caso 0, de espera
//  // Contador de jugadores
uint8_t jugador1 = 0; // Contador de jugador 1
uint8_t jugador2 = 0; // Contador de jugador 2
uint8_t contador = 5; // Número inicial mostrado durante la cuenta regresiva
uint32_t tiempoAnterior = 0; //  Guarda el tiempo para controlar eventos
uint8_t ganador = 0; // Guarda qué jugador ganó (1 o 2)
/*-------------------------------------------------------------
                Configuración de LEDs
-------------------------------------------------------------*/
void LEDs_Init(void)
{
    // Configuración de Salidas de Leds de Jugador 1
    DDRB |= (1<<PB0);
    DDRB |= (1<<PB1);
    DDRB |= (1<<PB2);
    DDRB |= (1<<PB3);
   // Configuración de Salidas de Leds de Jugador 2
    DDRC |= (1<<PC0);
    DDRC |= (1<<PC1);
    DDRC |= (1<<PC2);
    DDRC |= (1<<PC3);
	// Puertos configurados para que al inicio estén apagados
    PORTB &= ~0x0F;
    PORTC &= ~0x0F;
}
/*-------------------------------------------------------------
                Muestra contadores
-------------------------------------------------------------*/
void MostrarLEDs(void)
{
    // Borrar de LEDs y ponerlas en un estado inicial de apagadas
    PORTB &= ~0x0F;
    PORTC &= ~0x0F;

    // Muestra en binario, en los 4 LEDs de cada jugador, cuántas
    // veces ha presionado su botón (0 a 9)
    PORTB |= (jugador1 & 0x0F);
    PORTC |= (jugador2 & 0x0F);
}
/*-------------------------------------------------------------
                Muestra al ganador
-------------------------------------------------------------*/
void MostrarGanador(uint8_t jugadorGanador)
{
    if (jugadorGanador == 1)
    {
        PORTB |= 0x0F;   // enciende los 4 LEDs del jugador 1
        PORTC &= ~0x0F;  // apaga los 4 LEDs del jugador 2
        Display_Mostrar(1);
    }
    else if (jugadorGanador == 2)
    {
        PORTC |= 0x0F;   // enciende los 4 LEDs del jugador 2
        PORTB &= ~0x0F;  // apaga los 4 LEDs del jugador 1
        Display_Mostrar(2);
    }
}
/*-------------------------------------------------------------
                Programa principal
-------------------------------------------------------------*/
int main(void)
{
    cli(); // mientras configuramos todo, ninguna interrupción nos interrumpe

    Timer0_Init();
    Botones_Init();
    Display_Init();
    LEDs_Init();

    sei(); // listo: ya pueden ocurrir interrupciones

    while (1)
    {
        switch (estadoJuego)
        {
            /*-----------------------------------------------*/
            case ESPERANDO:

                if (BotonInicioPresionado()) //Si esta presionado el botón de inicio (PD7)
                {
                    jugador1 = 0; //El contador de jugador 1 se pone en 0
                    jugador2 = 0; //El contador de jugador 2 se pone en 0
                    MostrarLEDs(); //Se muestra en las LEDs el valor de lo contadores de los jugadores que en este caso es 0 para ambos

                    Botones_DeshabilitarJugadores(); //Deshabilita el presionado de botón para ambos jugadores, en este caso (PC4 y PC5)

                    contador = 5; //Se carga el valor de 5 al contador del display
                    Display_Mostrar(contador); //Se carga el valor del contador al display
                    tiempoAnterior = Timer0_Millis();

                    estadoJuego = CUENTA; //Al finalizar se procede a iniciar la cuenta de los contadores de los jugadores
                }
                break;

            /*-----------------------------------------------*/
            case CUENTA:

                // Cada 1000ms baja un número, SIN detener el programa
                if ((Timer0_Millis() - tiempoAnterior) >= 1000)
                {
                    tiempoAnterior = Timer0_Millis();

                    if (contador == 0)
                    {
                        // Se acabó el conteo: habilita a los jugadores
                        Botones_HabilitarJugadores();
                        estadoJuego = CARRERA;
                    }
                    else
                    {
                        contador--;
                        Display_Mostrar(contador);
                    }
                }
                break;

            /*-----------------------------------------------*/
            case CARRERA:

                if (BotonJugador1Presionado())
                {
                    jugador1++;
                    MostrarLEDs();

                    if (jugador1 >= META)
                    {
                        ganador = 1;
                        Botones_DeshabilitarJugadores();
                        MostrarGanador(ganador);
                        estadoJuego = FINAL;
                    }
                }

                if (BotonJugador2Presionado())
                {
                    jugador2++;
                    MostrarLEDs();

                    if (jugador2 >= META)
                    {
                        ganador = 2;
                        Botones_DeshabilitarJugadores();
                        MostrarGanador(ganador);
                        estadoJuego = FINAL;
                    }
                }
                break;

            /*-----------------------------------------------*/
            case FINAL:

                // El juego terminó: si presionan "Inicio" de nuevo,
                // se reinicia toda la carrera
                if (BotonInicioPresionado())
                {
                    jugador1 = 0;
                    jugador2 = 0;
                    MostrarLEDs();

                    Botones_DeshabilitarJugadores();

                    contador = 5;
                    Display_Mostrar(contador);
                    tiempoAnterior = Timer0_Millis();

                    estadoJuego = CUENTA;
                }
                break;
        }
    }

    return 0;
}