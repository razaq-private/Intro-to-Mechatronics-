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
#define CHANNEL_1 4
#define CHANNEL_2 1

//got value by doing 16M/8/50Hz for 2ms of ticks
#define VALUE 40000
//array to hold binary array once channel number is converted
int binary[4]; 

//one idea is to split the channel 
//convert the channel number into binary 
void intToBinary(unsigned int input){
    for (int i = 3; i>=0; i--){
        binary[i] = input % 2;
        input = input/2; 
    }
}

//set and clear different parameters to define which channel is being used 
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

//turn of DC from the selectged channels 
void disableDC(unsigned int ADC_Channel){
    if (ADC_Channel<=7){
        set(DIDR0, ADC_Channel);
    } else {
        set(DIDR2, ADC_Channel-8); //subtract by 8 because bits do not go up to 8-13
    }
}

//function that sets up the servo along with the timer 
void setupServo(){
    //timer 1A
    set(DDRB, 5);

    //timer 1B
    set(DDRB, 6);

    //attempt to use timer1 becuase it has 2 timers for the 2 motors
    set(TCCR1B, WGM13);
    set(TCCR1B, WGM12);
    set(TCCR1A, WGM11);
    clear(TCCR1A, WGM10);

   
    //try dividing by 1/8
    clear(TCCR1B, CS12); 
    set(TCCR1B, CS11);
    clear(TCCR1B, CS10);

    //clear timers
    set(TCCR1A, COM1A1);
    clear(TCCR1A, COM1A0);

    //set and rollback for timer B
    //clear timers
    set(TCCR1A, COM1B1);
    clear(TCCR1A, COM1B0);

    //set the period tha the amount of ticks have
    ICR1 = VALUE;
}



int main(void){
    int channel = CHANNEL_1;
   //sets the frequency of board to 1 MHz
    teensy_clockdivide(0);
    m_usb_init();
    //teensy_wait(500);
    m_usb_tx_string("\ninitialized");
    disableDC(CHANNEL_1);
    disableDC(CHANNEL_2);

    //set reference voltage to be 5V
    set(ADMUX, REFS0);
    clear(ADMUX, REFS1);

    //set the ADC with prescaler 1/128
    set(ADCSRA, ADPS2);
    set(ADCSRA, ADPS1);
    set(ADCSRA, ADPS0);

    //set the prescaler for the timer to be 1/1024
    set(TCCR3B, CS32);
    clear(TCCR3B, CS31);
    set(TCCR3B, CS30); 

    //set PWM clock timer 
    
    //call the servo function
    setupServo();

    readChannel(channel);
    //readChannel(CHANNEL_2);

  
    //enable ADC from pin c9
    set(ADCSRA, ADEN);
    //start conversion
    set(ADCSRA, ADSC);

  
    for(;;){
        if(bit_is_set(ADCSRA, ADIF)){ //check if the conversion is complete
            set(ADCSRA, ADIF); //reset the flag
            if(channel == CHANNEL_1) { 
                m_usb_tx_string(" \rADC1 = ");
                m_usb_tx_uint(ADC);
                //teensy_wait(100);
                m_usb_tx_string(" Angle1 = ");
                //int angle = ADC/3;
                //m_usb_tx_uint(angle);
                float float_ADC = (float)(ADC);
                float med = ((float_ADC-160)/170);
                //m_usb_tx_string(" med value = ");
                //usb_tx_decimal(med);
                float ocr = (2000+(med*1400));
                OCR1A = (int)(ocr);
                m_usb_tx_string(" output = ");
                PRINTNUM(OCR1A);
                
           }else { 
                m_usb_tx_string(" ADC2 = "); //print ADC value
                m_usb_tx_uint(ADC);
                //teensy_wait(100);
               // m_usb_tx_string(" Angle1 = ");
                //int angle = ADC/3;
                //m_usb_tx_uint(angle);
                //convert ADC to a float to make it more accurate
                float float_ADC = (float)(ADC);
                //used to just get the value for y = mx+b
                float med = ((float_ADC-160)/170);
                //actual float value of the ocr
                float ocr = (2000+(med*1400));
                OCR1B = (int)(ocr);
                m_usb_tx_string(" output = ");
                PRINTNUM(OCR1B);
           }
           m_usb_tx_uint(ADC);
           clear(ADCSRA, ADEN);
          // create a conditional to switch the channels depending on which is being turned 
          if(channel == CHANNEL_2){
               channel = CHANNEL_1;
          } else { 
               channel = CHANNEL_2;
          }

           readChannel(channel);
           
           set(ADCSRA, ADEN);
           set(ADCSRA, ADSC);
          
        }
    }
    return 0;   /* never reached */
}

//servo: control has PWM of 50 Hz
//5.5% duty cycle is full clockwise = 1ms
//11.1% duty cycle is full counter clockwise = 2ms
// mode 7 goes up to 1023 or 0x03FF
// mode 14 goes to up to ICR3 or what we preset 
