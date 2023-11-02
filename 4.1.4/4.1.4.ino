/*
 * MEAM510 HTML510 example w/LED and web slider
 * March 2021
 * 
 * Mark Yim
 * University of Pennsylvania
 * copyright (c) 2021 All Rights Reserved
 */

#include "buttonJS.h"  // contains string "body" html code
#include "html510.h"
HTML510Server h(80);

//LEDC variables
#define LEDC_CHANNEL 0 
#define LEDC_RESOLUTION_BITS 20
#define LEDC_RESOLUTION  ((1<<LEDC_RESOLUTION_BITS)-1) 


//Motor Pin
#define MOTOR_FORWARD_PIN 10
#define MOTOR_REVERSE_PIN 23

//PWM Pin
#define PWM_PIN 0

const char* ssid = "Razaq button";

void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 100) {
  uint32_t duty_cycle = LEDC_RESOLUTION * min(value, valueMax) / valueMax;
  ledcWrite(channel, duty_cycle); //Write the new duty cycle to ledc
}
/*****************/
/* web handler   */
void handleRoot(){
  h.sendhtml(body);
}

void handleForward(){
   digitalWrite(MOTOR_FORWARD_PIN, HIGH);
   digitalWrite(MOTOR_REVERSE_PIN, LOW);
   Serial.print("Forward");
   h.sendhtml(body); // acknowledge
}

void handleReverse(){
  digitalWrite(MOTOR_FORWARD_PIN, LOW);
  digitalWrite(MOTOR_REVERSE_PIN, HIGH);
  Serial.print("reverse");
  h.sendhtml(body); // acknowledge    
}

void handleSpeed() {
  String returnString = "Speed: ";
  uint32_t sliderValue = h.getVal();
  ledcAnalogWrite(LEDC_CHANNEL, sliderValue);
  returnString = returnString+ (sliderValue / 100.0);

  Serial.print("SPEED: "); Serial.println("" + sliderValue);
  
  h.sendplain(returnString);
}

/*void handleHit(){
  static int toggle;
  if (++toggle%2)  digitalWrite(LEDPIN,HIGH);
  else digitalWrite(LEDPIN,LOW);
  h.sendplain(""); // acknowledge         
}*/


void handleDuty(){ 
  uint32_t sliderValue = h.getVal();
  String returnString = ("\nDuty Cycle ");
  //Modify the duty cycle of the LED
  ledcAnalogWrite(LEDC_CHANNEL, sliderValue);

  returnString = returnString + (sliderValue/100.0);
  h.sendplain(returnString);
}

void setup() {  
  Serial.begin(9600);                
  WiFi.softAP(ssid);
  WiFi.softAPConfig(IPAddress(192, 168, 1, 105),  IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0)); 
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");  Serial.println(myIP);    
             
  h.begin();
  h.attachHandler("/ ",handleRoot);
  h.attachHandler("/duty?val=", handleDuty);
  h.attachHandler("/forward",handleForward);
  h.attachHandler("/reverse",handleReverse);


  pinMode(MOTOR_FORWARD_PIN, OUTPUT);
  pinMode(MOTOR_REVERSE_PIN, OUTPUT);
  pinMode(PWM_PIN, OUTPUT);

  digitalWrite(MOTOR_FORWARD_PIN, LOW);
  digitalWrite(MOTOR_REVERSE_PIN, LOW);

  ledcSetup(LEDC_CHANNEL, 50, LEDC_RESOLUTION_BITS);

  //Attatch Motor PWM to LEDC
  ledcAttachPin(PWM_PIN, LEDC_CHANNEL);
  ledcAnalogWrite(LEDC_CHANNEL, 0);
}

void loop(){
  h.serve();
  delay(10);
}
