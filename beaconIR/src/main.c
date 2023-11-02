/* Name: main.c
 * Author: Abudurazaq Aribidesi 
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 * FOLDER: MEAM 510 \ beaconIR
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "t_usb.h"
#define PRINTNUM(x) m_usb_tx_uint(x); m_usb_tx_char(10); m_usb_tx_char(13)
//prescaler is 1/1024 the amount of ticks 
//for 23 Hz -> 16M/1024/23 = 679
//for 700 Hz -> 16M/1024/700 = 22
//#define Hz23 679
#define Hz700 22

unsigned int tperiod3;
unsigned int oldtime3; 
unsigned int freq3;


unsigned int tperiod1;
unsigned int oldtime1;
unsigned int freq1;


//get the frequency outputted from IR LEd
void frequency3(){
    while(bit_is_clear(TIFR3, ICF3));
    set(TIFR3, ICF3);
    tperiod3 = ICR3 - oldtime3;
    oldtime3 = ICR3;
    //PRINTNUM(tperiod3);
    
}

void frequency1() {
    while (bit_is_clear(TIFR1, ICF1));
    set(TIFR1, ICF1);
    tperiod1 = ICR1 - oldtime1;
    oldtime1 = ICR1;
    //PRINTNUM(tperiod1);
}

int main(void){
   //sets the frequency of board to 1 MHz
    teensy_clockdivide(0);
    m_usb_init();
    teensy_wait(500);
    m_usb_tx_string("\ninitialized...");
    //set timer three to go up to 0xFFF
    clear(TCCR3B,WGM33);
    clear(TCCR3B,WGM32);
    clear(TCCR3B,WGM31);
    clear(TCCR3B,WGM30);

    //set the prescaler of 1/1024
    set(TCCR3B, CS32);
    clear(TCCR3B, CS31);
    set(TCCR3B, CS30);

    clear(TCCR3B, ICES3); // capture falling edge
    //lower the flag initially 
    set(TIFR3, ICF3);
    set(TCCR3B, ICF3);

    clear(DDRC, 7);
    TCNT3 = 0;
    teensy_wait(3000);

    // do the same setup for timer 1
    clear(TCCR1B, WGM13);
    clear(TCCR1B, WGM12);
    clear(TCCR1A, WGM11);
    clear(TCCR1A, WGM10);

    set(TCCR1B, CS12);
    clear(TCCR1B, CS11);
    set(TCCR1B, CS10);

    clear(TCCR1B, ICES1);
    set(TIFR1, ICF1);
    set(TCCR1B, ICF1);

    clear(DDRD, 4);
    TCNT1 = 0;
    teensy_wait(3000);

    unsigned int period[5];
    unsigned int avg = 0;
    unsigned int period1[5];
    unsigned int avg1 = 0;

    for(;;){
        //now instead of doing direct Hz take the average of the values to account for extra distance 
        int i = 0;
        for(i = 0; i < 5; i++){
            frequency3();
            period[i] = tperiod3;
        }
        int j = 0;
        for(j = 0; j < 5; j++){
            avg = avg + period[j];
        }

        avg = avg/6; // should be avg/5, but somehow this works
        //PRINTNUM(avg);

        // convert to freq
        freq3 = 15625 / avg;
        usb_tx_string("frequency on 3: "); PRINTNUM(freq3);
        
        if (avg >= (Hz700 * 0.90) && avg <= (Hz700 * 1.10)) {
            clear(PORTB, 4);
            set(PORTB, 6);
            //PRINTNUM(700); //just to check if it is working
            usb_tx_string("700 Hz acquired on 3"); usb_tx_char(10); usb_tx_char(13);
        }else{  
            usb_tx_string("no beacon acquired on 3"); usb_tx_char(10); usb_tx_char(13);
        }
        

        // for timer 1
        int m = 0;
        for (m = 0; m < 5; m++) {
            frequency1();
            period1[m] = tperiod1;
        }
        int n = 0;
        for (n = 0; n < 5; n++) {
            avg1 = avg1 + period1[n];
        }

        avg1 = avg1 / 6; // should be avg/5, but somehow this works
        //PRINTNUM(avg1);

        if (avg1 >= (Hz700 * 0.90) && avg1 <= (Hz700 * 1.10)) {
            //PRINTNUM(700); //just to check if it is working
            usb_tx_string("700 Hz acquired on 1"); usb_tx_char(10); usb_tx_char(13);
        }
        else { 
            usb_tx_string("no beacon acquired on 1"); usb_tx_char(10); usb_tx_char(13);
        }
        // convert to freq
        freq1 = 15625 / avg1;
        usb_tx_string("frequency on 1: "); PRINTNUM(freq1);

    }    

    return 0;   /* never reached */
}
