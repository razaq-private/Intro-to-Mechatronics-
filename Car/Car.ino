

/*
 * Hunter Williams, Sarah Ho, Razaq Aribidesi 
 * MEAM 510 | FALL 2021
 * Final Project: Grand Theft Autonomous
 * This Arduino Sketch will handle the motor control of the car & Autonomous Behavior
 * Parts of this code have been adapted from course material found on canvas
 * 
 */

/*
 * Add Libraries & Header Files
 */
 
#include <SoftwareSerial.h>                                   //For Pololu Qik Motor Driver
#include <PololuQik.h>                                        //For Pololu Qik Motor Driver
#include "CarJS.h"                                            //Contains HTML & JS for user control of car
#include "html510.h"                                          //Contains background stuff related to HTML for MEAM 510
#include <WiFi.h>                                             //WiFi library
#include <WiFiUdp.h>                                          //WiFi UDP library
#include <ESP32Servo.h>                                       //ESP32 Servo Library
#include "vive510.h"                                          //HTC Vive library
#include "Adafruit_VL53L1X.h"                                 //TOF sensor         


HTML510Server h(80);                                  //Enable ESP32 Server


/** Define Global Variables **/

//Motor Pins
#define TX 19                                         //Qik Serial Transmit
#define RX 23                                         //Qik Serial Recieve
#define RST 10                                        //Qik RESET (OPTIONAL)


//DIP SWITCH PINS
#define SW1 9                                         //SWITCH #1
#define SW2 10                                        //SWITCH #2
#define SW3 18                                        //SWITCH #3

//Servo PINS
#define SERVO 25                                      //SERVO CONTROL for BEACON

//Vive Pins
#define SIGNALPIN1 32                                  //Vive Detection circuit #1
#define SIGNALPIN2 26                                  //Vive Detection circuit #2

//TOF Pins
#define IRQ_PIN   2                                   //TOF IRQ pin
#define XSHUT_PIN 14                                   //TOF XSHUT pin

//Ultrasonic Pins & Vars
#define TRIG_PIN_FWD 27                               //UltraSonic trigger pin
#define TRIG_PIN_BCK 4                              //UltraSonic trigger pin
#define ECHO_PIN 34                                  //UltraSonic Echo Pin
const double SOUND_SPEED = 0.034;

//Robot ID & Car Position
int ROBOT_NUM;                                        //The Robot ID used for UDP comms
int x1Pos;                                            //The x1 position from vive
int y1Pos;                                            //The y1 position from vive
int x2Pos;                                            //The x2 position from the vive
int y2Pos;                                            //The y2 position from the vive
int angle2Can;                                        //The angle (deg) from car to can
int angle2Beacon;                                     //The angle (deg) from car to beacon
int xCAN;                                             //The broadcasted Coordinates of the can
int yCAN;                                             //The broadcasted Coordinates of the can
int TOF_DIST;                                         //The distance from front of car to wall
int USONIC_DIST1;                                     //The distance from one ultrasonic sensor
int USONIC_DIST2;                                     //The distance from other ultrasonic sensor
int parallel = 2;                                     //0 when ||, 2 when right, 1 when left



//Servo Position & Object
int servoPos = 0;                                     //Stores Servo Position
Servo myServo;                                        //Create Servo Object

//Vive Objects
Vive510 vive1(SIGNALPIN1);                            //Vive coordinate 1 object
Vive510 vive2(SIGNALPIN2);                            //Vive coordinate 2 object

//TOF Object
Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);

PololuQik2s9v1 qik(TX, RX, RST);                      //Initializes Qik Motor Driver
int MOTOR_SPEED = 0;                                  //Motor Speed

//Control Modes
bool WALL_FOLLOWING = false;                          //Enables Wall Following
bool BEACON_TRACKING = false;                         //Enables Beacon Tracking
bool CAN_TRACKING = false;                            //Enables Can Tracking

//Timing Stuff
int time_UDP = millis();    //Time stamp of entering loops
long time_last_serve = millis();
long ms;
char s[13];                 //UDP char array



/** Define ESP32 WiFi **/
const char* ssid = "Group 11's Car";                  //Used to control Car

/** UDP Sender/Receiver WiFi **/
const char* ssidUDP = "TP-Link_05AF";                 //Used for UDP
const char* password = "47543454";                    //Used for UDP



/** web Handlers for Motor Controls **/

/*
 * Handles the root HTML request
 */
void handleRoot() {
  h.sendhtml(body);
}

/*
 * Handles a change to the speed slider.
 * Changes the motor speed of the Motor driver
 */
void handleSpeed() {
  if(WALL_FOLLOWING || BEACON_TRACKING || CAN_TRACKING) {
    //Do nothing
    h.sendplain("");
  } else {
    String ret_string = "Speed: ";
    int sldr_val = h.getVal();
    MOTOR_SPEED = map(sldr_val, 0, 100, 0, 127);
    ret_string = ret_string + sldr_val;
    h.sendplain(ret_string);
    Serial.print("Slider Value: " ); Serial.println(sldr_val);
  }
}

int leftstate, rightstate, forward;
//long lastleverMs;
/*
 * Handles changes in the motor levers to change direction/turning
 */
void handleLever() {
  String s;
  //Check if in autonomous mode and if so do nothing.
  if(WALL_FOLLOWING || BEACON_TRACKING || CAN_TRACKING) {
    //Do nothing
    h.sendplain(s);
  } else {
  
    leftstate = h.getVal();
    rightstate = h.getVal();
    forward = h.getVal();
    s = String(leftstate) + "," + String(rightstate) + ",";

    //Handle Forward/Reverse
    if (forward > 0)            {qik.setSpeeds(-1*MOTOR_SPEED, -1*MOTOR_SPEED);}
    else if (forward < 0)       {qik.setSpeeds(MOTOR_SPEED,MOTOR_SPEED);}
    else                        qik.setCoasts();

    if (forward == 0) {
      //Handle Left Turns
      if (leftstate > 0)          qik.setM1Speed(-1 * MOTOR_SPEED);       //Handle Left Motor
      else if (leftstate < 0)     qik.setM1Speed(MOTOR_SPEED);
      else                        qik.setM1Coast();

      //Handle Right Turns
      if (rightstate > 0)         qik.setM0Speed(-1 * MOTOR_SPEED);     //Handle Right Motor
      else if (rightstate < 0)    qik.setM0Speed(MOTOR_SPEED);
      else                        qik.setM0Coast();     
    }
    
    h.sendplain(s);
    //Serial.printf("received %d %d \n",leftstate, rightstate); // move bot  or something
  }
}

/*
 * HAndle the beacon tracking button
 */
void handleBeaconTrack() {
  BEACON_TRACKING = true;
  WALL_FOLLOWING = false;
  CAN_TRACKING = false;

  Serial.println("Starting Beacon Tracking");
  
  h.sendplain("");
}

/*
 * HAndle the wall following button
 */
void handleWallFollow() {
 
  BEACON_TRACKING = false;
  WALL_FOLLOWING = true;
  CAN_TRACKING = false;
  
  Serial.println("Starting Wall Following");
  
  h.sendplain("");
}

/*
 * HAndle the can tracking button
 */
void handleCanTrack() {
  
  BEACON_TRACKING = false;
  WALL_FOLLOWING = false;
  CAN_TRACKING = true;

  Serial.println("Starting Can Tracking");
  
  h.sendplain("");
}

/*
 * Handle the stop autonomy button
 * Turn BEACON_TRACKING, WALL_FOLLOWING, CAN_TRACKING to False
 */
void handleStopAutonomy() {
  
  BEACON_TRACKING = false;
  WALL_FOLLOWING = false;
  CAN_TRACKING = false;
  qik.setCoasts();
  Serial.println("Stopping Autonomy");
  
  h.sendplain("");
}

/** END of Web Handlers for Motor Controls **/

/** WiFi UDP Stuff **/

//WiFiUDP UDPTestServer;

/** UDP Reciever WiFi **/
WiFiUDP canUDPServer;
WiFiUDP robotUDPServer;

unsigned int UDPPort = 2510; //Port for cans is 1150, port for bots is 2510
IPAddress ipTarget(192,168,1,255); //255 is a broadcast address to everyone at 192.168.1.xxx
IPAddress ipLocal(192,168,1,197); //Replace with your IP address

void fncUdpSend(char *datastr, int len) {
  robotUDPServer.beginPacket(ipTarget, UDPPort);
  robotUDPServer.write((uint8_t *)datastr, len);
  robotUDPServer.endPacket();
}

void handleCanMsg() {
  const int UDP_PACKET_SIZE = 14; //Can be upto 65535
  uint8_t packetBuffer[UDP_PACKET_SIZE];

  int cb = canUDPServer.parsePacket();
  if (cb) {
    int x,y;
    packetBuffer[cb]=0; //null terminate string
    canUDPServer.read(packetBuffer, UDP_PACKET_SIZE);

    x = atoi((char *)packetBuffer+2);
    y = atoi((char *)packetBuffer+7);
    if (atoi((char *)packetBuffer) == 1) {
      xCAN = x;
      yCAN = y; 
    }

/*
    Serial.print("From Can ");
    Serial.println((char *)packetBuffer);
    Serial.println(x);
    Serial.println(y);
*/
  }
}

void handleRobotMsg() {
  const int UDP_PACKET_SIZE = 14; //Can be upto 65535
  uint8_t packetBuffer[UDP_PACKET_SIZE];

  int cb = robotUDPServer.parsePacket();
  if (cb) {
    int x,y;
    packetBuffer[cb]=0; // null terminate string
    robotUDPServer.read(packetBuffer, UDP_PACKET_SIZE);

    x = atoi((char *)packetBuffer+2);
    y - atoi((char *)packetBuffer+7);
    /*
    Serial.print("From Robot ");
    Serial.println((char *)packetBuffer);
    Serial.println(x);
    Serial.println(y);
    */
  }
}



/*
 * calcTheta(int x1, int y1, int x2, int y2, int x3, int y3)
 * takes in three coordinate pairs and calculates the angle between vectors
 * A and B where A is formed by (x1,y1) and (x2,y2) and B is formed by (x3,y3) and (x2,y2)
 */
double calcTheta(int x1, int y1, int x2, int y2, int x3, int y3) {
  int Ax = x1-x2; int Ay = y1-y2; int Bx = x3-x2; int By = y3-y2;
  double theta = atan2(By*Ax - Bx*Ay, Bx*Ax+By*Ay);
  //Serial.print("THETA (RAD): "); Serial.println(theta);
  theta = theta * 180.0 / PI;
  return(theta);
}

/*
 * 
 * 
 */
double dist2(int x1, int y1, int x2, int y2){
  double dist = sqrt(pow(x1-x2,2)+pow(y1-y2,2));
  return dist;
}

void readTOF() {
  if (vl53.dataReady()) {
    // new measurement for the taking!
    TOF_DIST = vl53.distance();
    if (TOF_DIST == -1) {
      // something went wrong!
      Serial.print(F("Couldn't get distance: "));
      //Serial.println(vl53.vl_status);
      return;
    }
    Serial.print(F("Distance: "));
    Serial.print(TOF_DIST);
    Serial.println(" mm");

    // data is read out, time for another reading!
    vl53.clearInterrupt();
  }
}

/*
 * Read both Ultrasonic sensors to determine the cars orientation to the wall
 */
void checkParallel() {
  //Read the fwd sensor
  digitalWrite(TRIG_PIN_FWD, LOW);                //Clear TRIG Pin
  delayMicroseconds(2);                           //Wait
  digitalWrite(TRIG_PIN_FWD, HIGH);               //Ping TRIG Pin
  delayMicroseconds(10);                          //Wait
  digitalWrite(TRIG_PIN_FWD, LOW);                //Clear TRIG Pin
  long duration1 = pulseIn(ECHO_PIN, HIGH);       //Read echo time
  USONIC_DIST1 = duration1 * SOUND_SPEED/2/10;    //Dist in mm
  Serial.print("Forward PORT DIST: "); Serial.println(USONIC_DIST1);
  delay(10);

  //Read the fwd sensor
  digitalWrite(TRIG_PIN_BCK, LOW);                //Clear TRIG Pin
  delayMicroseconds(2);                           //Wait
  digitalWrite(TRIG_PIN_BCK, HIGH);               //Ping TRIG Pin
  delayMicroseconds(10);                          //Wait
  digitalWrite(TRIG_PIN_BCK, LOW);                //Clear TRIG Pin
  long duration2 = pulseIn(ECHO_PIN, HIGH);       //Read echo time
  USONIC_DIST2 = duration2 * SOUND_SPEED/2/10;    //Dist in mm
  Serial.print("Forward PORT DIST: "); Serial.println(USONIC_DIST2);

  if(USONIC_DIST1 > 8 || USONIC_DIST2 < 5) {
    parallel = 3;                                       //Car is too far from wall turn towards it
  }
  else if(USONIC_DIST1 < 5 || USONIC_DIST2 > 8) {
    parallel = 4;
  }
  else if(USONIC_DIST1 > USONIC_DIST2) {                //Car is pointing away from wall
    parallel = 2;
  } else if(USONIC_DIST2 > USONIC_DIST1) {        //Car is pointing towards wall
    parallel = 1;
  } else { parallel = 0;}                         //Car is || to wall

  if(USONIC_DIST1 > 8 && USONIC_DIST2 > 8) {
    parallel = 3;
  } else if(USONIC_DIST1 < 5 && USONIC_DIST2 < 5) {
    parallel=4;
  }else {}
}

 
/*
 * Check service will check the Webpage queue, sync the vives, and handle the robot/can messages
 * 
 */
void CheckService(long ms) {
  //Handle UDP Broadcasts
  handleCanMsg();
  handleRobotMsg();
  
  //Sync VIVES
  if(vive1.status() == VIVE_LOCKEDON) {
      //Add filtering?
      x1Pos = vive1.xCoord();
      y1Pos = vive1.yCoord();
  } else {
      vive1.sync(15); //Try to resync (nonblocking);
    }
  if(vive2.status() == VIVE_LOCKEDON) {
    //Add filtering?
    x2Pos = vive2.xCoord();
    y2Pos = vive2.yCoord();
  } else {
      vive2.sync(15); //Try to resyc (nonblocking);
    }

  sprintf(s,"%1d:%4d,%4d",ROBOT_NUM, x1Pos, y1Pos);
  //Broadcast position Once every second
  ms = millis();
  if(ms - time_UDP >= 1000) {
    time_UDP = ms;
    fncUdpSend(s,13);
    Serial.printf("sending data: %s",s);
    Serial.print("VIVE #1: (X,Y): "); Serial.print(x1Pos); Serial.print(","); Serial.println(y1Pos); 
    Serial.print("VIVE #2: (X,Y): "); Serial.print(x2Pos); Serial.print(","); Serial.println(y2Pos); 
  }
  
  if(ms-time_last_serve >= 10) {
    time_last_serve = ms;
    h.serve();
  }
}

void setup()
{
  /** Qik Serial Motor Control Setup **/
  Serial.begin(9600);
  delay(50);
  Serial.println("WiFi & UDP INITIALIZATION");

  WiFi.mode(WIFI_MODE_STA);      //Group 11's Car Wifi network
  WiFi.config(ipLocal, IPAddress(192,168,1,1), IPAddress(255,255,254,0));
  WiFi.begin(ssidUDP, password);                           
  Serial.print("Connecting to "); Serial.println(ssidUDP);
  delay(100);  
  //UDPTestServer.begin(UDPPort);
  canUDPServer.begin(1510); //can port 1510
  robotUDPServer.begin(2510); //robot port 2510

  while (WiFi.status() !=WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("WiFi Connected\n");
  Serial.print("Router IP: "); Serial.println(WiFi.localIP());
  Serial.print("ESP32 IP: " ); Serial.println(WiFi.softAPIP());

  qik.init();

  Serial.println("POLOLU MOTOR CONTROLLER INITIALIZATION");
  Serial.println("qik 2s9v1 dual serial motor controller");
  Serial.print("Firmware version: ");
  Serial.write(qik.getFirmwareVersion());
  Serial.println("\n");
  delay(100);

  Serial.println("TOF sensor Initialization");
  Wire.begin();
  if (! vl53.begin(0x29, &Wire)) {
    Serial.print(F("Error on init of VL sensor: "));
    Serial.println(vl53.vl_status);
    while (1)       delay(10);
  }
  Serial.println(F("VL53L1X sensor OK!"));

  Serial.print(F("Sensor ID: 0x"));
  Serial.println(vl53.sensorID(), HEX);

  if (! vl53.startRanging()) {
    Serial.print(F("Couldn't start ranging: "));
    Serial.println(vl53.vl_status);
    while (1)       delay(10);
  }
  Serial.println(F("Ranging started"));
  // Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms!
  vl53.setTimingBudget(50);

  //Ultrasonic Sensor Setup
  pinMode(TRIG_PIN_FWD, OUTPUT);
  pinMode(TRIG_PIN_BCK, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  //Vive Setup
  vive1.begin();
  vive2.begin();
  Serial.println("Vive trackers started");


  /** ROBOT ID SETUP **/
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);

  /*
   * To select a robot Number slide dipswitch of the corresponding slider
   * 
   */
  if(digitalRead(SW1) == 0)       ROBOT_NUM = 1;
  else if(digitalRead(SW2) == 0)  ROBOT_NUM = 2;
  else if(digitalRead(SW3) == 0)  ROBOT_NUM = 3;
  else                            ROBOT_NUM = 4;

  Serial.print("ROBOT NUMBER SET TO: "); Serial.println(ROBOT_NUM); Serial.println();
  delay(100);

   
  /** Attatch JS handlers to interact with HTML/JS **/
  h.begin();
  h.attachHandler("/ ", handleRoot);
  h.attachHandler("/speed?val=", handleSpeed);
  h.attachHandler("/lever?val=", handleLever);
  h.attachHandler("/beaconTrack", handleBeaconTrack);
  h.attachHandler("/canTrack", handleCanTrack);
  h.attachHandler("/wallFollow", handleWallFollow);
  h.attachHandler("/stopAutonomy", handleStopAutonomy);

  /** Servo Setup **/
  myServo.setPeriodHertz(50);
  myServo.attach(SERVO, 1000, 2000);        //Attach Servo and set max/min pos in microseconds

  //BEACON_TRACKING = true; //for testing purposes
  //WALL_FOLLOWING = true;  //for testing purposes
  //CAN_TRACKING = true;    //for testing purposes
}
int i = 0;
void loop() {
  
  //This loop handles manual controlling
  while(!WALL_FOLLOWING && !BEACON_TRACKING && !CAN_TRACKING) {
    //h.serve(); //Webpage Interaction
    // store into a string with format #:####,####, which is robotid, x, y
    //sprintf(s,"%1d:%4d,%4d",ROBOT_NUM, x1Pos, y1Pos);
    ms = millis();
    CheckService(ms);
    delay(5);
  }

  //This loop handles wall following mode
  while(WALL_FOLLOWING) {
    //Get Initital Readings
    checkParallel();
    readTOF();

    while(TOF_DIST > 300 && parallel == 0) {
      //Move forward
      Serial.println("MOVE FORWARD");
      qik.setSpeeds(127/2,127/2);
      delay(10);
      //Check Sensors
      checkParallel();
      readTOF();
      ms = millis();
      CheckService(ms);
      if(WALL_FOLLOWING == false) {
        break;
      }
      delay(2);
    }
    
    while(TOF_DIST > 300 && parallel == 1) {
      //Turn back towards wall
      Serial.println("TURN TOWARDS WALL");
      qik.setM1Speed(127/3);
      qik.setM0Coast();
      delay(10);
      checkParallel();
      readTOF();
      ms = millis();
      CheckService(ms);
      if(WALL_FOLLOWING == false) {
        break;
      }
      delay(2);
    }
    
    while(TOF_DIST > 300 && parallel == 2) {
      //Turn back towards wall
      Serial.println("TURN away from WALL");
      qik.setM0Speed(127/3);
      qik.setM1Coast();
      delay(10);
      checkParallel();
      readTOF();
      ms = millis();
      CheckService(ms);
      if(WALL_FOLLOWING == false) {
        break;
      }
      delay(2);
    }

    while(TOF_DIST > 300 && parallel == 3) {
      //Turn towards wall and forward a bit
      Serial.println("Car needs to go forward and to wall");
      qik.setSpeeds(127/4,127/2);
      checkParallel();
      readTOF();
      ms = millis();
      CheckService(ms);
      if(WALL_FOLLOWING == false) {
        break;
      }
      delay(30);
    }

    while(TOF_DIST > 300 && parallel == 4) {
      //Turn away from wall and forward a bit
      Serial.println("Car needs to go forward and away from wall");
      qik.setSpeeds(127/2,127/4);
      checkParallel();
      readTOF();
      ms = millis();
      CheckService(ms);
      if(WALL_FOLLOWING == false) {
        break;
      }
      delay(30);
    }
    
    while(TOF_DIST > 0 && TOF_DIST < 300) {
      Serial.println("CAR NEEDS TO TURN 90");
      qik.setCoasts();
      delay(25);
      checkParallel();
      readTOF();
      ms = millis();
      CheckService(ms);
      if(WALL_FOLLOWING == false) {
        break;
      }
      qik.setM1Speed(127/3);
      delay(75);
    }
  }
  
  //This loop handles the Beacon Tracking mode

  while(BEACON_TRACKING) {
    //Add Beacon Tracking Code
     bool locked = false;
     //Sweep servo until IR sensors are "locked" onto Beacon
     while (!locked) {
        for (servoPos = 0; servoPos <= 180; servoPos += 1) { // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          myServo.write(servoPos);    // tell servo to go to position in variable 'pos'
          delay(5);             // waits xxx ms for the servo to reach the position
          //Check if IR Sensors are high if so exit loop
        }
        for (servoPos = 180; servoPos >= 0; servoPos -= 1) { // goes from 180 degrees to 0 degrees
          myServo.write(servoPos);    // tell servo to go to position in variable 'pos'
          delay(5);             // waits xxx ms for the servo to reach the position
          //Check if IR sensors are high is so exit loop
        }
        
        h.serve(); //Webpage Interaction
  
        char s[13];
        handleCanMsg();
        handleRobotMsg();
  
        // store into a string with format #:####,####, which is robotid, x, y
        sprintf(s,"%1d:%4d,%4d",ROBOT_NUM, x1Pos, y1Pos); //Figure out how to determine position
        fncUdpSend(s,13);
        //Serial.printf("sending data: %s",s);

        if(!BEACON_TRACKING) break;
        delay(100);
     }

        //Code for Beacon tracking after being locked
        h.serve(); //Webpage Interaction
  
        char s[13];
        handleCanMsg();
        handleRobotMsg();
  
        // store into a string with format #:####,####, which is robotid, x, y
        sprintf(s,"%1d:%4d,%4d",ROBOT_NUM, x1Pos, y1Pos); //Figure out how to determine position
        fncUdpSend(s,13);
        Serial.printf("sending data: %s",s);
        delay(100);
  }

  /*
   * This While Loop handles can tracking functionality. It starts by getting an initial angle between the car and can.
   * Then the car will rotate until that angle is within an acceptable delta and move forward to the can's X,Y position.
   */
  while(CAN_TRACKING) {
    Serial.println("Begining CAN TRACKING");
    //If angle2Can > 0 -> Can is to car's left, if less than 0 -> Can is to its right
    
    angle2Can = calcTheta(x1Pos, y1Pos, x2Pos, y2Pos, xCAN, yCAN);
    
    //Loop until Car is aligned with Can
    while (angle2Can > 3 || angle2Can < -3) {
      if(angle2Can > 0) {
        qik.setM0Speed(127/5 *2);                      //Turn Car to the left
        qik.setM1Coast();
      } else {
        qik.setM0Speed(127/5 * 2);                      //Turn Car to the right
        qik.setM1Coast();
      }
      delay(25);                                    //Car will turn for 15 ms
      qik.setCoasts();                              //Stop Car and recalculate angle
      //Update positions
      ms = millis();
      CheckService(ms);                             //Service Webpage & UDP
      if(!CAN_TRACKING) {break;}                    //Break out of loop if user hit Stop Autonomous
      //Recalculate Angle
      angle2Can = calcTheta(x1Pos, y1Pos, x2Pos, y2Pos, xCAN, yCAN);
      Serial.print("ANGLE2CAN: "); Serial.println(angle2Can);
      delay(25);
    }

    //In theory car is aligned with can
    qik.setCoasts();
    delay(500);
    //while ((x1Pos < xCAN - 75 || x1Pos > 75 + xCAN) || (y1Pos < yCAN - 75 || y1Pos > yCAN + 75)) {
    while(dist2(x1Pos,y1Pos,xCAN,yCAN) > 350) {
      qik.setSpeeds(127/2, 127/2);
      ms = millis();
      CheckService(ms);
      if(!CAN_TRACKING) {break;}                    //Break out of loop if user hit Stop Autonomous
      delay(5);
    }
    qik.setCoasts();
    Serial.println("ENDING CAN TRACKING");
    CAN_TRACKING = false;
    delay(50);
  }
 
}
