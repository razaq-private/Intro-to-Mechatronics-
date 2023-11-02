/* Name: main.c
 * Author: Abudurazaq Aribidesi 
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "t_usb.h"
#define PRINTNUM(x) m_usb_tx_uint(x); m_usb_tx_char(10); m_usb_tx_char(13)
//50Hz frequency should have value of 16,000,000/8/50
#define VALUE 40000

int main(void){
    teensy_clockdivide(0);
    m_usb_init();
    //use pin for timer 1 channel A
    set(DDRB, 5);
    
    double clockwise_duty = 0.50;
    double counterclockwise_duty = 0.11;
    //attempt to use timer1 becuase it has 2 timers for the 2 motors
    set(TCCR1B, WGM13);
    set(TCCR1B, WGM12);
    set(TCCR1A, WGM11);
    clear(TCCR1A, WGM10);

    //set the prescaler to be 1/1024
    //try dividing by 1/8
    clear(TCCR1B, CS12); 
    set(TCCR1B, CS11);
    clear(TCCR1B, CS10);

    //clear timers
    set(TCCR1A, COM1A1);
    clear(TCCR1A, COM1A0);

    ICR1 = VALUE;

    //Set timer to have the same perod/frequency that is given for 50 Hz
    //OCR1A = VALUE;

    for(;;){
        //OCR1A = 0;
        //OCR1A = 2000; 
        //OCR1A = 0;
        OCR1A = 3333;
        for (int i = 0; i<1900; i=i+100){
            OCR1A = i+2000;
            //teensy_wait(500);
            m_usb_tx_string("\rServo OCR1A: ");
            PRINTNUM(OCR1A);
        }
         for (int i = 2000; i>100; i=i-100){
            OCR1A = i+2000;
            //teensy_wait(500);
            m_usb_tx_string("\rServo OCR1A: ");
            PRINTNUM(OCR1A);
        }

    
     }
    
    return 0;   /* never reached */
}

//servo: control has PWM of 50 Hz
//5.5% duty cycle is full clockwise = 1ms
//11.1% duty cycle is full counter clockwise = 2ms
// mode 7 goes up to 1023 or 0x03FF
// mode 14 goes to up to ICR3 or what we preset 
