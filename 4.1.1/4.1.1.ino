//set the pins for the LED
int LED = 14;
int Switch = 12;
int LED_State = HIGH;
int Switch_State = LOW;
int previous_State = LOW; 

void setup() {
  // put your setup code here, to run once:
  //make the pin an input
  Serial.begin(115200);
  pinMode(Switch, INPUT);

  //set the LED switch to be an Output
  pinMode(LED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Switch_State = digitalRead(Switch);
  if (Switch_State != previous_State){
    if (Switch_State == HIGH) {
      Serial.println("LED high");
      LED_State = HIGH;
    } else {
      LED_State = LOW; 
    } 
  }
  digitalWrite(LED, LED_State);
  previous_State = Switch_State;
}
