
/* Name: main.c
 * Author: Sarah Ho
 * MEAM 510
 * Lab 3.1 Waldo Input (ADC)
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "t_usb.h" // Initialize Print to terminal
#define PRINTNUM(x) m_usb_tx_uint(x); m_usb_tx_char(10); m_usb_tx_char(13)
#define CH_1 4
#define CH_2 1
void setupChannel(unsigned int ADCchannel);
void readChannel(unsigned int ADCchannel);
void convertIntToBinary(unsigned int input);
int binaryADC[4];

int main(void)
{
    int ch = CH_1;
    m_usb_init(); //Initialize USB communication
    teensy_clockdivide(0); //set the clock speed

    // 10 bit ADC counts up to 1024
    
    // STEP 1: ADC input V range
    // select Vcc
    clear(ADMUX, REFS1);
    set(ADMUX, REFS0);

    // STEP 2: ADC frequency
    // select /128, for 125kHz
    set(ADCSRA, ADPS2);
    set(ADCSRA, ADPS1);
    set(ADCSRA, ADPS0);

    // STEP 3
    setupChannel(CH_1);
    setupChannel(CH_2);

    readChannel(ch);

    // STEP 6: Enable ADC, STEP 7: start converting
    set(ADCSRA, ADEN);
    set(ADCSRA, ADSC);

    for (;;) {
        if (bit_is_set(ADCSRA, ADIF)) { // STEP 8: Wait for conversion to finish and set flag
            set(ADCSRA, ADIF); // reset flag

            if (ch == CH_1) m_usb_tx_string(" \rADC1 = ");
            else m_usb_tx_string(" ADC2 = ");

            m_usb_tx_uint(ADC); // print current ADC register value

            clear(ADCSRA, ADEN); // stop ADC
            // don't need to clear(ADSCRA, ADSC)??

            if (ch == CH_2) { // switch channels
                ch = CH_1;
            } else {
                ch = CH_2;
            }

            readChannel(ch);
            set(ADCSRA, ADEN); // enable ADC
            set(ADCSRA, ADSC); // start converting again

            //set(ADCSRA, ADSC); // step 7 again: start converting again
        }
    }
    return 0;   /* never reached */
}

// STEP 3 disable digital input
void setupChannel(unsigned int ADCchannel) {
    // disable digital input for respective channel
    if (ADCchannel <= 7) {
        set(DIDR0, ADCchannel);
    }
    else {
        set(DIDR2, ADCchannel - 8);
    }
}

void readChannel(unsigned int ADCchannel) {
    // STEP 5:
    // ADEN has been cleared, ok to change
    // to generalize, try to convert channel int or char to binary, indiv. bits
    // int channel_int = ADCchannel - '0'
    convertIntToBinary(ADCchannel); // changes global arr to binary bits
    if (binaryADC[0] == 0) {
        clear(ADCSRB, MUX5);
    }
    else {
        set(ADCSRB, MUX5);
    }
    if (binaryADC[1] == 0) {
        clear(ADMUX, MUX2);
    }
    else {
        set(ADMUX, MUX2);
    }
    if (binaryADC[2] == 0) {
        clear(ADMUX, MUX1);
    } 
    else {
        set(ADMUX, MUX1);
    }
    if (binaryADC[3] == 0) {
        clear(ADMUX, MUX0);
    }
    else {
        set(ADMUX, MUX0);
    }
}

void convertIntToBinary(unsigned int input) {
    int i;
    for (i = 3; i >= 0; i--) {
        binaryADC[i] = input % 2;
        input = input / 2;
    }
}
