/*
 * File:   proyecto2main.c
 * Author: David Tobar
 *
 * Created on 13 de octubre de 2021, 08:43 PM
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>

//Variables globales
uint8_t semaforo, start, jugador1, jugador2;
//Prototipos de funciones
void config(void);

void __interrupt()isr(){
    if(PIR1bits.TMR1IF){
        if(start){
            semaforo++;
            if(semaforo>3){
                semaforo=0;
                start=0;
            }
        }
        TMR1H=133;
        TMR1L=238;
        PIR1bits.TMR1IF=0;
    }
    if(INTCONbits.RBIF){
        if(!PORTBbits.RB0){
            start=1;
            semaforo=0;
            PORTC++;
        }else if(!PORTBbits.RB1){
            if(start){
            
            }
            jugador1++;
        }else if(!PORTBbits.RB2){
            jugador2++;
        }
        INTCONbits.RBIF=0;
    }
}

void main(void) {
    config();
    while(1){
        switch(semaforo){
            case 0:
                PORTE=0b110;
                break;
            case 1:
                PORTE=0b010;
                break;
            case 2:
                PORTE=0b000;
                break;
            case 3:
                PORTE=0b001;
                break;
            default:
                PORTE=0b000;
                break;
        }
    }
}

void config(void){
    //------------Configuración de Puertos--------------------------------------
    //Se configuran los puertos como digitales
    ANSEL=0;
    ANSELH=0;
    //Se configuran PORTA,PORTC,PORTD y PORTE como salidas
    TRISA=0;
    TRISC=0;
    TRISD=0;
    TRISE=0;
    //Se configuran RB0-1 son entradas, y RB2-7 son salidas
    TRISB=0b00000111;
    //Se limpian todos los puertos
    PORTA=0;
    PORTC=0;
    PORTD=0;
    PORTE=0;
    //Se configuran RB0-1 con resistencias Pull-Up
    OPTION_REGbits.nRBPU=0;
    WPUB=0b00000111;
    PORTB=0;
    
    //----------Configuración del Oscilador-------------------------------------
    OSCCONbits.SCS=1;
    OSCCONbits.IRCF=0b100;
    
    //----------Configuración de interrupciones---------------------------------
    //Activación de las interrupciones
    INTCONbits.GIE=1;
    INTCONbits.PEIE=1;
    //Se activa la interrupción de TMR1
    PIE1bits.TMR1IE=1;
    PIR1bits.TMR1IF=0;
    //Se activa la interrupciónde PORTB
    INTCONbits.RBIE=0;
    INTCONbits.RBIF=0;
    
    //----------Configuranción de TIMER1----------------------------------------
    T1CONbits.TMR1CS=0;
    T1CONbits.T1CKPS=0b11;
    T1CONbits.TMR1ON=1;
    TMR1H=133;
    TMR1L=238;
    PIR1bits.TMR1IF=0;
    
    //----------Configuración Interrupción de Puerto B--------------------------
    IOCBbits.IOCB0=1;
    IOCBbits.IOCB1=1;
    IOCBbits.IOCB2=1;
    INTCONbits.RBIF=0;
    
    //----------Valor inicial de las variables----------------------------------
    semaforo=0;
    start=0;
    return;
}