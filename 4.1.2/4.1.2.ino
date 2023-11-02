int POT = 32; 


#define LEDC_CHANNEL 0 
#define LEDC_RESOLUTION_BITS 10
#define LEDC_RESOLUTION  ((1<<LEDC_RESOLUTION_BITS)-1) 
#define LED_PIN 14
#define LEDC_FREQ_HZ 5000

void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) {            
  uint32_t duty_cycle =  LEDC_RESOLUTION * min(value, valueMax) / valueMax;   
  ledcWrite(channel, duty_cycle);  // write duty to LEDC 
}
void setup() {
  // put your setup code here, to run once:
  ledcSetup(LEDC_CHANNEL, LEDC_FREQ_HZ, LEDC_RESOLUTION_BITS);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint32_t mV = map(analogRead(POT), 0, 4095, 0, 3300);
  Serial.println(mV);

  int duty = map(mV, 0, 3300, 0, 255);
  ledcAnalogWrite(LEDC_CHANNEL, duty);
}
