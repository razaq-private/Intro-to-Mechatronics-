/*
 * MEAM510 HTML510 example w/LED and web slider
 * March 2021
 * 
 * Mark Yim
 * University of Pennsylvania
 * copyright (c) 2021 All Rights Reserved
 */

#include "sliderJS.h"  // contains string "body" html code
#include "html510.h"
HTML510Server h(80);

#define LEDC_CHANNEL 0 
#define LEDC_RESOLUTION_BITS 20
#define LEDC_RESOLUTION  ((1<<LEDC_RESOLUTION_BITS)-1) 
#define LED_PIN 14


const char* ssid = "Razaq Slider";

/*****************/
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 100) {            
  uint32_t duty_cycle =  LEDC_RESOLUTION * min(value, valueMax) / valueMax;   
  ledcWrite(channel, duty_cycle);  // write duty to LEDC 
}

/* web handler   */
void handleRoot() {
  h.sendhtml(body);
}

void changeDuty(){ 
  uint32_t sliderValue = h.getVal();
  String returnString = ("\nDuty Cycle ");
  //Modify the duty cycle of the LED
  ledcAnalogWrite(LEDC_CHANNEL, sliderValue);

  returnString = returnString + (sliderValue/100.0);
  h.sendplain(returnString);
}

//change frequencty with slider
void changeFrequency(){
  uint32_t sliderValue = h.getVal();
  String returnString = "Frequency: ";
  double LedC_frequency = sliderValue/10.0; 
  Serial.print("Frequency: ");
  Serial.println(LedC_frequency);

  //detatch LEDC
  ledcDetachPin(LED_PIN);

  //re-setup LEDC channel
  ledcSetup(LEDC_CHANNEL, LedC_frequency, LEDC_RESOLUTION_BITS);

  //attatch LED_PIN
  ledcAttachPin(LED_PIN, LEDC_CHANNEL); 

  returnString = returnString + LedC_frequency; 
  h.sendplain(returnString);
}


void setup() {      
  Serial.begin(9600);
  WiFi.softAP(ssid);
  WiFi.softAPConfig(IPAddress(192, 168, 1, 105) ,  IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0)); 
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");  Serial.println(myIP);      
             
  h.begin();
  h.attachHandler("/ ",handleRoot);
  h.attachHandler("/duty?val=",changeDuty);
  h.attachHandler("/frequency?val=",changeFrequency);

  pinMode(LED_PIN, OUTPUT);

  //setup ledc 
  ledcSetup(LEDC_CHANNEL, 0.1, LEDC_RESOLUTION_BITS);

  //Attatch led pin to LEDC channel 
  ledcAttachPin(LED_PIN, LEDC_CHANNEL);
}

void loop(){
  h.serve();
  delay(10);
}
