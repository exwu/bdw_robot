#include <Adafruit_MotorShield.h>

#include <Wire.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *rf = AFMS.getMotor(1);
Adafruit_DCMotor *lf = AFMS.getMotor(2); 
Adafruit_DCMotor *lb = AFMS.getMotor(3); 
Adafruit_DCMotor *rb = AFMS.getMotor(4); 

#define n  3
#define queue_size 100
#define BLACK 0
#define GRAY 1 
#define WHITE 2



#define FF 0
#define LL -1
#define RR 1
#define NONE 2; 


int queue[queue_size]; 

int queue_end = 0; 
int queue_start = 0; 
int queue_occupancy = 0; 

int s[n] = {0, 1, 2};
int e[n] = {7, 8, 12}; 
int gray_white = 1000; 
int black_gray = 600;

int motorSpeed = 120;

int forwardPin = 3; 
int leftPin = 4; 
int rightPin = 5; 

int confirationPin = 6; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int i = 0; i < n; i++) {
    pinMode(e[i], OUTPUT); 
    digitalWrite(e[i], HIGH);
  }

  AFMS.begin(); 

  rf->setSpeed(motorSpeed); 
  rb->setSpeed(motorSpeed); 
	lf->setSpeed(motorSpeed); 
	lb->setSpeed(motorSpeed); 

  

 
}

void loop() {
  listenForCommands(); 
  boolean bl[n]; 
  int l[n]; 
  int command = NONE; 
  for (int i = 0; i < n; i++) {
    l[i] = readLine(i); 
    bl[i] = l[i] == 0; 
  }
  boolean intersecting = atIntersection(l); 
  if (intersecting) {
    // run the intersection routine
    if (queue_occupancy == 0) {
      // no instructions on the queue; freak out; 
      delay(5000); 
    } else {
      command = queue[queue_start]; 
      queue_start = (queue_start + 1)%queue_size; 
      queue_occupancy--; 
    }
  }

  
  if ((bl[0] && bl[1] && !bl[2]) 
    || (bl[0] && !bl[1] && !bl[2])
    && !(intersecting && command != LL)) {
    turnLeft();       
  } else if ((!bl[0] && bl[1] && bl[2])
    || (!bl[0] && !bl[1] && bl[2])
    && !(intersecting && command != RR)) {
    turnRight();  
  } else if ((!bl[0] && bl[1] && !bl[2])
    && !(intersecting && comand != FF)) {
    l_run(FORWARD); 
    r_run(FORWARD); 
  } else {
    l_run(RELEASE); 
    r_run(RELEASE); 
  }
}

void listenForCommands() {
  
}

boolean atIntersection(int[] line) {
  return false; 
}

void turnLeft() {
  r_run(FORWARD); 
  l_run(BACKWARD); 
}
void turnRight() {
  r_run(BACKWARD); 
  l_run(FORWARD); 
}

int readLine(int pin) {
  int val = analogRead(pin); 
  if (val > gray_white) {
    return WHITE; 
  } else if (val > black_gray) {
    return GRAY; 
  } else {
    return BLACK; 
  }
}

boolean r_run(uint8_t dir) {
  rf->run(dir); 
  rb->run(dir); 
}


boolean l_run(uint8_t dir) {
  lf->run(dir); 
  lb->run(dir); 
}
