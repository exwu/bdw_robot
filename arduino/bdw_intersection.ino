#include <RFduinoBLE.h>
// for the rfduino

#define queue_size 100
int queue_start = 0; 
int queue_end = 0;
int queue_occupancy = 0; 
int queue[queue_size];

char fwd = '1'; 
char left = '2';
char right = '3'; 

int fwdPin = 2; 
int leftPin = 3; 
int rightPin = 4; 

int nextPin = 5;

#define FF 0
#define LL -1
#define RR 1
#define NONE 2


void setup() {
  pinMode(ledPin, OUTPUT);
  RFduinoBLE.advertisementData = "CouDow"; // shouldnt be more than 10 characters long
  RFduinoBLE.deviceName = "BDWBot";  //  name of your RFduino. Will appear when other BLE enabled devices search for it
  RFduinoBLE.begin(); // begin
}

boolean nextHighLow = true; 
 
void loop() {
  
  int next = NONE; 

  if (digitalRead(nextPin) == HIGH) {
    // arduino sent us back that it used the last command it gave to us; 
    // send it another one
    // only do this if the arduino has gone high once and low once (make sure it's a different request)
    if (nextHighLow) {
      if (queue_occupancy > 0) {
        next = queue[queue_start]; 
        queue_start = (queue_start + 1) % queue_size; 
        queue_occupancy--;   
      }
      // remember whether we've gotten a low since getting a high
      nextHighLow = false;  
    }
  } else {
    nextHighLow = true; 
  }
  // reset all pins
  digitalWrite(fwdPin, LOW); 
  digitalWrite(leftPin, LOW); 
  digitalWrite(rightPin, LOW); 

  // set the appropriate control pin
  if (next == FF) {
    digitalWrite(fwdPin, HIGH); 
  } else if (next == LL) {
    digitalWrite(leftPin, HIGH); 
  } else if (next == RR) {
    digitalWrite(rightPin, HIGH); 
  }
}

void RFduinoBLE_onReceive(char *data, int len) {
  // on receiving a command, stash it in the queue to be used later. 
  Serial.println(data); 
  int dir = NONE;
  if (data[0] == fwd){
    dir = FF; 
  }
  else if (data[0] == left) {
    dir = LL;
  } else if (data[0] == right) {
    dir = RR; 
  }

  if (dir != NONE) {
    queue[queue_end] = dir; 
    queue_end = (queue_end + 1);
    queue_occupancy++; 
  }
    
    
}
