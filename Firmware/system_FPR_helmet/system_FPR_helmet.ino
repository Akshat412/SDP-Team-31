/*
 * SDP23 Team 31
 * FPR System Software - Helmet 
 * --------------------
 * Samples accelerometer
 * If collision, goes into alert state
 * Remote alert over BLE to Android app 
 * Device mode:   switch between FSR and collisions  
 * System checks: LED turns on if peripherals connected 
 * System alerts: BUZZER if peripheral disconnected during operation 
 */

/* include statements */
#include <Wire.h>
#include <SPI.h>

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

/* hardware and const defines */
#define ADXL375 0x53
#define FSR     A3 
#define BUZZER  A2        
#define BATT    A1
#define LED     11

#define ADXL375_MG2G_MULTIPLIER (0.049) // 49mg per lsb
#define ADXL375_DEV_ID 0xE5             // accelerometer device ID 
#define FACTORYRESET_ENABLE 1           // reference in Adafruit test code 

// collision locations 
#define FRONT_RIGHT 1
#define FRONT_LEFT  2
#define BACK_RIGHT  3
#define BACK_LEFT   4

// buzzer frequencies
#define BUZZER_FSR  2000 
#define BUZZER_CD   1000
#define BUZZER_ERR  500 

/* BLE Friend over hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST); 

/* variables for computation */
uint8_t acc_addr = 0; // ADXL375 address

double mag = 0; // magnitude of collision

// coordinates for accelerometer 
float x = 0; 
float y = 0; 
float z = 0; 

// collision location 
double theta_impact = 0; 
uint8_t direction = 0; 

// device mode 
uint8_t dev_mode = 1; 

// FSR output 
uint16_t helm_tight = 0;

// LED state for sampling rate 
uint8_t led_state = 0; 

void setup() {
  // initialize serial at 115200
  Serial.begin(115200);

  // set SCL to 400KHz and begin I2C
  Wire.setClock(400000); 
  Wire.begin();

  // communicate to get ADXL375 address 
  Wire.beginTransmission(ADXL375);
  Wire.write(0x00); 
  Wire.endTransmission(false);

  Wire.requestFrom(ADXL375, 1, true);
  acc_addr = Wire.read();  

  // check ADXL375 return address
  if(acc_addr != ADXL375_DEV_ID) {
    while(1) {
      // print to console for debugging 
      Serial.println("ERROR: Failure communicating with ADXL375"); 

      tone(BUZZER, BUZZER_ERR);
      delay(250);
      noTone(BUZZER);
      delay(250);
    }
  }

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

  // initialize BLE Friend 
  Serial.print(F("Initialising the Bluefruit LE module: "));
  if ( !ble.begin(VERBOSE_MODE) ) {
    while(1) {
      // print to console for debugging 
      Serial.println("ERROR: Failure communicating with BLE module"); 

      tone(BUZZER, BUZZER_ERR);
      delay(250);
      noTone(BUZZER);
      delay(250);
    } 
  }

  // factory reset to remove old characteristics 
  if ( FACTORYRESET_ENABLE ) {
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      while(1) {
        // print to console for debugging 
        Serial.println("ERROR: Failure resetting BLE module"); 

        tone(BUZZER, BUZZER_ERR);
        delay(250);
        noTone(BUZZER);
        delay(250);
      } 
    }
  }

  // disable echo from console 
  ble.echo(false);

  // define new service with UUID 0x180E, PHONE ALERT STATUS
  ble.println("AT+GATTADDSERVICE=UUID=0x180E");
  ble.waitForOK();  

  // define new characteristic with UUID 0x2A46, NEW ALERT
  ble.println("AT+GATTADDCHAR=UUID=0x2A46,PROPERTIES=0x02,MIN_LEN=1,VALUE=75");
  ble.waitForOK(); 

  // reset module to implement new service 
  ble.println("ATZ");
  ble.waitForOK(); 

  // GPIO enables 
  pinMode(LED, OUTPUT); 
  pinMode(BUZZER, OUTPUT); 
  pinMode(FSR, INPUT); 
  pinMode(BATT, INPUT); 

  digitalWrite(LED, HIGH); 
}

void loop() {
  if(dev_mode == 0) {
    helm_tight = analogRead(FSR); 

    if(helm_tight > 128) {
      // beep buzzer 
      tone(BUZZER, BUZZER_FSR);
      delay(250);
      noTone(BUZZER);
      delay(250); 

      tone(BUZZER, BUZZER_FSR);
      delay(250);
      noTone(BUZZER);
      delay(250); 

      tone(BUZZER, BUZZER_FSR);
      delay(250);
      noTone(BUZZER);
      delay(250); 

      // set device mode to 1
      dev_mode = 1; 
    }
  }

  else {
    // communicate to get data from ADXL375
    Wire.beginTransmission(ADXL375);
    Wire.write(0x32); 
    Wire.endTransmission(false);

    // read 6 byte over I2C, convert to normal values
    Wire.requestFrom(ADXL375, 6, true); 
    x = (int16_t)( Wire.read()| Wire.read() << 8); 
    y = (int16_t)( Wire.read()| Wire.read() << 8); 
    z = (int16_t)( Wire.read()| Wire.read() << 8); 

    // convert values to Gs 
    x = x * ADXL375_MG2G_MULTIPLIER; 
    y = y * ADXL375_MG2G_MULTIPLIER; 
    z = z * ADXL375_MG2G_MULTIPLIER; 

    // calculate magnitude 
    mag = sqrt(x*x + y*y + z*z); 

    // check magnitude for collision, 85 Gs
    if(mag > 30) {
      // collision, enter alert state
      Serial.println(mag); 

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
      
      // change value of characteristic 
      ble.println("AT+GATTCHAR=1,65");
      ble.waitForOK(); 
      
      // buzzer at 1000 Hz
      while(1) {
        tone(BUZZER, 1000);
        delay(250);
        noTone(BUZZER);
        delay(250);   
      }
    }
  } 

  // system check to ensure everything is good 
  if(millis() % 600000 == 0) {
    // battery level check 
    if(analogRead(BATT) < 48) {
      // turn off system LED
      digitalWrite(LED, LOW); 

      // beep 
      tone(BUZZER, BUZZER_ERR);
      delay(250);
      noTone(BUZZER);
      delay(250); 

      tone(BUZZER, BUZZER_ERR);
      delay(250);
      noTone(BUZZER);
      delay(250); 

      tone(BUZZER, BUZZER_ERR);
      delay(250);
      noTone(BUZZER);
      delay(250); 
    }

    // communicate to get ADXL375 address 
    Wire.beginTransmission(ADXL375);
    Wire.write(0x00); 
    Wire.endTransmission(false);

    Wire.requestFrom(ADXL375, 1, true);
    acc_addr = Wire.read();  

    // check ADXL375 return address
    if(acc_addr != ADXL375_DEV_ID) {
      digitalWrite(LED, LOW); 

      while(1) {
        // print to console for debugging 
        Serial.println("ERROR: Failure communicating with ADXL375"); 

        tone(BUZZER, BUZZER_ERR);
        delay(250);
        noTone(BUZZER);
        delay(250);
      }
    }
  }

  /*
  if(led_state == 0) {
    digitalWrite(LED, HIGH); 
    led_state = 1; 
  }

  else {
    digitalWrite(LED, LOW); 
    led_state = 0; 
  }
  */
}
