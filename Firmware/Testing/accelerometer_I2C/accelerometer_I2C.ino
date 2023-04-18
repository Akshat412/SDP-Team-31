/*
 * SDP23 Team 31
 * ADXL375 I2C Test
 * Forked from Dejan: https://www.youtube.com/watch?v=KMhbV1p3MWk
 */

#include <Wire.h> 

// hardware defines 
#define ADXL375 0x53
#define LED A2        

// constant defines
#define ADXL375_MG2G_MULTIPLIER (0.049) // 49mg per lsb

// variables
float x, y, z;      
double magn = 0; 
int led_state = 0; 

void setup() {
  Serial.begin(115200); 
  
  // set SCL to 400KHz and begin I2C
  Wire.setClock(400000); 
  Wire.begin();

  // communicate to set up ADXL375
  Wire.beginTransmission(ADXL375); 

  Wire.write(0x2D); // write to POWER_CTL register 
  Wire.write(0x08); // set bit D3 high for measurement 

  Wire.write(0x2C); // write to BW_RATE register  
  Wire.write(0x0D); // set bits D3:D0 to 1101 for 800 Hz

  Wire.write(0x31); // write to DATA_FORMAT register
  Wire.write(0x0B); // set bits D4:D0 to 01011 for format
  Wire.endTransmission();

  // minor delay for ensuring setup 
  delay(10);

  pinMode(LED, OUTPUT); 
}

void loop() {
  // communicate to get data from ADXL375
  Wire.beginTransmission(ADXL375);
  Wire.write(0x32); 
  Wire.endTransmission(false);

  // read 6 byte over I2C, convert to normal values
  Wire.requestFrom(ADXL375, 6, true); 
  x = (int16_t)( Wire.read()| Wire.read() << 8); 
  y = (int16_t)( Wire.read()| Wire.read() << 8); 
  z = (int16_t)( Wire.read()| Wire.read() << 8); 

  x = x * ADXL375_MG2G_MULTIPLIER; 
  y = y * ADXL375_MG2G_MULTIPLIER; 
  z = z * ADXL375_MG2G_MULTIPLIER; 

  mag = sqrt(x*x + y*y + z*z); 
  // Serial.println(mag); 
  
  /*
  Serial.print(x);
  Serial.print(", ");  
  Serial.print(y);
  Serial.print(", ");  
  Serial.print(z);
  Serial.println();  
  */
  
  if(led_state == 0) {
    digitalWrite(LED, HIGH); 
    led_state = 1; 
  }

  else {
    digitalWrite(LED, LOW); 
    led_state = 0; 
  }
}
