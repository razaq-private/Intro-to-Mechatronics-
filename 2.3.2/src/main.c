/* Name: main.c
 * Author: Abudurazaq Aribidesi 
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "t_usb.h"
#define PRINTNUM(x) m_usb_tx_uint(x); m_usb_tx_char(10); m_usb_tx_char(13)
//prescaler is 1/1024 the amount of ticks 
//for 23 Hz -> 16M/1024/23 = 679
//for 700 Hz -> 16M/1024/700 = 22
#define Hz23 679
#define Hz700 22

unsigned int tperiod;
unsigned int oldtime; 
//get the frequency outputted from IR LEd
void frequency(){
    while(bit_is_clear(TIFR3, ICF3));
    set(TIFR3, ICF3);
    tperiod = ICR3 - oldtime;
    oldtime = ICR3;

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

    //clear the timer 
    clear(TCCR3B, ICES3);
    //lower the flag initially 
    set(TIFR3, ICF3);
    

    set(DDRB, 4); //set blue LED to output
    set(DDRB, 6); //set green LED to output
    clear(DDRC, 7); //clear timer to start at 0

    TCNT3 = 0;
    
    
    for(;;){
        //get values from frequency function 
        frequency();
        //PRINTNUM(tperiod);
        //make sure that if there is no signal and it's been more than a second, turn of the LED
         while(!bit_is_set(TIFR3, ICF3)){
             if(TCNT3-oldtime > 1000){
                clear(PORTB,4);
                clear(PORTB, 6);
             }
         }
        // m_usb_tx_uint(1);

    //conditionals to check if the Hz and if so light up an LED
        if (tperiod >= (Hz23*0.95) && tperiod <= (Hz23*1.05)){
            set(PORTB, 4);
            clear(PORTB, 6);
            //PRINTNUM(23);
        } else if (tperiod >= (Hz700*0.95) && tperiod <= (Hz700*1.05)) {
            clear(PORTB, 4);
            set(PORTB, 6);
            //PRINTNUM(700); //just to check if it is working
        }else{
            clear(PORTB, 4);
            clear(PORTB, 6);           
        }
    }    
    return 0;   /* never reached */
}
