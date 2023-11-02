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
    //teensy_clockdivide(4);
    //set pin D7 to output be an output pin
    set(DDRD, 7);

    //set the high and low occurances for the duty cycle
    //when high should output a period not frequency, multiply by 1000 or frequency would be too small 
    double frequency = 3; //in Hz
    double duty = 0.0; 
    int high  = ((1/frequency) * duty) * 1000; //duration it stays high
    int low  = ((1/frequency) * (1-duty)* 1000); //duration it stays low 
    for(;;){
        //sends 5V to port D7
        set(PORTD, 7);
        //waits that amount of time or period to keep the port high
        teensy_wait(high);
        //sends 0V to pin D7
        clear(PORTD,7);
        //waits a certain amount of time to keep the pin at 0V
        teensy_wait(low);
    }
    return 0;   /* never reached */
}

//duty cycle stays off for a certain amount of time and then turns on 

