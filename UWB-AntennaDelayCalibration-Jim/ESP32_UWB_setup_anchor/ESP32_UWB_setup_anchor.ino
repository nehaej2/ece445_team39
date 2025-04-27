//anchor #4 setup


// be sure to edit anchor_addr and select the previously calibrated anchor delay
// my naming convention is anchors 1, 2, 3, ... have the lowest order byte of the MAC address set to 81, 82, 83, ...

#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000.h"

// leftmost two bytes below will become the "short address"
char anchor_addr[] = "84:00:5B:D5:A9:9A:E2:9C"; //#4

#define SPI_SCK 12
#define SPI_MISO 13
#define SPI_MOSI 11
#define DW_CS 9

// connection pins
const uint8_t PIN_RST = 5; // reset pin
const uint8_t PIN_IRQ = 4; // irq pin
const uint8_t PIN_SS = 9;   // spi select pin


//calibrated Antenna Delay setting for this anchor
uint16_t Adelay = 16600; //starting value
float dist_m = 1.5; // calibration distance meters 
uint16_t Adelay_delta = 100; //initial binary search step size

//Kalman filter variables 
float estimate = 0; //Initial guess of distance 
float estimate_error = 1; //Initial guess of the estimate error 
float measurement_error = 4; //TODO: tune this measurement noise. Based on how jumpy your UWB readings are. If you see ±20 cm jumps, maybe set measurement_error = 0.04 m² = (20 cm)².
float process_noise = 0.05; //TODO: tune this process noise. Control how quickly you want the filter to adapt. Start low like 0.01, increase if it feels sluggish.
float kalman_gain = 0; 

void setup()
{
  Serial.begin(115200);
  estimate = 0; 

  while (!Serial); //wait for serial monitor to connect TODO: remove when testing without serial monitor!

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
  // DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_SHORTDATA_FAST_LOWPOWER);
  // DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_LONGDATA_FAST_LOWPOWER);
  // DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_SHORTDATA_FAST_ACCURACY);
  // DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_LONGDATA_FAST_ACCURACY);
  // DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_LONGDATA_RANGE_ACCURACY);
}

void loop()
{
  DW1000Ranging.loop();
}

void newRange()
{
  static float last_delta = 0.0; //retains values between function calls

  if (Adelay_delta < 3) {
    // Done Callibrating the Antenna Delay
    // from here on its only distance calculations
    // set antenna delay for anchors only. Tag is default (16384)
    DW1000.setAntennaDelay(Adelay);
    Serial.print(", final Adelay ");
    Serial.println(Adelay);
    Serial.print("from: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), DEC);
    Serial.print(", ");
  
    float distance = DW1000Ranging.getDistantDevice()->getRange();

    //Kalman filter 
    estimate_error = estimate_error + process_noise; // predict step: increawse uncertainty 

    kalman_gain = estimate_error / (estimate_error + measurement_error); // update step
    estimate = estimate + kalman_gain * (distance - estimate);
    estimate_error = (1 - kalman_gain) * estimate_error;

    Serial.print("Raw measurement: ");
    Serial.print(distance);
    Serial.print(" | Filtered estimate: ");
    Serial.println(estimate);

  }else{
    //Continue Antenna Callibration
    float dist = DW1000Ranging.getDistantDevice()->getRange();
    Serial.print("distance: ");
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
