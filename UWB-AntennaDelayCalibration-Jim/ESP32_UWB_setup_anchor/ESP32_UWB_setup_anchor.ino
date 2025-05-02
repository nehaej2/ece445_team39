// ANCHOR 2 CODE 
#include <SPI.h>
#include <algorithm>
#include "DW1000Ranging.h"
#include "DW1000.h"

// leftmost two bytes below will become the "short address"
char anchor_addr[] = "83:00:5B:D5:A9:9A:E2:9C"; //#4

// ESP 32 PORTS
#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4
const uint8_t PIN_RST = 27; // reset pin
const uint8_t PIN_IRQ = 34; // irq pin
const uint8_t PIN_SS = 4;   // spi select pin

// Motor driver pins 
#define AIN1 25
#define AIN2 16 
#define PWMA 17 
#define BIN1 14
#define BIN2 13
#define PWMB 32
#define STBY 26

// Calibrated Antenna Delay setting for this anchor
uint16_t Adelay = 16600; //starting value
float dist_m = 1; // calibration distance meters 
uint16_t Adelay_delta = 100; //initial binary search step size

// Kalman filter variables 
float estimate = 0; //Initial guess of distance 
float estimate_error = 1; //Initial guess of the estimate error 
float measurement_error = 0.04; //TODO: tune this measurement noise. Based on how jumpy your UWB readings are. If you see ±20 cm jumps, maybe set measurement_error = 0.04 m² = (20 cm)².
float process_noise = 0.1; //TODO: tune this process noise. Control how quickly you want the filter to adapt. Start low like 0.01, increase if it feels sluggish.
float kalman_gain = 0; 

// List for taking the average 
const int windowSize = 5;
float distanceWindow[windowSize];
int windowIndex = 0;
bool windowReady = false;

// PWM Set-up
#define BASE_FREQ 5000
#define RESOL 8 // With 8 bit resolution, the duty ranges from 0->255
#define MOTOR_A_CHANNEL 1
#define MOTOR_B_CHANNEL 2

// Motor labels
#define MOTORA 1
#define MOTORB 2

//FAKE RIGHT WHEEL VALUES
float RW_1 = 1;
float RW_2 = 1.5;
float RW_3 = 1.8;

void setup()
{
  Serial.begin(115200);
  estimate = 0; 
  //while (!Serial); //wait for serial monitor to connect TODO: remove when testing without serial monitor!

  // Init the configuration
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin

  Serial.print("Starting Adelay "); Serial.println(Adelay);
  Serial.print("Measured distance "); Serial.println(dist_m);
  
  // Set antenna delay for anchors only. Tag is default (16384)
  DW1000.setAntennaDelay(Adelay);

  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachNewDevice(newDevice);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);

  //start the module as an anchor, do not assign random short address
  DW1000Ranging.startAsAnchor(anchor_addr, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false); //TODO: do we want to switch modes?

   /* MOTOR SET UP */
  // MUST initialize pins to be input or output for use 
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  
  ledcAttachChannel(PWMA, BASE_FREQ, RESOL, MOTOR_A_CHANNEL);
  ledcAttachChannel(PWMB, BASE_FREQ, RESOL, MOTOR_B_CHANNEL);
}

void loop()
{
  DW1000Ranging.loop();
}

void newRange()
{
  static float last_delta = 0.0; //retains values between function calls

  if (Adelay_delta < 3) {// Done Callibrating the Antenna Delay. From here on its only distance calculations. Set antenna delay for anchors only. Tag is default (16384)
    DW1000.setAntennaDelay(Adelay);
  
    float distance = DW1000Ranging.getDistantDevice()->getRange();
    KalmanFilter(distance);
    
    addToWindow(estimate);

    if(windowReady)
    {
      float filteredAvgDistance = computeAverage();
      Serial.print("Final smoothed distance: ");
      Serial.println(filteredAvgDistance);
      //fwd_v2(filteredAvgDistance, filteredAvgDistance); // Forward 
      fwd_v2(filteredAvgDistance, 2); // Testing turning on left wheel 
    }

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

void fwd_v2(double a, double b){
    double angle_a = get_angle_a(a, b);
    double angle_b = get_angle_b(a, b);
    double difference = abs(angle_a - angle_b);
    double median_dist = get_median(a, b);
    int scaled_dc = fmin(170 * median_dist - 255, 255);
    
    // is user within 1.5 m of wagon?
    if(median_dist < 1.5){
        Serial.println("brake");
        brake();
        return;
    }else if (median_dist >= 1.5){
        // case 1: a and b within .2 radians (about 11.5 degrees)
        if(difference < .3){
            // go straight
            straight(a, b, scaled_dc);
            Serial.println("straight");
        }
        // case 2: a > b
        else if(angle_a > angle_b && difference >= .2){
            // decrease duty cycle of a 
            left(a, b, scaled_dc);
            Serial.println("left");
        }
        // case 3: a < b
        else if (angle_a < angle_b && difference >= .2){
            // decrease duty cycle of b
            right(a, b, scaled_dc);
            Serial.println("right");
        }
    }
    
}

void left(double a, double b, int scaled_dc){
    digitalWrite(STBY, HIGH);
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    ledcWriteChannel(MOTOR_A_CHANNEL, int(scaled_dc * .7)); //decrease duty cycle of motor a
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    ledcWriteChannel(MOTOR_B_CHANNEL, scaled_dc);
}

void straight(double a, double b, int scaled_dc){
    digitalWrite(STBY, HIGH);
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    ledcWriteChannel(MOTOR_A_CHANNEL, scaled_dc); //decrease duty cycle of motor a
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    ledcWriteChannel(MOTOR_B_CHANNEL, scaled_dc);
}

void right(double a, double b, int scaled_dc){
    digitalWrite(STBY, HIGH);
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    ledcWriteChannel(MOTOR_A_CHANNEL, scaled_dc); 
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    ledcWriteChannel(MOTOR_B_CHANNEL, int(scaled_dc * .7)); //decrease duty cycle of motor a
}

// Stops both motors
void brake(){
  digitalWrite(STBY, HIGH);
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, HIGH);
  
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, HIGH);
  ledcWriteChannel(MOTOR_A_CHANNEL,0);
  ledcWriteChannel(MOTOR_B_CHANNEL,0);
}

/*
 * get_angle_a:
 * returns: angle a in radians
 * 
 * parameters:
 * a: length of left motor to user
 * b: length of right motor to user
*/
double get_angle_a(double a, double b){
  return acos((a * a + 0.457 * 0.457 - b * b)/(2 * a * 0.457));
}

/*
 * get_angle_b:
 * returns: angle a in radians
 * 
 * parameters:
 * a: length of left motor to user
 * b: length of right motor to user
*/
double get_angle_b(double a, double b){
  return acos((b * b + 0.457 * 0.457 - a * a)/(2 * b * 0.457));
}

/*
 * get_median:
 * returns: length of median of triangle from middle of two motors to user
 * 
 * parameters:
 * a: length of left motor to user
 * b: length of right motor to user
*/
double get_median(double a, double b){
  return 0.5 * sqrt((2 * a * a) + (2 * b * b) - (0.457 * 0.457));
}
