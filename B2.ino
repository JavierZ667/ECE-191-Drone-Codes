//==============( B2) ======================
#include <SPI.h>  
#include "RF24.h"

RF24 radio (7, 8);
byte addresses[][6] = {"1"};

#define syncStateA 10
#define syncStateB 11
#define debugPinC 12
#define echoPin 4 // Echo Pin
#define trigPin 5 // Trigger Pin
#define rfRxPin 6 // rf Receive input pin
#define minBitPeriod 450  // Low threshold for bit period (uS)
#define maxBitPeriod 550  // Max threshold for bit period (uS)
#define minSyncBits 8     // Min number of valid 1 plus 0 transitions before 1 plus 1



int maximumRange = 500; // Maximum range needed (cm)
int minimumRange = 0; // Minimum range needed
float duration, distance; // Duration used to calculate distance
long timeout = 50000;  // Ping duration timeout




void setup() {
  Serial.begin (9600);
  pinMode(syncStateA, OUTPUT) ;
  pinMode(syncStateB, OUTPUT) ;
  pinMode(debugPinC, OUTPUT) ;
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(rfRxPin, INPUT);
  digitalWrite(trigPin, LOW) ;
  radio.begin();  
  radio.setChannel(115); 
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate( RF24_250KBPS ) ; 
  radio.openWritingPipe( addresses[0]);
  delay(100);
}

void loop() {
  RxRFSync() ;   
  digitalWrite(trigPin, HIGH);    // Start ping sequence
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, timeout);

  //Calculate the distance (in cm) based on the speed of sound.
  distance = duration / 29.0483 ;

  if (distance >= maximumRange || distance <= minimumRange) {
    Serial.println(" ");
  }
  else {
    Serial.print("B2 distance: ");
    Serial.println(distance);
    struct package
{
  int beacon=2;
  float distance2=distance;

};


typedef struct package Package;
Package data;

    radio.write(&data, sizeof(data)); 
   delay(100);
  }
}

void RxRFSync()
{
  long int lastBitTime = 0 ;    // holds time of last bit transition (uS)
  int BitCount = 0 ;            // counts number of valid bits detected

  boolean synced = false ;
  boolean lastBit = LOW ;  // holds last bit detected

  digitalWrite(syncStateA, LOW) ; digitalWrite(syncStateB, LOW) ; digitalWrite(debugPinC, LOW) ;

  while (!synced)
  {
    while (digitalRead(rfRxPin) == lastBit) { } // Block until bit transition
    int currBitTime = micros() ;
    int bitPeriod = currBitTime - lastBitTime ;

    if ((bitPeriod > (2 * minBitPeriod)) && (bitPeriod < (2 * maxBitPeriod)) && (lastBit == HIGH) && (BitCount > minSyncBits))
    {
      // Valid completion of sync sequence
      synced = true ;
      digitalWrite(syncStateA, HIGH) ; digitalWrite(syncStateB, HIGH) ; digitalWrite(debugPinC, lastBit) ;
    }
    else
    {
      lastBit = !lastBit ;
      lastBitTime = currBitTime ;
      if ((bitPeriod > minBitPeriod) && (bitPeriod < maxBitPeriod))
      {
        // Valid single bit detected, increment valid bit count and look for next bit
        BitCount += 1 ;     // increment valid bit count
        digitalWrite(syncStateA, LOW) ; digitalWrite(syncStateB, HIGH) ; digitalWrite(debugPinC, lastBit) ;
      }
      else
      {
        // Invalid bit detected, reset valid bit count and look for next bit
        BitCount = 0 ;
        digitalWrite(syncStateA, HIGH) ; digitalWrite(syncStateB, LOW) ; digitalWrite(debugPinC, lastBit) ;
      }
    }
  }
}
