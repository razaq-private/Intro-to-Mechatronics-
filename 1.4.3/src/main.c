/* Name: main.c
 * Author: Abudurazaq Aribidesi 
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
//try to choose the slowest cycle which is 1/1024
#define VALUE 156
//initialize the functions
void light_increase(float fade_in_time, float max_intensity);
void light_decrease(float fade_out_time, float max_intensity);

int main(void)
{
    teensy_clockdivide(0); //set output to be 16 MHz
    //set PC6 as output 
    set(DDRC, 6);
    //double duty = 1.0;
    //prescale to the slowest timer which is 1/1024
    set(TCCR3B, CS32);
    clear(TCCR3B, CS31);
    set(TCCR3B, CS30);

    //set the wave modulators to have PMW 
    set(TCCR3B, WGM33);
    set(TCCR3B, WGM32);
    set(TCCR3A, WGM31);
    clear(TCCR3A, WGM30);
    
    // clear OCR3A
    set(TCCR3A, COM3A1);
    clear(TCCR3A, COM3A0);
   
    //sets the period
    ICR3 = VALUE;
    
    for(;;){
        //high intensity was 1.0 so a full duty cycle
        float high;
        //lower intensity started at 50 or 5.0 for the duty cycle 
        float low;
        for(float k = 20.0; k>=0.0; k--){
            high = k/20.0; //start at this to have duty cycle of 1.0
            low = k/40.0;
            light_increase(0.1, high);
            light_decrease(0.4,high);
            light_increase(0.1, low);
            light_decrease(0.4,low);
            teensy_wait(2000);
        } 
    }
    return 0;   /* never reached */
}
// create a function that is meant to slowly light up based on the time given and the intensity 
void light_increase(float fade_in_time, float max_intensity){
    // want to take 10 steps in total
    int time_in = (int)((fade_in_time / 10)*1000);
    //use the duty cycle to be the max_intensity
    float duty = max_intensity;
    //find how to increase the duty cycle over the amount of steps (in this case 10 steps)
    float duty_incrament = duty/10;
    //set the current duty to start from 0 and continue to increase
    float current_duty = 0;
    
    for (int i = 0; i<=10;i++){
        //increase the duty for each run to increase the brightness 
        current_duty  = duty_incrament*i;
        // match the frequency to the pmw 
        OCR3A = (int)(156 * current_duty);
        teensy_wait(time_in);
    }
}

void light_decrease(float fade_out_time, float max_intensity){
    // want to take 10 steps in total
    int time_out = (int)((fade_out_time / 10)*1000);
    //use the duty cycle to be the max_intensity
    float duty = max_intensity;
    //find how to increase the duty cycle over the amount of steps (in this case 10 steps)
    float duty_incrament = duty/10;
    //set the current duty to start from 0 and continue to increase
    float current_duty = max_intensity;
    
    for (int i = 10; i>=0; i--){
        //use the i variable instead to change the duty cycle; starts at 1.0 and changes based on how much the max intensity is 
        current_duty  = duty_incrament*i;
        //cast the PMW to read as an in
        OCR3A = (int)(156 * current_duty);
        //wait a differnt amount of time based on how long it should fade out for 
        teensy_wait(time_out);
    }
}