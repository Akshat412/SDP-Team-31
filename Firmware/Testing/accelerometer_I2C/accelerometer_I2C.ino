/*
 * SDP23 Team 31
 * ADXL375 I2C Test
 * Forked from Dejan: https://www.youtube.com/watch?v=KMhbV1p3MWk
 */

#include <Wire.h> 

// hardware defines 
#define ADXL375 0x53
#define LED 11        

// constant defines
#define ADXL375_MG2G_MULTIPLIER (0.049) // 49mg per lsb

// collision locations 
#define FRONT_RIGHT 1
#define FRONT_LEFT  2
#define BACK_RIGHT  3
#define BACK_LEFT   4

// variables
float x, y, z;      
double mag = 0; 
uint8_t led_state = 0; 

// collision location 
double theta_impact = 0; 
uint8_t direction = 0; 

void setup() {
  Serial.begin(115200); 
  
  // set SCL to 400KHz and begin I2C
  Wire.setClock(400000); 
  Wire.begin();

  // communicate to set up ADXL375
  Wire.beginTransmission(ADXL375);
  Wire.write(0x2C); // write to BW_RATE register  
  Wire.write(0x0F); // set bits D3:D0 to 1101 for 800 Hz
  Wire.endTransmission();

  Wire.beginTransmission(ADXL375);
  Wire.write(0x2D); // write to POWER_CTL register 
  Wire.write(0x08); // set bit D3 high for measurement 
  Wire.endTransmission();

  Wire.beginTransmission(ADXL375);
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
  if(mag > 40) {
    // calculate impact location and store in characteristic 
    if(x > 0) {
      // only front right or back right 
      theta_impact = atan(y/x) * 180/3.14;

      if(theta_impact >= 0 && theta_impact <= 90) direction = FRONT_RIGHT; 
      else direction = BACK_RIGHT;
    }

    else {
      // only front left or back left 
      theta_impact = atan(y/x) * 180/3.14 + 180;

      if(theta_impact >= 90 && theta_impact <= 180) direction = FRONT_LEFT;
      else direction = BACK_LEFT;  
    }

    Serial.println(mag);
    Serial.println(direction);  
  }
  
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
