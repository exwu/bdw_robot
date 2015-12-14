
#include <Adafruit_MotorShield.h>

#include <Wire.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *rf = AFMS.getMotor(2);
Adafruit_DCMotor *lf = AFMS.getMotor(4); 
/*
Adafruit_DCMotor *rb = AFMS.getMotor(2); 
Adafruit_DCMotor *lb = AFMS.getMotor(4); 
*/


// constants to define colors
#define n  3
#define BLACK 0
#define GRAY 1 
#define WHITE 2


// constants representing directions
#define FF 0
#define LL -1
#define RR 1
#define NONE 2


// sensor pins
int s[n] = {0, 1, 2};
// threshold levels
int marker_white = 200; 
int white_black = 800; 

int leftSpeed = 60;
int rightSpeed = 70; 

// pins for talking to the RFDuino
int fwdPin = 2; 
int leftPin = 3; 
int rightPin = 4; 
// use this pin to request the next direction from the RFDuino
int nextPin = 5; 


// some booleans
boolean intersecting;
boolean prevIntersecting = false; 



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  AFMS.begin(); 

  lf->setSpeed(leftSpeed); 
//  rb->setSpeed(motorSpeed); 
//	lb->setSpeed(motorSpeed); 
	rf->setSpeed(rightSpeed); 

  pinMode(fwdPin, INPUT); 
  pinMode(leftPin, INPUT); 
  pinMode(rightPin, INPUT); 
  pinMode(nextPin, OUTPUT); 

  digitalWrite(nextPin, HIGH); 

  

 
}
#define NUM_DIRS  9
// rules about what to do given certain colors
int R[NUM_DIRS][4]={
  {WHITE, BLACK, WHITE, FF}, 
  //{BLACK, BLACK, WHITE, LL}, 
  //{WHITE, BLACK, BLACK, RR}, 
  {WHITE, WHITE, BLACK, RR}, 
  {BLACK, WHITE, WHITE, LL}/* 
  {GRAY,  WHITE, BLACK, RR}, 
  {BLACK, WHITE,  GRAY, LL}, 
  {WHITE, WHITE,  GRAY, LL}, 
  {GRAY,  WHITE, WHITE, RR}
  */
};

int next_command = NONE; 

void loop() {


  // listen for commands sets the next_command
  listenForCommands(); 
  int l[n]; 
  int command = NONE; 
  for (int i = 0; i < n; i++) {
    // get color readings from the sensor
    l[i] = readLine(s[i]); 
  } 
  
  if (millis() % 80 == 0) {
    for (int i = 0; i < n; i++) {
      Serial.print(analogRead(s[i])); 
      Serial.print(" "); 
    }
      
    Serial.println(); 
  }  
  
  
  int dir = NONE; // dir is the direction determined by the rules
  for (int i = 0; i < NUM_DIRS; i++) {
    // iterate through the rules and find the one that matches
    boolean match = true; 
    for (int j = 0; j < 3; j++) {
      if (R[i][j] != l[j]) {
        match = false; 
        break;
      }
    }
    if (match) {
      dir = R[i][3]; 
      break;
    }
  } 
  
  prevIntersecting = intersecting; 
  intersecting = atIntersection(l); 
  
  if (intersecting) {
    // if we're intersecting, 
    // go in the direction of the next command from the rfduino
    if (next_command == NONE) {
      r_run(RELEASE); 
      l_run(RELEASE); 
      // do nothing; wait
    } else {
      runMotors(next_command); 
    }
    
  } else {
    runMotors(dir);   
    if (prevIntersecting) {
      // if we come out of an intersection, request the next command for the next intersection
      digitalWrite(nextPin, HIGH); 
    }
  }
  

  

}


void listenForCommands() {
  Serial.print(digitalRead(leftPin));
  Serial.print(digitalRead(rightPin));
  Serial.print(digitalRead(fwdPin));
  Serial.println(); 
  delay(1000); 
 
  int command = NONE; 
  if (digitalRead(nextPin) == HIGH) {
    // if we are requesting the next command; this might be a problem spot, if this
    // doesn't accurate reflect if we're actually requesting the next command
    if (digitalRead(fwdPin) == HIGH) {
      command = FF; 
      
    Serial.print("got command "); 
    } else if (digitalRead(leftPin) == HIGH) {
      command = LL; 
      
    Serial.print("got command "); 
    } else if (digitalRead(rightPin) == HIGH) {
      command = RR; 
      
    Serial.print("got command "); 
    }
  }
  if (command != NONE) {
    // if we got a command, turn off the request for the next command
    digitalWrite(nextPin, LOW); 
    next_command = command; 
  }
}

int wait_time = 100; 

// are we at an intersection? 
boolean atIntersection(int* line) {
  if (line[1] == GRAY) {
    // if we think we might be at an intersection
    // test that we're actually at an intersection for
    // a certain period of time. Only then, return true. 
 
    boolean failed = false; 
    long start_time = millis(); 
    while (millis() - start_time < wait_time) {
      if (readLine(s[1]) != GRAY) {
        failed = true; 
        break;
      }
    }
    return !failed; 
  } else {
    return false; 
  }
}

void runMotors(int dir) {
  //Serial.println(dir); 

 
  if (dir == FF) {
    r_run(FORWARD); 
    l_run(FORWARD);  
    
  } 
  if (dir == LL) {
    r_run(FORWARD); 
    l_run(BACKWARD); 
    //lf->run(RELEASE); 
    //rb->run(RELEASE); 
  }
  if (dir == RR) {
    r_run(BACKWARD); 
    l_run(FORWARD); 
    //rf->run(RELEASE); 
    //lb->run(RELEASE);
  }
  if (dir == NONE) {
    
  }
  
}


int readLine(int pin) {
  int val = analogRead(pin); 
  if (val < marker_white) {
    return GRAY; 
  } else if (val < white_black) {
    return WHITE; 
  } else {
    return BLACK; 
  } 
}

boolean r_run(uint8_t dir) {
  rf->run(dir); 
  //rb->run(dir); 
}


boolean l_run(uint8_t dir) {
  lf->run(dir); 
  //lb->run(dir); 
}
