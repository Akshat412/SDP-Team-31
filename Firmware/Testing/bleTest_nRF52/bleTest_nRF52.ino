/**
 * BLEThermo
 * 
 * Allen Snook
 * December 2020
 * 
 * Reads temperature from a DS18B20 one wire thermometer connected
 * to pin 5 on a Adafruit Feather nRF52840.
 * 
 * It then acts as a BLE Peripheral, implementing the Environmental
 * Sensing Service and Temperature Characteristic, notifying a connected
 * Central with the current temperature.
 */

#include <bluefruit.h>

// Most recent temperature measurement in degrees F
double dataBLE = 0;

// Most recent temperature measurement ready for BLE transport
uint8_t BLEData[2] = {0};

// GATT Service - 0x181A - Environmental Sensing
BLEService        es_svc = BLEService(UUID16_SVC_ALERT_NOTIFICATION);

// GATT Characteristic and Object Type - 0x2A6E - Temperature
BLECharacteristic tchar = BLECharacteristic(UUID16_CHR_ALERT_STATUS);

// DIS (Device Information Service) helper class instance
BLEDis bledis;

/**
 * Set up the Environmental Sensing Service (BLE)
 */
void setupESS() {
  es_svc.begin();

  // The temperature characteristic is two bytes, LSB then MSB
  // e.g. 2E09 -> 092E
  // then convert to decimal 092E -> 2350
  // then divide by 100 to get degC 2350 -> 23.5

  tchar.setProperties(CHR_PROPS_NOTIFY);
  tchar.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  tchar.setFixedLen(2);
  tchar.begin();

  // Optionally set an initial value for the temperature characteristic
  // uint8_t tdata[2] = { 0b00000110, 0x40 };
  // tchar.notify(tdata, 2);
}

/**
 * Start BLE Advertising
 */
void startAdvertising() {
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(es_svc);
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(160, 1600);   // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);
  Bluefruit.Advertising.start(0);
}

/**
 * On connect
 */
void connectCallback(uint16_t conn_handle) {
}

/**
 * On disconnect
 */
void disconnectCallback(uint16_t conn_handle, uint8_t reason) {
}

/**
 * Setup the Feather
 */
void setup() {
  Bluefruit.begin();
  Bluefruit.setName("AlertTester");
  Bluefruit.Periph.setConnectCallback(connectCallback);
  Bluefruit.Periph.setDisconnectCallback(disconnectCallback);

  bledis.setManufacturer("SDP TEAM 31");
  bledis.setModel("NRF52840-SDP");
  bledis.begin();

  setupESS();
  startAdvertising();
}

/**
 * The Feather's cyclic executive
 */
void loop() {
  if ( Bluefruit.connected() ) {
    Serial.println("Connected, sending notification");
    tchar.notify(BLEData, 2); // TODO - validate this is the correct structure
  } 
  
  else {
    Serial.println("Not connected");
  }

  dataBLE = 20; 
  int16_t dataBLEScaled = 100 * dataBLE;
  BLEData[0] = dataBLEScaled & 0xFF; // LSB
  BLEData[1] = (dataBLEScaled >> 8) & 0xFF; // MSB
}
