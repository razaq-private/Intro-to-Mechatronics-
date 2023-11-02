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
        //PRINTNUM(pinstate);
        set(PIND, 3);
        m_usb_tx_string("\n Button Pressed: ");
        m_usb_tx_uint(TCNT3);
    } /*else if(pinstate == oldstate){
        clear(PIN)
    }*/
    oldstate = pinstate; 
}

int main(void){
    teensy_clockdivide(0); //set output to be 16 MHz
    m_usb_init();
    set(DDRD, 6);
    set(PORTD, 6);

    set(TCCR3B, CS32);
    clear(TCCR3B, CS31);
    set(TCCR3B, CS30);


    while(1){
        checkPB4();
    }
    return 0;   /* never reached */
}
