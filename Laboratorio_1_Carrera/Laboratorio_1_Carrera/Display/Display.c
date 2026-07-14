/*
 * Display.c
 *
 * Created: 11/07/2026 18:44:54
 *  Author: Diego Cardona
 */ 
#include <avr/io.h>
#include "Display.h"

/*-------------------------------------------------------------
    Display de 7 segmentos - conectado en PD0 a PD6

    segmento a -> PD0        segmento e -> PD4
    segmento b -> PD1        segmento f -> PD5
    segmento c -> PD2        segmento g -> PD6
    segmento d -> PD3

    Se asume display de CÁTODO COMÚN: segmento encendido = 1
-------------------------------------------------------------*/

// Un patrón de bits por cada número (bit0=a, bit1=b, ... bit6=g)
const uint8_t tablaNumeros[10] = //En esta la lista van cada uno de los bits para mostrar en el display
{
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

void Display_Init(void)
{
    // PD0 y PD1 son también RXD/TXD (usados por el UART). Como el display los necesita como salidas normales, apagamos el periférico de UART para que no interfiera con esas líneas.
    UCSR0B = 0x00;

    // Configuración de Salidas del Display (PD0 a PD6)
    DDRD |= (1<<PD0);
    DDRD |= (1<<PD1);
    DDRD |= (1<<PD2);
    DDRD |= (1<<PD3);
    DDRD |= (1<<PD4);
    DDRD |= (1<<PD5);
    DDRD |= (1<<PD6);

    Display_Apagar(); //Función para apagar el display
}

void Display_Mostrar(uint8_t numero)
{
    if (numero > 9)
    {
        return; // Si es mayor a 9 detecta que existe un número fuera de rango, regresa a la función principal
    }

    // Escribimos el patrón sin afectar los demás pines de PORTD
    PORTD = (PORTD & 0x80) | (tablaNumeros[numero] & 0x7F);
}

void Display_Apagar(void)
{
    PORTD &= 0x80; // Apaga solo los segmentos (PD0-PD6), deja PD7 intacto el cual es el botón de inicio
}