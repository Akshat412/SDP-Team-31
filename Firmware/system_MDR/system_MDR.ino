/*
 * SDP23 Team 31
 * MDR System Software 
 * --------------------
 * Samples accelerometer
 * If collision, goes into alert state
 */

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL375.h>

#define BUZZER 9

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL375 accel = Adafruit_ADXL375(12345);

sensors_event_t event;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("ADXL375 Accelerometer Test"); Serial.println("");

  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL375 ... check your connections */
    Serial.println("Ooops, no ADXL375 detected ... Check your wiring!");
    while(1);
  }

  // Range is fixed at +-200g
  
  accel.setDataRate(ADXL343_DATARATE_800_HZ); 

  /* Display some basic information on this sensor */
  //accel.printSensorDetails();
  //displayDataRate();

  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(BUZZER, OUTPUT); 
  digitalWrite(LED_BUILTIN, LOW); 
}

void loop() {
  /* Get a new sensor event */
  accel.getEvent(&event);

  //Serial.print(event.acceleration.x/9.81); Serial.print(", ");
  //Serial.print(event.acceleration.y/9.81); Serial.print(", ");
  //Serial.print(event.acceleration.z/9.81); Serial.print(", "); 

  double magnitude = sqrt(event.acceleration.x*event.acceleration.x + 
                          event.acceleration.y*event.acceleration.y + 
                          event.acceleration.z*event.acceleration.z); 
  
  //Serial.println(magnitude/9.81); 

  if(magnitude > 90) {
    // collision 
    digitalWrite(LED_BUILTIN, HIGH); 
    while(1) {
      tone(BUZZER, 1000);
      delay(250);
      noTone(BUZZER);
      delay(250);   
    }
  }
}
