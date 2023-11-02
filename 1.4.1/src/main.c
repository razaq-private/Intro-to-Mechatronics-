/* Name: main.c
 * Author: Abudurazaq Aribidesi 
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
//try to choose the slowest cycle which is 1/1024
//set the frequency to 100Hz so that the blinks arent noticble to the human eye 
#define VALUE 156

int main(void)
{
    teensy_clockdivide(0); //set output to be 16 MHz
    //set PC6 as output 
    set(DDRC, 6);
    //double duty = 1.0;
    //prescale to the slowest timer which is 1/1024
    set(TCCR3B, CS32);
    set(TCCR3B, CS30);

    //set the wave modulators to have PMW 
    set(TCCR3B, WGM33);
    set(TCCR3B, WGM32);
    set(TCCR3A, WGM31);
    clear(TCCR3A, WGM30);
    
    // clear OCR3A
    set(TCCR3A, COM3A1);
    clear(TCCR3A, COM3A0);
    
    ICR3 = VALUE;
    // 10 steps * 100 ms = 1 second 
    //for 1 sec, if 0.3s is for on can change either the amount of steps or 
    // change the amount of steps based on how much time you want it to take 
    //take the duty that the light is at in order to incrament it later
    float current_duty = 0.0;
    //the max duty cycle which is 100
    float duty = 1.0;
    //incrase the duty cycle by 10% each time 
    float duty_incrament = duty/10;
    //spend 0.3 seconds lighting up to max intensinty 
    int time_light_up = (int)((0.3 / 10)*1000);
    //spend 0.7 seconds changing the intensity back to 0
    int time_light_down = (int)((0.7 / 10)*1000);
    for(;;){
        //increase the intensity continuously from 0.0 to the amount of seconds or steps you want.
        for (float i = 0.0; i <= 10.0; i++){
            current_duty = duty_incrament*i;
            OCR3A = VALUE * current_duty;
            teensy_wait(time_light_up);   
        }
        
        set(TCCR3A, COM3A1);
        //to fade start at the steps needed and go to 0
        //start decreasing from the amount of steps left after increasing
        for (float i = 10; i>=0.0; i--){
            //set the output to be based on the amount of steps left
            current_duty = duty_incrament*i;
            OCR3A = VALUE * current_duty;
            teensy_wait(time_light_down);  
        }
        teensy_wait(500);
        set(TCCR3A, COM3A1);
    }
    
    return 0;   /* never reached */
}
/*
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
} */