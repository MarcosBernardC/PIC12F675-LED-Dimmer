#include <xc.h>

// Configuración para el PIC12F675
#pragma config FOSC = INTRCIO   // Oscilador interno (No necesitas cristal)
#pragma config WDTE = OFF       // Watchdog apagado
#pragma config PWRTE = ON       // Power-up Timer encendido
#pragma config MCLRE = OFF      // GP3 es entrada digital, no Reset externo

#define _XTAL_FREQ 4000000      // 4MHz interno

void main(void) {
    // Configuración de pines
    ANSEL  = 0x00;              // Todo digital
    CMCON  = 0x07;              // Comparadores apagados
    TRISIO = 0b001000;          // GP3 como entrada (siempre), el resto salidas (incluyendo GP2)
    GPIO   = 0x00;              // Iniciar pines en bajo

    while(1) {
        GPIO = 0b000100;        // Encender GP2 (Pin 5)
        __delay_ms(500);
        GPIO = 0b000000;        // Apagar GP2
        __delay_ms(500);
    }
}