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
uint8_t starter, inGame, estado, ganador;

//Prototipos de funciones
void configuracion(void);
uint8_t num_a_display (uint8_t numero);

void __interrupt()isr(){
    if(INTCONbits.RBIF==1){
        if(PORTBbits.RB0==0){
            starter=1;
            estado=0;
            inGame=0;
            PORTC=0;
            PORTD=0;
            PORTBbits.RB3=0;
            PORTBbits.RB4=0;
            ganador=0;
        }
        if(PORTBbits.RB1==0){
            if(inGame){
                PORTC = PORTC << 1;
                if(PORTCbits.RC7){
                    inGame=0;
                    ganador=1;
                    estado=5;
                }
            }
        }
        if(PORTBbits.RB2==0){
            if(inGame){
                PORTD = PORTD << 1;
                if(PORTDbits.RD7){
                    inGame=0;
                    ganador=2;
                    estado=5;
                }
            }
        }
        INTCONbits.RBIF=0;
    }
    if(PIR1bits.TMR1IF){
        if(starter){
            estado++;
            if(estado>3){
                starter=0;
                PORTD=1;
                PORTC=1;
                inGame=1;
            }
        }
        TMR1H = 133;
        TMR1L = 238;            //Se elige un tiempo de 1a
        PIR1bits.TMR1IF=0;      //Se desactiva la bandera
    }
}

void main(void) {
    configuracion();
    while(1){
        switch(estado){
            case 0:
                PORTE=0b000;
                PORTA=0;
                break;
            case 1:
                PORTE=0b110;
                PORTA=num_a_display(3);
                break;
            case 2:
                PORTE=0b010;
                PORTA=num_a_display(2);
                break;
            case 3:
                PORTE=0b000;
                PORTA=num_a_display(1);
                break;
            case 4:
                PORTE=0b001;
                PORTA=num_a_display(0);
                break;
            default:
                PORTE=0b111;
                PORTA=num_a_display(ganador);
                if(ganador==1){
                    PORTBbits.RB3=1;
                }else if(ganador==2){
                    PORTBbits.RB4=1;
                }else{
                    PORTBbits.RB3=0;
                    PORTBbits.RB4=0;
                }
                break;
        }
    }
}

void configuracion(void){
    //---------------------Configuracion de Puertos-----------------------------
    //Las entradas y salidas son digitales
    ANSEL=0;
    ANSELH=0;
    //Los puertos RA,RC,RD y RE serán salidas
    TRISA=0;
    TRISC=0;
    TRISD=0;
    TRISE=0;
    //RB0-2 son entradas, y RB3-7 son salidas
    TRISB=0;
    TRISBbits.TRISB0=1;
    TRISBbits.TRISB1=1;
    TRISBbits.TRISB2=1;
    //Se configuran las resistencias Pull-Up de RB0-2
    OPTION_REGbits.nRBPU=0;
    WPUB=0b00000111;
    //Se limpian los puertos
    PORTA=0;
    PORTB=0;
    PORTC=0;
    PORTD=0;
    PORTE=0;
    
    //---------------------Configuracion del oscilador--------------------------
    //Se configura el oscilador interno a 1MHz
    OSCCONbits.SCS=1;
    OSCCONbits.IRCF=0b100;
    
    //---------------------Configuracion de Interrupciones----------------------
    //Se activan las interrupciones globales y perifericas
    INTCONbits.GIE=1;
    INTCONbits.PEIE=1;
    //Se activa la interrupciónde puerto B
    INTCONbits.RBIE=1;
    //Se activa la interrupción de TMR1
    PIE1bits.TMR1IE=1;
    //Se desactivan las banderas
    INTCONbits.RBIF=0;
    PIR1bits.TMR1IF=0;
    
    //---------------------Configuración del IOCB-------------------------------
    //Los puertos RB0-2 tienen interrupciones de IOCB
    IOCB=0b00000111;
    //Se desactiva el bit RBIF
    INTCONbits.RBIF=0;
    
    //---------------------Configuración de TMR1--------------------------------
    T1CONbits.TMR1CS=0;     //Timer1 usa clock interno
    T1CONbits.T1CKPS=0b11;  //El preescalado es de 1:8
    T1CONbits.TMR1GE=0;     //El puerto de Timer1 se inhabilita
    T1CONbits.TMR1ON=1;     //Se activa TIMER1
    TMR1H = 133;
    TMR1L = 238;            //Se elige un tiempo de 1a
    PIR1bits.TMR1IF=0;      //Se desactiva la bandera
    
    //---------------------Valores iniciales-------------------------------------
    starter=0;
    inGame=0;
    estado=0;
    
    return;
}

uint8_t num_a_display (uint8_t numero){
    switch(numero){
        case 0:
            return 0b00111111;
            break;

        case 1:
            return 0b00000110;
            break;

        case 2:
            return 0b01011011;
            break;

        case 3:
            return 0b01001111;
            break;

        case 4:
            return 0b01100110;
            break;

        case 5:
            return 0b01101101;
            break;

        case 6:
            return 0b01111101;
            break;

        case 7:
            return 0b00000111;
            break;

        case 8:
            return 0b01111111;
            break;

        case 9:
            return 0b01100111;
            break;

        default:
            return 0b00000000;
            break;
    }
}