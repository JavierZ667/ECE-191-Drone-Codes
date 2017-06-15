I. File list
------------
UAV_6-11-850.ino		
B1.ino
B2.ino
B3.ino
B4.ino


II. Libraries 
----------
SPI.h
RF24.h
MatrixMath.h

III. Beacons 
----------
B1, B2, B3, B4 contain code to be uploaded to each landing pad beacon.
Each of the landing pad beacon has the SPI.h and RF24.h libraries in order 
to properly use the hardware and some functions in the code
#define echoPin 4 // Echo Pin from HCSR-04 is conntected to pin 4
#define trigPin 5 // Trig Pin " "
#define rfRxPin 6 // Pin from the 433 Mhz Chip reciever 
Pin 7, and Pin 8 are from the NRF24L01 chip which correstpond to the CSN
and CE inputs from the NRF24L01. Additionally pin 11 on the arduino is 
connected to MOSI, pin 12 to MISO, and pin 13 to SCK, from the NRF24L01
The NRF24L01 acts as transmitter on this beacon

This uses the SPI.h and RF24.h libraries 

IV. UAV
----------
Connected in the same way as beacon one except with a 433 Mhz Chip acting
as the transmitter. The NRF24L01 acts as a reciever on this beacon
This uses the RF24.h and MatrixMath.h libraries
A. Beacon Distances
byte addresses[][6] = {"0", "1", "2", "3"};
this line recieves the code from the beacons, beacon 1 having byte adress "0"
beacon 2 having byte address "1" and so one. This is to distinguish which distance
corresponds to which beacon

B. Intial Position
Since we did not have time to include a way for the beacons to determine
each other's position this section is hard coded.
The lines:
float bx[4] = {0.0, 30, 30, 0.0};
float by[4] = {0.0, 0.0, 30, 30};
float bz[4] = {0.0, 0.0, 0.0, 1};

Are the X, Y, Z positions of the beacons hence from this example
B1 is (0,0,0)
B2 is (30,0,0)
B3 is (30,30,0)
B4 is (0,30,1)

One of the Z value is a non zero value since we need that offset 
so that our equations are linearly independent and we can get a solution

C. Individual distances

This part has been commented out on the code
    //Serial.print("\nPackage");
    //Serial.print(data.beacon);
    //Serial.print(" Distance");
    //Serial.print(data.beacon);
    //Serial.print(": ");
    //Serial.println(data.distance);

Uncomment it to find indivual distances being recieved for each 
individual beacon

V. Trilateration psuedo code
----------
#include <MatrixMath.h>
 
void setup() {
  
 
}
 
void loop() {
 
float d[4] ;
// d[1]...d[4] are the distances from the uav to beacon 1 through 4.
 
float bx[4] ;
float by[4] ;
float bz[4] ;
 
//  x,y,z coordinates of beacons 1 through four
 
//trilateration using matrices
float X[3][1];
float A[3][3];
 
 
for (int i = 0; i <= 2; i++ ) {
  X[i][0] =  d[i]*d[i] - bx[i]*bx[i] - by[i]*by[i]- bz[i]*bz[i]-d[3]*d[3] +bx[3]*bx[3]+by[3]*by[3] +bz[3]*bz[3]; 
  
  }
  
 
for (int i = 0 ; i <=2; i++) {
  for (int j = 0; j <=2; j++ ) {
    
    if (j == 0) 
      A[i][j] = -2*(bx[i] - bx[3]);
    else if ( j==1)
      A[i][j] = -2*(by[i] - by[3]);  
    else
      A[i][j] = -2*(bz[i] - bz[3]);
    }
  
  }
 
float P[3][1];
Matrix.Invert( (float *) A, 3);
 Matrix.Multiply((float*) A,(float *)X,3,3,1 , (float *)P);   
 
// coordinates of uav
 
 
//Serial.println( P[0][0] ", " P[1][0] ", " P[2][0]);
 
}
