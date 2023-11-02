/* Name: main.c
 * Author: Abudurazaq Aribidesi 
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "t_usb.h"
#define PRINTNUM(x) m_usb_tx_uint(x); m_usb_tx_char(10); m_usb_tx_char(13)

int main(void){
   //sets the frequency of board to 1 MHz
    teensy_clockdivide(0);;
    //set pin D2 to output 5V for the LED
    set(DDRD, 2);
    set(PORTD, 2);

    //set values for the phototransistor 
    set(DDRB, 4);
    set(PORTB, 4);

    //make sure phototransistor does not get signal in the beginning 
    clear(PORTB, 6);
    
    while(1){
        //if the phototransitor sends a signal, light up the LED and if not turn of the LED
        if(bit_is_set(PINB, 6)){
            set(PORTD, 2);
        } else {
            clear(PORTD, 2);
        }
    }
    return 0;   /* never reached */
}
