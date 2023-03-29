#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL375.h>

#define ADXL375_SCK 13
#define ADXL375_MISO 12
#define ADXL375_MOSI 11
#define ADXL375_CS 10

/* Assign a unique ID to this sensor at the same time */
/* Uncomment following line for default Wire bus      */
Adafruit_ADXL375 accel = Adafruit_ADXL375(12345);

/* Uncomment for software SPI */
//Adafruit_ADXL375 accel = Adafruit_ADXL375(ADXL375_SCK, ADXL375_MISO, ADXL375_MOSI, ADXL375_CS, 12345);

/* Uncomment for hardware SPI */
//Adafruit_ADXL375 accel = Adafruit_ADXL375(ADXL375_CS, &SPI, 12345);

sensors_event_t event;
int currentTime   = 0; 

/* variables for computation */
double magnitude = 0; 
float x = 0; 
float y = 0; 
float z = 0; 

void setup(void)
{
  Serial.begin(115200);
  Serial.println("ADXL375 Accelerometer Test"); Serial.println("");

  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL375 ... check your connections */
    while(1) Serial.println("Ooops, no ADXL375 detected ... Check your wiring!");
  }

  // Range is fixed at +-200g
  
  accel.setDataRate(ADXL343_DATARATE_1600_HZ); 

  /* Display some basic information on this sensor */
  //accel.printSensorDetails();
  //displayDataRate();

  pinMode(LED_BUILTIN, OUTPUT); 
  digitalWrite(LED_BUILTIN, LOW); 
}

void loop(void)
{  
  /* Get a new sensor event */
  accel.getEvent(&event);
  
  x = event.acceleration.x / 9.81; 
  y = event.acceleration.y / 9.81; 
  z = event.acceleration.z / 9.81; 

  // calculate magnitude of current collision
  magnitude = sqrt(x*x + y*y + z*z);

  /*
  Serial.print(event.acceleration.x);
  Serial.print(", ");  

  Serial.print(event.acceleration.y);
  Serial.print(", ");  

  Serial.print(event.acceleration.z);
  Serial.println();  
  */

  Serial.println(magnitude);
}
