#include <xc.h>

// Configuración del PIC12F675
#pragma config FOSC = INTRCIO   // Oscilador interno 4MHz
#pragma config WDTE = OFF       // Watchdog apagado
#pragma config PWRTE = ON       // Power-up Timer
#pragma config MCLRE = OFF      // GP3 como entrada (no reset)

#define _XTAL_FREQ 4000000

void main(void) {
    // Inicialización de registros
    ANSEL  = 0x00;       // Todo digital
    CMCON  = 0x07;       // Comparadores apagados
    TRISIO = 0x00;       // Todos los pines como salida
    GPIO   = 0x00;       // Iniciar todo apagado

    unsigned char brillo = 0;
    signed char paso = 1;     // Indica si sube (+1) o baja (-1)
    unsigned char i = 0;

    while(1) {
        // --- Generación de PWM por Software ---
        // Un ciclo rápido de 100 pasos para engañar al ojo
        for(i = 0; i < 100; i++) {
            if(i < brillo) {
                GP2 = 1; // Encendido
            } else {
                GP2 = 0; // Apagado
            }
        }

        // --- Control de la Rampa (Fade) ---
        // 'divisor' controla qué tan lento es el efecto. 
        // A mayor número, más místico y pausado.
        static unsigned char divisor = 0;
        if(++divisor > 10) { 
            brillo += paso;
            
            // Si llega al tope (100) o al mínimo (0), invierte dirección
            if(brillo >= 100 || brillo <= 0) {
                paso *= -1;
            }
            divisor = 0;
        }
    }
}