/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h> 


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

// Our unique service ID for data transmission
#define SERVICE_UUID        "ad560f3e-5103-4317-9541-eb28e8e4551b"

// Global stuff
BLEService *pService;
bool deviceConnected = false;
BLECharacteristic *pCharacteristic;

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("Long name works now");
  BLEServer *pServer = BLEDevice::createServer();

  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Defining the distance characteristic -- notify so we can send value over
  pCharacteristic = pService->createCharacteristic(
    "a9e3380c-3391-47e7-91e1-9eaa9bd0e5e5",
    BLECharacteristic::PROPERTY_NOTIFY
  );

  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->start();
  Serial.println("Waiting a client connection to notify...");
}


void loop() {
  // put your main code here, to run repeatedly:

  // dummy value -- will need to send actual distance values from here
  if (deviceConnected) {
    float distance = 1.23;  // replace with UWB distance values

    // Send distance as string
    pCharacteristic->setValue(String(distance).c_str()); // set value to send
    pCharacteristic->notify();  // Notify the client -- this is what actually sends the value over to the client

    Serial.print("Sent distance: ");
    Serial.println(distance);

    delay(1000);  // Send every second
  }

}
