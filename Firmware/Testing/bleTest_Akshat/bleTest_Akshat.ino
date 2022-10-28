/*
 * Test for HC-05/Adafruit BLE Friend 
 * Author: Akshat Sahay
 */

#include <SoftwareSerial.h> 
SoftwareSerial MyBlue(2, 3); // RX | TX 
int flag = 415; 

void setup() {   
  Serial.begin(9600); // initialize serial
  MyBlue.begin(9600); // initialize software serial for BLE
  Serial.println("Ready to connect\nDefualt password is 1234 or 000"); 
} 
void loop() { 
  MyBlue.println(flag); 
  delay(1000); 
}  
