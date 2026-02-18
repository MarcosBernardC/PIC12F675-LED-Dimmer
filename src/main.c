#include <xc.h>

#pragma config FOSC = INTRCIO // Oscilador interno
#pragma config WDTE = OFF     // Watchdog apagado
#pragma config PWRTE = ON     // Power-up Timer encendido
#pragma config MCLRE = OFF    // Pin GP3 como entrada (no reset)

#define _XTAL_FREQ 4000000 // 4MHz

void main(void) {
  ANSEL = 0x00;  // Digital I/O
  CMCON = 0x07;  // Comparadores apagados
  TRISIO = 0x00; // Todo como salida
  GPIO = 0x00;   // Todo apagado

  while (1) {
    GP2 = 1; // Enciende el LED azul
    __delay_ms(500);
    GP2 = 0; // Apaga el LED azul
    __delay_ms(500);
  }
}