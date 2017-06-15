

//===============( UAV)===========
#include "RF24.h"
#include "MatrixMath.h"

RF24 radio (7, 8);



struct package
{
  int beacon = 0;
  float distance = 0.0;

};

byte addresses[][6] = {"0", "1", "2", "3"};

typedef struct package Package;
Package data;


#define echoPin 4 // Echo Pin
#define trigPin 5 // Trigger Pin
#define rfTxPin 6 // rf Transmit enable

#define BitPeriod 500   // Bit period for RF sync bits in microseconds

boolean syncSeq[16] = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1} ; // Bit sequence for synchronization

int maximumRange = 500; // Maximum range needed (cm)
int minimumRange = 0; // Minimum range needed
float duration, distance; // Duration used to calculate distance
long timeout = 50000;  // Ping duration timeout (uS)


float bx[4] = {0.0, 30, 30, 0.0};
float by[4] = {0.0, 0.0, 30, 30};
float bz[4] = {0.0, 0.0, 0.0, 1};
float X[3][1];
float A[3][3];

void setup() {
  Serial.begin (4800);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(rfTxPin, OUTPUT);
  digitalWrite(trigPin, LOW) ;
  digitalWrite(rfTxPin, LOW) ;
  radio.begin();
  radio.setChannel(115);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate( RF24_250KBPS ) ;
  radio.openReadingPipe(1, addresses[0]);
  radio.openReadingPipe(2, addresses[1]);
  radio.openReadingPipe(3, addresses[2]);
  radio.openReadingPipe(4, addresses[3]);
  radio.startListening();
  delay(100);
}

void loop() {
  TxRFSync() ;        // Send RF synchronization sequence
  digitalWrite(trigPin, HIGH);      // Start ping sequence
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, timeout);

  float d[4] = {0.00, 0.00, 0.00, 0.00};

  if ( radio.available())
  {
    while (radio.available() && ( d[0] == 0 || d[1] == 0 || d[2] == 0 || d[3] == 0))
    {
      radio.read( &data, sizeof(data) );
    }
    if (data.beacon == 1)
    {
      d[0] = data.distance;
    }
    if (data.beacon == 2)
    {
      d[1] = data.distance;
    }
    if (data.beacon == 3)
    {
      d[2] = data.distance;
    }
    if (data.beacon == 4)
    {
      d[3] = data.distance;
    }

    if (d[0] > 0 || d[1] > 0 || d[2] > 0 || d[3] > 0)
    {
      for (int i = 0; i <= 2; i++ )
      {
        X[i][0] =  d[i] * d[i] - bx[i] * bx[i] - by[i] * by[i] - bz[i] * bz[i] - d[3] * d[3] + bx[3] * bx[3] + by[3] * by[3] + bz[3] * bz[3];
      }
      for (int i = 0 ; i <= 2; i++) {
        for (int j = 0; j <= 2; j++ ) {
          if (j == 0)
            A[i][j] = -2 * (bx[i] - bx[3]);
          else if ( j == 1)
            A[i][j] = -2 * (by[i] - by[3]);
          else
            A[i][j] = -2 * (bz[i] - bz[3]);
        }
      }

      float position[3][1];
      Matrix.Invert( (float *) A, 3);
      Matrix.Multiply((float*) A, (float *)X, 3, 3, 1 , (float *)position ); // matrix mult/ inv, gonna need to find libraries.
      if (position[2][0] > 0)
      {
        // Matrix.Print((float *) position, 3, 1, "position");

        Serial.print( "( ");
        Serial.print(position[0][0]);
        Serial.print(" , ");
        Serial.print(position[1][0]);
        Serial.print(" , ");
        Serial.print(position[2][0]);
        Serial.println(" ) ");


      }
    }




    //Serial.print("\nPackage");
    //Serial.print(data.beacon);
    //Serial.print(" Distance");
    //Serial.print(data.beacon);
    //Serial.print(": ");
    //Serial.println(data.distance);
  }
  delay(100);
}


void TxRFSync()
{
  for (int k = 0; k < sizeof(syncSeq) ; k++)
  {
    digitalWrite(rfTxPin, syncSeq[k]) ;
    delayMicroseconds(BitPeriod) ;
  }
  digitalWrite(rfTxPin, LOW) ;      // Turn off transmit at end of sequence
}
