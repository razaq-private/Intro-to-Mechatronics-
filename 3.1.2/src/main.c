/* Name: main.c
 * Author: Abudurazaq Aribidesi 
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "t_usb.h"
#define PRINTNUM(x) m_usb_tx_uint(x); m_usb_tx_char(10); m_usb_tx_char(13)

//one subroutine to check which channel you are using 
//one subroutine to use the right channel 
//define channels 
#define channel_1 10
//#define channel_2 8
int binary[4]; 

//one idea is to split the channel 
//convert the channel number into binary 
void intToBinary(unsigned int input){
    for (int i = 3; i>=0; i--){
        binary[i] = input % 2;
        input = input/2; 
    }
}

void readChannel(unsigned int ADC_Channel){
    //get the binary channel number 
    intToBinary(ADC_Channel);
    //set conditions for what to set/clear based on the channel number 
    if(binary[0] == 1){
        set(ADCSRB, MUX5);
    } else {
        clear(ADCSRB, MUX5);
    }

    if(binary[1] == 1){
        set(ADMUX, MUX2);
    } else {
        clear(ADMUX, MUX2);
    }

    if(binary[2] == 1){
        set(ADMUX, MUX1);
    } else {
        clear(ADMUX, MUX1);
    }

    if(binary[3] == 1){
        set(ADMUX, MUX0);
    } else {
        clear(ADMUX, MUX0);
    }
}
//Disable DC so that you only have ADC
void disableDC(unsigned int ADC_Channel){
    if (ADC_Channel<=7){
        set(DIDR0, ADC_Channel);
    } else {
        set(DIDR2, ADC_Channel); //subtract by 8 because bits do not go up to 8-13
    }
}

int main(void){
    int channel = channel_1;
   //sets the frequency of board to 1 MHz
    teensy_clockdivide(0);
    m_usb_init();
    //teensy_wait(500);
    m_usb_tx_string("\ninitialized");
    disableDC(channel_1);
   

    //set reference voltage to be 5V
    set(ADMUX, REFS0);
    clear(ADMUX, REFS1);

    //set the ADC with prescaler 1/128
    set(ADCSRA, ADPS2);
    set(ADCSRA, ADPS1);
    set(ADCSRA, ADPS0);

    //disable digital output 
    //set(DIDR0, ADC0D); // change based on pin used 
    

    readChannel(channel_1);
  
    //enable ADC from pin c9
    set(ADCSRA, ADEN);
    //start conversion
    set(ADCSRA, ADSC);

    
    
    for(;;){
        if(bit_is_set(ADCSRA, ADIF)){ //check if the conversion is complete
           set(ADCSRA, ADIF); //reset the flag
           if(channel == channel_1) m_usb_tx_string("\rADC1 = "); 
           
           //print out ADC value
           m_usb_tx_uint(ADC);

           clear(ADCSRA, ADEN);

           //repreat by checking the channel
           readChannel(channel);
           //enable ADC again
           set(ADCSRA, ADEN);
           //start converting again 
           set(ADCSRA, ADSC);
          
        }
    }
    return 0;   /* never reached */
}
