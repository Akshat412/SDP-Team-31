//  SerialIn_SerialOut_HM-10_01
//
//  Uses hardware serial to talk to the host computer and AltSoftSerial for communication with the bluetooth module
//
//  What ever is entered in the serial monitor is sent to the connected device
//  Anything received from the connected device is copied to the serial monitor
//  Does not send line endings to the HM-10
//
//  Pins
//  BT VCC to Arduino 5V out. 
//  BT GND to GND
//  Arduino D8 (SS RX) - BT TX no need voltage divider 
//  Arduino D9 (SS TX) - BT RX through a voltage divider (5v to 3.3v)
 
#include <SoftwareSerial.h>
SoftwareSerial BTserial(2, 3); 

char c=' ';
boolean NL = true;

boolean lastChar = false;
 
void setup() 
{
    Serial.begin(9600);
    Serial.println(" ");
 
    BTserial.begin(9600);  
    Serial.println("BTserial started at 9600");
}
 
void loop()
{
    // Read from the Bluetooth module and send to the Arduino Serial Monitor
    if (BTserial.available()) {
        c = BTserial.read();
        Serial.print(c);

        lastChar = true; 
    }
 
 
    // Read from the Serial Monitor and send to the Bluetooth module
    if (Serial.available()) {
        c = Serial.read();
 
        // do not send line end characters to the HM-10
        if (c!=10 & c!=13 ) {  
             BTserial.write(c);
        }
    }
}
