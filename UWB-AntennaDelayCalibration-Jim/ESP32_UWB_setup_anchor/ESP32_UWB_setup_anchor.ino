//anchor #4 setup
// be sure to edit anchor_addr and select the previously calibrated anchor delay
// my naming convention is anchors 1, 2, 3, ... have the lowest order byte of the MAC address set to 81, 82, 83, ...

/*

IMPORTANT (MOTOR SUBSYSTEM -- CLIENT):
This will be our motor subsystem anchor. For bluetooth communication it will be configured to be the client
so we can receive data from the other anchor. 

*/

#include <SPI.h>
#include <algorithm>
#include "DW1000Ranging.h"
#include "DW1000.h"

#include "BLEDevice.h"

/*      UWB Global Variables and Definitions       */

// leftmost two bytes below will become the "short address"
char anchor_addr[] = "84:00:5B:D5:A9:9A:E2:9C"; //#4


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

/*       BLE GLobals and Definitions       */

// The remote service we wish to connect to -- same on server side
static BLEUUID serviceUUID("ad560f3e-5103-4317-9541-eb28e8e4551b");

// The characteristic of the remote service we are interested in -- distance characteristic
static BLEUUID distanceCharacteristicUUID("a9e3380c-3391-47e7-91e1-9eaa9bd0e5e5");

// Global stuff
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLEAdvertisedDevice *myDevice;

static BLERemoteCharacteristic* distanceCharacteristic;

float distanceVal;
boolean newDistance = false;


/*        BLE CLASSES AND FUNCTIONS       */

// BLE Callback function
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient *pclient) {}

  void onDisconnect(BLEClient *pclient) {
    connected = false;
    doScan = true;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient *pClient = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");
  delay(50);
  pClient->setMTU(517);  //set client to request maximum MTU from server (default is 23 otherwise)

  // Obtain a reference to the service we are after in the remote BLE server.
  Serial.println(" - Getting remote service now...");
  BLERemoteService *pRemoteService = pClient->getService("ad560f3e-5103-4317-9541-eb28e8e4551b");
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }

  Serial.println(" - Found our service");

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  distanceCharacteristic = pRemoteService->getCharacteristic(distanceCharacteristicUUID);
  if(distanceCharacteristic == nullptr){
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(distanceCharacteristicUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our characteristic");
  distanceCharacteristic->registerForNotify(distanceNotifyCallback);

  connected = true;
  return true;
}

/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = false;

    }  // Found our server
  }  // onResult
};  // MyAdvertisedDeviceCallbacks

// Triggered when a notification is pushed on the server side
static void distanceNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                                        uint8_t* pData, size_t length, bool isNotify) {
  //store distance value
  char* distanceChar = (char*)pData;
  distanceVal = atof(distanceChar);
  newDistance = true;
}

void setup()
{
  Serial.begin(115200);
  estimate = 0; 

  //while (!Serial); //wait for serial monitor to connect TODO: remove when testing without serial monitor!

  /* BLE SET UP */

  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);

  /* UWB SET UP */

  //init the configuration
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
  DW1000Ranging.loop();

  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothing more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    if(newDistance){
      Serial.print("Received distance value: ");
      Serial.println(distanceVal);
      Serial.print(", ");
      newDistance = false;
    }
  } else if (doScan) {
    Serial.print("Attempting to scan");
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
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
    // Serial.print(", final Adelay ");
    // Serial.println(Adelay);
  
    float distance = DW1000Ranging.getDistantDevice()->getRange();
    KalmanFilter(distance);
    
    addToWindow(estimate);

    if(windowReady)
    {
      filteredAvgDistance = computeAverage();
      Serial.print("This device final smoothed distance: ");
      Serial.println(filteredAvgDistance);
    }

  }else{
    //Continue Antenna Callibration
    float dist = DW1000Ranging.getDistantDevice()->getRange();
    Serial.print("calibrated distance: ");
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
