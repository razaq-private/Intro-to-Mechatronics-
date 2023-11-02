/* Name: main.c
 * Author: Abudurazaq Aribidesi 
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file

//DDRD = 0x80 means you activate that port to output 5V
//PORTD = 0x80 means the 7th D port has 5V
int main(void)
{
    
    //sets the frequency of board to 1 MHz
    teensy_clockdivide(4);
    //set pin D7 to output 5V
    DDRD = 0x80;
    for(;;){
        int i; 
        //specifies the period of the blinks
        int period = 15000;
        /* insert your main loop code here */
        PORTD ^= 0x80;
        //run the code for a certain amount of times 
        for(i=0;i<period;i++);
    }
    return 0;   /* never reached */
}

//Duty cycle just dictates the amount of time on and off the signal has 
//will have to use teensy_wait