/*
 * SDP23 Team 31
 * MDR System Software 
 * --------------------
 * Samples accelerometer
 * If collision, goes into alert state
 */

/* include statements */
#include <Wire.h>
#include <SPI.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL375.h>

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

/* user defines */
#define BUZZER  9
#define SQWAVE  8 

#define FACTORYRESET_ENABLE 1  // reference in Adafruit test code 

/* BLE Friend over hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST); 

/* Initialize accelerometer and event */
Adafruit_ADXL375 accel = Adafruit_ADXL375(12345);
sensors_event_t event;

/* whether sqwave on or off */
int sqwave_state = 0; 

/* Helper function for debugging BLE Friend */
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

void setup() {
  // initialize serial at 115200
  Serial.begin(115200);
  Serial.println("SDP Team 31: MDR"); 
  Serial.println("-----------------");

  // initialize the accelerometer
  if(!accel.begin())
  {
    // problem connecting to accelerometer, check connections 
    Serial.println("ERROR: No ADXL375, check wiring");
    while(1);
  }

  // set sampling rate of accelerometer to 800 Hz, diagnostic from test code 
  accel.setDataRate(ADXL343_DATARATE_800_HZ); 

  // initialize BLE Friend 
  Serial.print(F("Initialising the Bluefruit LE module: "));
  if ( !ble.begin(VERBOSE_MODE) ) {
    Serial.println("ERROR: No BLE module, check wiring");
    while (1); 
  }

  // factory reset to remove old characteristics 
  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  // disable echo from console 
  ble.echo(false);

  // define new service with UUID 0x180E
  ble.println("AT+GATTADDSERVICE=UUID=0x180E");
  ble.waitForOK();  

  // define new characteristic with UUID 0x2A46
  ble.println("AT+GATTADDCHAR=UUID=0x2A46,PROPERTIES=0x02,MIN_LEN=1,VALUE=75");
  ble.waitForOK(); 

  // reset module to implement new service 
  ble.println("ATZ");
  ble.waitForOK(); 

  // GPIO enables 
  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(BUZZER, OUTPUT); 
  pinMode(SQWAVE, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); 
}

void loop() {
  // get a new sensor event 
  accel.getEvent(&event);

  float x = event.acceleration.x / 9.81; 
  float y = event.acceleration.y / 9.81; 
  float z = event.acceleration.z / 9.81; 

  // calculate magnitude of current collision
  double magnitude = sqrt(x*x + y*y + z*z); 

  double theta_impact = 0; 
  if(event.acceleration.z > 0) theta_impact = atan(x/z) * 180/3.14 + 180;
  else theta_impact = atan(x/z) * 180/3.14; 

  double phi = acos(y / magnitude) - 90;

  // check magnitude for collision, 85 Gs
  if(magnitude > 85) {
    // collision, enter alert state
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.print(", ");
    Serial.print(z);
    Serial.print(", ");
    
    Serial.print(magnitude); 
    Serial.print(", ");
    Serial.print(theta_impact);
    Serial.print(", ");
    Serial.println(phi);
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
  
  else if(magnitude > 20) {
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.print(", ");
    Serial.print(z);
    Serial.print(", ");
    
    Serial.print(magnitude); 
    Serial.print(", ");
    Serial.print(theta_impact);
    Serial.print(", ");
    Serial.println(phi); 
  }

  if(sqwave_state == 0) {
    sqwave_state = 1;
    digitalWrite(SQWAVE, HIGH);  
  }

  else {
    sqwave_state = 0;
    digitalWrite(SQWAVE, LOW);  
  }
}
