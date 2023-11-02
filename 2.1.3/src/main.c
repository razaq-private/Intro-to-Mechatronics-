/* Name: main.c
 * Author: Abudurazaq Aribidesi 
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "t_usb.h"
#define PRINTNUM(x) m_usb_tx_uint(x); m_usb_tx_char(10); m_usb_tx_char(13)

unsigned int oldtime;
unsigned int tperiod;

void dosomething(){
    _delay_ms(500);
}

void waitforpress(){

    while(!bit_is_set(TIFR3, ICF3));
    while(bit_is_clear(PINC, 7)); //wait while button is pressed 
    //m_usb_tx_string("\ninitialized...");
    set(TIFR3, ICF3);
    tperiod = ICR3 - oldtime;
    oldtime = ICR3;
    m_usb_tx_string("\n Time elapsed in clicks: ");
    usb_tx_decimal(tperiod);
}

int main(void){
    teensy_clockdivide(0); //set output to be 16 MHz
    m_usb_init();
    teensy_wait(500);
    m_usb_tx_string("\ninitialized...");


    set(DDRD, 2); //set switch pin to output 5V
    clear(DDRC, 7); //clear the timer pin
    set(PORTD, 2);
    set(PORTC, 7);

    //initialize timer mode up tp 0xFFFF
    clear(TCCR3B, WGM33);
    clear(TCCR3B, WGM32);
    clear(TCCR3B, WGM31);
    clear(TCCR3B, WGM30);

    //divide timer3 by prescaler 1024
    set(TCCR3B, CS32);
    clear(TCCR3B, CS31);
    set(TCCR3B, CS30);
 

    set(TIFR3, ICF3);
    clear(TCCR3B, ICES3);
    teensy_wait(1000);

    
    m_usb_tx_string("\nstart in 2 seconds");
    //wait 2 seconds 
    teensy_wait(1500);
    m_usb_tx_string("\n Begin!");
  
    unsigned int timeIntervals [5];
    int count = 0;
    int sum = 0;
    double avg = 0.0;
    int avgMs = 0;
    TCNT3 = 0;
    while (count < 5){
        waitforpress();
        timeIntervals[count] = tperiod;
        count++;
    }

    for (int i = 0; i<5; i++){
        sum = sum + timeIntervals[i];
    }

    avg = sum/5;

    m_usb_tx_string("\n Average time between presses: ");
    usb_tx_decimal(avg);
    avgMs = avg/15625*1000;
    m_usb_tx_string("\nAvg time between presses in milliseconds: ");
    usb_tx_decimal(avgMs);


    for(;;){
    }
    return 0;   /* never reached */
}
