#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL375.h>

#define LED A2
#define ADXL375_MG2G_MULTIPLIER (0.049) // 49mg per lsb

/* Assign a unique ID to this sensor at the same time */
/* Uncomment following line for default Wire bus      */
Adafruit_ADXL375 accel = Adafruit_ADXL375(12345);

uint8_t led_state = 0; 

/* variables for computation */
double mag = 0; 
float x = 0; 
float y = 0; 
float z = 0; 

void setup(void)
{
  Serial.begin(115200);

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

  pinMode(LED, OUTPUT); 
}

void loop(void)
{  
  /* Get a new sensor event */
  /*
  accel.getEvent(&event);
  
  x = event.acceleration.x / 9.81; 
  y = event.acceleration.y / 9.81; 
  z = event.acceleration.z / 9.81; 
  */ 

  x = (int16_t)accel.getX(); 
  y = (int16_t)accel.getY(); 
  z = (int16_t)accel.getZ(); 

  x = x * ADXL375_MG2G_MULTIPLIER; 
  y = y * ADXL375_MG2G_MULTIPLIER; 
  z = z * ADXL375_MG2G_MULTIPLIER; 

  // calculate mag of current collision
  mag = sqrt(x*x + y*y + z*z);

  /*
  Serial.print(x);
  Serial.print(", ");  

  Serial.print(y);
  Serial.print(", ");  

  Serial.print(z);
  Serial.println();  
  */ 

  // Serial.println(mag);

  if(led_state == 0) {
    digitalWrite(LED, HIGH); 
    led_state = 1; 
  }

  else {
    digitalWrite(LED, LOW); 
    led_state = 0; 
  }
}
