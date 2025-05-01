//anchor #4 setup
// be sure to edit anchor_addr and select the previously calibrated anchor delay
// my naming convention is anchors 1, 2, 3, ... have the lowest order byte of the MAC address set to 81, 82, 83, ...

#include <SPI.h>
#include <algorithm>
#include "DW1000Ranging.h"
#include "DW1000.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h> 

// leftmost two bytes below will become the "short address"
char anchor_addr[] = "83:00:5B:D5:A9:9A:E2:9C"; //#4

/*        PIN CONFIGS       */

// // ESP32-S3 pin configs
// #define SPI_SCK 12
// #define SPI_MISO 13
// #define SPI_MOSI 11
// #define DW_CS 9

// // connection pins
// const uint8_t PIN_RST = 5; // reset pin
// const uint8_t PIN_IRQ = 4; // irq pin
// const uint8_t PIN_SS = 9;   // spi select pin


// ESP32 ONLY pin configs
#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4

// connection pins
const uint8_t PIN_RST = 27; // reset pin
const uint8_t PIN_IRQ = 34; // irq pin
const uint8_t PIN_SS = 4;   // spi select pin


//calibrated Antenna Delay setting for this anchor
uint16_t Adelay = 16600; //starting value
float dist_m = 1; // calibration distance meters 
uint16_t Adelay_delta = 100; //initial binary search step size

//Kalman filter variables 
float estimate = 0; //Initial guess of distance 
float estimate_error = 1; //Initial guess of the estimate error 
float measurement_error = 0.04; //TODO: tune this measurement noise. Based on how jumpy your UWB readings are. If you see ±20 cm jumps, maybe set measurement_error = 0.04 m² = (20 cm)².
float process_noise = 0.1; //TODO: tune this process noise. Control how quickly you want the filter to adapt. Start low like 0.01, increase if it feels sluggish.
float kalman_gain = 0; 

//For list for taking the average 
const int windowSize = 5;
float distanceWindow[windowSize];
int windowIndex = 0;
bool windowReady = false;

float filteredAvgDistance = 0;


/* BLE Globals and Definitions*/

// Our unique service ID for data transmission
#define SERVICE_UUID        "ad560f3e-5103-4317-9541-eb28e8e4551b"

// Global stuff
BLEService *pService;
bool deviceConnected = false;
BLECharacteristic *pCharacteristic;

/*        BLUETOOTH CLASSES AND FUNCTIONS       */

// Callback function for BLE 
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;

    Serial.println("Client disconnected. Restart advertising...");
    pServer->startAdvertising();
  }
};

void setup()
{
  Serial.begin(115200);
  estimate = 0; 

  //while (!Serial); //wait for serial monitor to connect TODO: remove when testing without serial monitor!


  /*       BLE: init the configuration       */
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

  /*      UWB: init the configuration       */
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin

  Serial.print("Starting Adelay "); Serial.println(Adelay);
  Serial.print("Measured distance "); Serial.println(dist_m);
  
  // set antenna delay for anchors only. Tag is default (16384)
  DW1000.setAntennaDelay(Adelay);

  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachNewDevice(newDevice);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);

  //start the module as an anchor, do not assign random short address
  DW1000Ranging.startAsAnchor(anchor_addr, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false); //TODO: do we want to switch modes?
}

void loop()
{
  if (deviceConnected){
    DW1000Ranging.loop();
  }else{
    Serial.println("Device disconnected, waiting to reconnect");
  }
}

void newRange()
{
  static float last_delta = 0.0; //retains values between function calls

  if (Adelay_delta < 3) {
    // Done Callibrating the Antenna Delay
    // from here on its only distance calculations
    // set antenna delay for anchors only. Tag is default (16384)
    DW1000.setAntennaDelay(Adelay);
  
    float distance = DW1000Ranging.getDistantDevice()->getRange();
    KalmanFilter(distance);
    
    addToWindow(estimate);

    if(windowReady)
    {
      filteredAvgDistance = computeAverage();
      Serial.print("Final (sent) smoothed distance: ");
      Serial.println(filteredAvgDistance);

      pCharacteristic->setValue(String(filteredAvgDistance).c_str()); // set value to send
      pCharacteristic->notify();  // Notify the client -- this is what actually sends the value over to the client
    }

  }else{
    //Continue Antenna Callibration
    float dist = DW1000Ranging.getDistantDevice()->getRange();
    Serial.print("calibration distance: ");
    Serial.print(dist); 
    
    float this_delta = dist - dist_m;  //error in measured distance
  
    if ( this_delta * last_delta < 0.0) Adelay_delta = Adelay_delta / 2; //sign changed, reduce step size
    last_delta = this_delta;
    
    if (this_delta > 0.0 ) Adelay += Adelay_delta; //new trial Adelay
    else Adelay -= Adelay_delta;
  
    Serial.print(", Adelay = ");
    Serial.println (Adelay);
    DW1000.setAntennaDelay(Adelay);
  }
}

void newDevice(DW1000Device *device)
{
  Serial.print("Device added: ");
  Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device *device)
{
  Serial.print("Delete inactive device: ");
  Serial.println(device->getShortAddress(), HEX);
}

void KalmanFilter(float distance)
{
   //Kalman filter 
    estimate_error = estimate_error + process_noise; // predict step: increawse uncertainty 

    kalman_gain = estimate_error / (estimate_error + measurement_error); // update step
    estimate = estimate + kalman_gain * (distance - estimate);
    estimate_error = (1 - kalman_gain) * estimate_error;

//    Serial.print("Raw measurement: ");
//    Serial.print(distance);
//    Serial.print(" | Filtered estimate: ");
//    Serial.println(estimate);
}

void addToWindow(float distance)
{
  distanceWindow[windowIndex++] = distance;
  if (windowIndex >= windowSize)
  {
    windowReady = true;
    windowIndex = 0; // reset for next batch
  }
}

float computeAverage()
{
  if (!windowReady) return -1;

  //calculate mean 
  float sum;
  for(int i = 0; i < windowSize; i++)
  {
    sum += distanceWindow[i];
  }
  float mean = sum / windowSize;

  //remove outliers 
  sum = 0;
  int count = 0;
  for(int i = 0; i < windowSize; i++)
  {
    if(abs(distanceWindow[i] - mean) < 0.25 && distanceWindow[i] > 0)
    {
      sum += distanceWindow[i];
      count++;
    }
  }
  float average = sum/count;

  windowReady = false;
  if (count == 0) return mean; //TODO: change to median
  return average;
}
