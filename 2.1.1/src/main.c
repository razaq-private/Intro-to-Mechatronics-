/* Name: main.c
 * Author: Abudurazaq Aribidesi 
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "t_usb.h"
#define PRINTNUM(x) m_usb_tx_uint(x); m_usb_tx_char(10); m_usb_tx_char(13)

void checkPB4(){
    static int oldstate; 
    int pinstate = !bit_is_set(PINB, 4);

    if(pinstate != oldstate){
        PRINTNUM(pinstate);
        set(PIND, 3);
    }
    oldstate = pinstate; 
}

int main(void){
    teensy_clockdivide(0); //set output to be 16 MHz
    m_usb_init();
    set(DDRD, 6)
    set(PORTD, 6);
    while(1){
        checkPB4();
        
    }
    return 0;   /* never reached */
}
