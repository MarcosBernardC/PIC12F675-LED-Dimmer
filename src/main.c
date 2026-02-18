#include <xc.h>

// ---------------------------------------------------------------------------
// Configuración del PIC12F675
// ---------------------------------------------------------------------------
#pragma config FOSC = INTRCIO   // Oscilador interno 4MHz
#pragma config WDTE = OFF       // Watchdog apagado
#pragma config PWRTE = ON       // Power-up Timer
#pragma config MCLRE = OFF      // GP3 como entrada general

#define _XTAL_FREQ 4000000

// ---------------------------------------------------------------------------
// Parámetros del efecto
// ---------------------------------------------------------------------------
#define PWM_STEPS       100     // Resolución del PWM (pasos de 0 a 100)
#define T0_RELOAD       156     // Timer0: recarga para tick de 200µs → PWM 50Hz
#define T1_RELOAD_H     0xF8    // Timer1 HIGH: recarga para paso de fade cada 15ms
#define T1_RELOAD_L     0xED    // Timer1 LOW

// ---------------------------------------------------------------------------
// Variables compartidas entre ISR y main
// ---------------------------------------------------------------------------
volatile unsigned char brillo  = 0;   // Nivel actual (0–100), modificado por T1
volatile signed char   paso    = 1;   // Dirección del fade: +1 sube, -1 baja

// ---------------------------------------------------------------------------
// ISR — Manejador de interrupciones
// ---------------------------------------------------------------------------
void __interrupt() isr(void) {

    // --- Timer0: tick de PWM ---
    // Se dispara cada 200µs. Genera la señal PWM en GP0/GP1 en contrafase.
    if(INTCONbits.T0IF) {
        static unsigned char pwm_cnt = 0;

        GP0 = (pwm_cnt < brillo)               ? 1 : 0;  // Semicírculo A
        GP1 = (pwm_cnt < (PWM_STEPS - brillo)) ? 1 : 0;  // Semicírculo B (contrafase)

        if(++pwm_cnt >= PWM_STEPS) pwm_cnt = 0;

        // Recarga manual del Timer0
        TMR0 = T0_RELOAD;
        INTCONbits.T0IF = 0;    // Limpia bandera
    }

    // --- Timer1: tick de fade ---
    // Se dispara cada 15ms. Avanza un paso en la rampa de brillo.
    if(PIR1bits.TMR1IF) {

        // Verificamos límites ANTES de sumar para evitar desborde en unsigned
        if(paso > 0 && brillo >= PWM_STEPS) {
            paso = -1;
        } else if(paso < 0 && brillo == 0) {
            paso = 1;
        }

        brillo += paso;

        // Recarga del Timer1
        TMR1H = T1_RELOAD_H;
        TMR1L = T1_RELOAD_L;
        PIR1bits.TMR1IF = 0;    // Limpia bandera
    }
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------
void main(void) {

    // Periféricos
    ANSEL  = 0x00;      // Todo digital
    CMCON  = 0x07;      // Comparadores apagados
    TRISIO = 0x00;      // Todos los pines como salida
    GPIO   = 0x00;      // Apagado inicial

    // --- Configuración Timer0 ---
    // Fuente: reloj interno, preescaler 1:2 asignado a TMR0
    OPTION_REG = 0b00000000;  // T0CS=0 (interno), PSA=0 (a TMR0), PS=000 (1:2)
    TMR0 = T0_RELOAD;
    INTCONbits.T0IE = 1;      // Habilita interrupción Timer0

    // --- Configuración Timer1 ---
    // Preescaler 1:8, fuente interna
    T1CON = 0b00110001;       // TMR1ON=1, T1CKPS=11 (1:8), TMR1CS=0 (interno)
    TMR1H = T1_RELOAD_H;
    TMR1L = T1_RELOAD_L;
    PIE1bits.TMR1IE  = 1;     // Habilita interrupción Timer1
    INTCONbits.PEIE  = 1;     // Habilita interrupciones de periféricos

    // --- Interrupciones globales ON ---
    INTCONbits.GIE = 1;

    // Main loop vacío: todo ocurre en la ISR
    while(1) {
        // Aquí vivirá la lógica futura (arco, virgen, modo, etc.)
    }
}