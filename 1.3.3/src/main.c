/* Name: main.c
 * Author: Abudurazaq Aribidesi 
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#define VALUE 6250
// (16,000,000/(256*20)) * 2 = 6250
//divide by 256*20 because of the prescalar and the desired frequency, multiply by 2 for on and off

int main(void)
{

    teensy_clockdivide(0); // set the teensy to be 16Mhz
    double duty = 0.5;
    set(DDRC, 6); //set Pin C6 to output 5V
    
    //set the wave modulators to have PMW 
    set(TCCR3B, WGM33);
    set(TCCR3B, WGM32);
    set(TCCR3A, WGM31);

    set(TCCR3A, COM3A1);

    //prescale to 1/256
    
    set(TCCR3B, CS32);
    //set a limit that the PMW reaches to be VALUE, and whenever there is a change in the duty cycle it should influence the limit 
    OCR3A = VALUE*duty;
    //whenever you hit the VALUE or that frequency, restart the timer
    ICR3 = VALUE;
    

    for(;;){
        
    }
    
    return 0;   /* never reached */
}
