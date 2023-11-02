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
    teensy_clockdivide(0);
    m_usb_init();
    //teensy_wait(500);
    m_usb_tx_string("\ninitialized");
    //set reference voltage to be 5V
    set(ADMUX, REFS0);
    clear(ADMUX, REFS1);

    //set the ADC with prescaler 1/128
    set(ADCSRA, ADPS2);
    set(ADCSRA, ADPS1);
    set(ADCSRA, ADPS0);
  
    //disable digital output 
    set(DIDR2, 13);
    
    //use pin F0 as one input for ADC0
    set(ADCSRB, MUX5);
    set(ADMUX, MUX2);
    clear(ADMUX, MUX1);
    set(ADMUX, MUX0);

    //enable ADC from pin c9
    set(ADCSRA, ADEN);

    //start conversion
    set(ADCSRA, ADSC);

    //print to usb
    m_usb_tx_uint(ADC);

    //clear conversion by setting it
    set(ADCSRA, ADIF);
    
    for(;;){
        if(bit_is_set(ADCSRA, ADIF)){ //check if the conversion is complete
           set(ADCSRA, ADIF); //reset the flag
           m_usb_tx_string("\rADC = "); 
           m_usb_tx_uint(ADC); //print ADC value 

           set(ADCSRA, ADSC);
        }
    }
    return 0;   /* never reached */
}
