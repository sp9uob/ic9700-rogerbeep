/* 
 * File:   main.c
 * Author: SP9UOB
 *
 * Created on 5 marca 2021, 22:10
 */

#define _XTAL_FREQ 4000000
#pragma config WDTE = OFF, PWRTE = OFF, CP = OFF, BOREN = OFF, MCLRE = OFF, CPD = OFF, FOSC = INTRCIO


#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>


// Port Shadowing  
volatile uint8_t sGPIO=0; // Port shadow register (global)  
#define sGPIObits (*(GPIObits_t * volatile) &sGPIO)  
#define updGPIO()   (GPIO = sGPIO)  


#define AUDIOTRIS TRISIObits.TRISIO5
#define AUDIOPORT sGPIObits.GP5

#define PTT_TRIS  TRISIObits.TRISIO0
#define PTT_PORT  sGPIObits.GP0

#define SEND_TRIS TRISIObits.TRISIO2
#define SEND_PORT GPIObits.GP2 

#define F_BEEPON    0x80
#define F_SEND      0x01
#define F_LASTSEND  0x02


volatile uint16_t beeplen;
volatile uint8_t flags=0;



void interrupt isr (void) {
    
    if (INTCONbits.T0IF) {

    	// 1750 Hz tone	    
        TMR0=0xb7;
        
        updGPIO();
        
        if (flags&F_BEEPON) {
            AUDIOPORT^=1;
            if (--beeplen==0)  {
                flags&=~F_BEEPON;
                PTT_PORT=0;
            };
        };
        
        
        INTCONbits.T0IF=0;
    }
    
}


int main(int argc, char** argv) {
    
  uint8_t i;
  uint8_t  last,now;  
  
  
    ANSEL=0;
    CMCONbits.CM=0b111;
    
    AUDIOTRIS=0;
    PTT_TRIS=0;
    SEND_TRIS=1;
    GPIO=0;
            
    //OPTION_REGbits.PSA=0;
    OPTION_REG=0b11000001;

    INTCONbits.T0IF=0;
    INTCONbits.PEIE=1;
    INTCONbits.GIE=1;
    
    INTCONbits.T0IF=0;
    INTCONbits.T0IE=1;
    last=1;
    
    __delay_ms(250);
    __delay_ms(250);
    
    while (1) {
        
        last=now;
        
        if(SEND_PORT) {
            now=1;
        } else {
            now=0;
        };
       
        
        if  (last==0 && now==1) {
        

	    // tone duration
  	    beeplen=555;

            flags|=F_BEEPON;

            PTT_PORT=1;
            while (flags&F_BEEPON);
            
            for (i=0;i<3;i++) __delay_ms(25);
            
        }
        
    };
    
    return (EXIT_SUCCESS);
}

