# SDP-Team-31
SDP23 Project for Team 31

We present SMH, a smart biking helmet and associated sensing system which can sense riders’ vital signs such as heart rate, ambient temperature and humidity, and make it accessible to them while they bike. In the event of an accident, the system would send a remote alert to a registered contact, including the rider’s location. The helmet uses a microcontroller for sampling a MEMS accelerometer capable of measuring upto 200 Gs of acceleration at 4 KHz for collision detection. In the event of a dangerous collision, the system communicates with an Android app over Bluetooth Low Energy, which then sends a text message to an emergency contact. The bike board samples temperature, humidity, heart rate, altitude and UV-index and displays them to the biker during their bike ride. Both devices are lightweight, inexpensive, and have long battery lives. 

Members: 
Marloon Paula Gonzalez - EE 2023; 
Max Morel - EE 2023; 
Ruchit Patel - EE 2023; 
Akshat Sahay - CompE 2023; 

This repository contains the hardware and software for the helmet board which performs concussion detection and generates BLE alerts for the associated smartphone app in case of a dangerous collision. Software is written using the RP2040 core in Arduino, with the Adafruit BLE Friend library to interface with the BLE module. The hardware is designed in KiCad 7. 
