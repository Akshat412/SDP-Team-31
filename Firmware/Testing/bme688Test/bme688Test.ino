/*
 * BME688 Test
 */

/* include statements */
#include <Wire.h>
#include <SPI.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL375.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "Adafruit_BME680.h"

#include "BluefruitConfig.h"

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)
#define DELAY_TIME 10000

/* user defines */
#define BUZZER              9
#define FACTORYRESET_ENABLE 1  // reference in Adafruit test code 

/* BLE Friend over hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* Initialize BME688 */
Adafruit_BME680 bme; // I2C

/* Initialize display */
Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

/* Helper function for debugging BLE Friend */
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

void setup() {
  // initialize serial at 115200
  Serial.begin(115200);
  Serial.println("BME688 Test"); 
  Serial.println("-----------------");

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }

  // initialize BLE Friend 
  Serial.print(F("Initialising the Bluefruit LE module: "));
  if ( !ble.begin(VERBOSE_MODE) ) error(F("Couldn't find Bluefruit!"));

  // factory reset for BLE Friend to remove old characteristics 
  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  delay(250); // wait for the OLED to power up
  display.begin(0x3C, true); // Address 0x3C default

  Serial.println("OLED begun");

  // Clear the buffer.
  display.clearDisplay();
  
  display.setRotation(3);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);

  display.println("SDP Team 31 MDR ");
  display.println("Feature Display"); 
  display.display();

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
  digitalWrite(LED_BUILTIN, LOW); 
}

void loop() {
  if(millis() % DELAY_TIME == 0) {    
    display.clearDisplay();
    bme.performReading(); 

    display.setCursor(0,0);
    display.print("Temp: ");
    display.print(bme.temperature);
    display.println(" *C");
    
    display.print("Hum: ");
    display.print(bme.humidity);
    display.println(" %");
    
    display.print("Alt: ");
    display.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    display.println(" m");

    display.display();
  }
}
