#include <xc.h>

// ---------------------------------------------------------------------------
// CONFIG — PIC12F675, fuses completos
// ---------------------------------------------------------------------------
#pragma config FOSC  = INTRCIO  // Oscilador interno, GP4/GP5 como I/O
#pragma config WDTE  = OFF      // Watchdog Timer desactivado
#pragma config PWRTE = ON       // Power-up Timer activado
#pragma config MCLRE = OFF      // GP3 como entrada digital (no reset)
#pragma config BOREN = ON       // Brown-out Reset activado
#pragma config CP    = OFF      // Sin protección de código
#pragma config CPD   = OFF      // Sin protección de datos EEPROM

#define _XTAL_FREQ 4000000UL

// ---------------------------------------------------------------------------
// Parámetros del efecto
// ---------------------------------------------------------------------------
#define PWM_STEPS   100     // Resolución PWM (0–100)
#define T0_RELOAD   156     // Timer0: tick de 200µs → PWM 50Hz @ 4MHz, prescaler 1:2
#define T1_RELOAD_H 0xF8    // Timer1: tick de 15ms @ 4MHz, prescaler 1:8
#define T1_RELOAD_L 0xED
#define ARCO_DIV    2       // Arco: 1 paso cada 2 ticks → ciclo ~6s
#define VIRGEN_DIV  4       // Virgen: 1 paso cada 4 ticks → ciclo ~12s

// ---------------------------------------------------------------------------
// Variables compartidas ISR ↔ main
// ---------------------------------------------------------------------------

// Base (GP0/GP1) — ola 3s
volatile unsigned char brillo_base   = 0;
volatile signed char   paso_base     = 1;

// Arco (GP2/GP4) — ola 6s, arranca en contrafase con base
volatile unsigned char brillo_arco   = PWM_STEPS;
volatile signed char   paso_arco     = -1;

// Virgen (GP5) — respiración lenta 12s, arranca en brillo medio
volatile unsigned char brillo_virgen = 50;
volatile signed char   paso_virgen   = 1;

// ---------------------------------------------------------------------------
// ISR
// ---------------------------------------------------------------------------
void __interrupt() isr(void) {

    // --- Timer0: PWM tick (200µs) ---
    if(INTCONbits.T0IF) {
        static unsigned char cnt = 0;

        // Base: GP0 y GP1 en contrafase
        GP0 = (cnt < brillo_base)               ? 1 : 0;
        GP1 = (cnt < (PWM_STEPS - brillo_base)) ? 1 : 0;

        // Arco: GP2 y GP4 en contrafase entre sí
        GP2 = (cnt < brillo_arco)               ? 1 : 0;
        GP4 = (cnt < (PWM_STEPS - brillo_arco)) ? 1 : 0;

        // Virgen: GP5 respiración lenta independiente
        GP5 = (cnt < brillo_virgen)             ? 1 : 0;

        if(++cnt >= PWM_STEPS) cnt = 0;

        TMR0 = T0_RELOAD;
        INTCONbits.T0IF = 0;
    }

    // --- Timer1: fade tick (15ms) ---
    if(PIR1bits.TMR1IF) {

        // -- Fade base (cada tick → ciclo ~3s) --
        if(paso_base > 0 && brillo_base >= PWM_STEPS) {
            paso_base = -1;
        } else if(paso_base < 0 && brillo_base == 0) {
            paso_base = 1;
        }
        brillo_base += paso_base;

        // -- Fade arco (cada 2 ticks → ciclo ~6s) --
        static unsigned char arco_cnt = 0;
        if(++arco_cnt >= ARCO_DIV) {
            arco_cnt = 0;
            if(paso_arco > 0 && brillo_arco >= PWM_STEPS) {
                paso_arco = -1;
            } else if(paso_arco < 0 && brillo_arco == 0) {
                paso_arco = 1;
            }
            brillo_arco += paso_arco;
        }

        // -- Fade virgen (cada 4 ticks → ciclo ~12s) --
        static unsigned char virgen_cnt = 0;
        if(++virgen_cnt >= VIRGEN_DIV) {
            virgen_cnt = 0;
            if(paso_virgen > 0 && brillo_virgen >= PWM_STEPS) {
                paso_virgen = -1;
            } else if(paso_virgen < 0 && brillo_virgen == 0) {
                paso_virgen = 1;
            }
            brillo_virgen += paso_virgen;
        }

        TMR1H = T1_RELOAD_H;
        TMR1L = T1_RELOAD_L;
        PIR1bits.TMR1IF = 0;
    }
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------
void main(void) {

    ANSEL  = 0x00;
    CMCON  = 0x07;
    TRISIO = 0x00;
    GPIO   = 0x00;

    // Timer0 — preescaler 1:2, fuente interna
    OPTION_REG = 0b00000000;
    TMR0 = T0_RELOAD;
    INTCONbits.T0IE = 1;

    // Timer1 — preescaler 1:8, fuente interna
    T1CON = 0b00110001;
    TMR1H = T1_RELOAD_H;
    TMR1L = T1_RELOAD_L;
    PIE1bits.TMR1IE = 1;
    INTCONbits.PEIE = 1;

    INTCONbits.GIE  = 1;

    while(1) {
        // Base ola 3s | Arco ola 6s | Virgen respiración 12s
    }
}