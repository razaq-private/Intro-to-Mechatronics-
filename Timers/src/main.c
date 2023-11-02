/* Name: main.c
 * Author: Abudurazaq Aribidesi 
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
// (16,000,000/(256*20)) * 2 = 6250
//divide by 256*20 because of the prescalar and the desired frequency, multiply by 2 for on and off
#define VALUE 6250
int main(void)
{
    teensy_clockdivide(0); // set the teensy to be 16Mhz
    set(DDRC, 6); //set Pin C6 to output 5V
    
    //set the timers connected to pin c6
    set(TCCR3A, COM3A0);
    set(TCCR3B, WGM32); 
    //want to use prescalar because clock is running at 16 Mhz and should go slower 
    set(TCCR3B, CS30);
    //set(TCCR3B, CS31);
    set(TCCR3B, CS32);
    OCR3A = VALUE;
    
    for(;;){
        
    }
    return 0;   /* never reached */
}
