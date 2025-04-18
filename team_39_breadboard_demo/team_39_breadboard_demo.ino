//include statements
#include <math.h>

// Motor Pin Definitions

// From breadboard demo
//#define AIN1 15
//#define AIN2 16
//#define PWMA 17
//
//#define BIN1 6
//#define BIN2 5
//#define PWMB 4
//
//#define STBY 7

// For current testing 4/17
#define AIN1 8 //
#define AIN2 18 //
#define PWMA 17 //

#define BIN1 46
#define BIN2 9
#define PWMB 14

#define STBY 3

// PWM Set-up
#define BASE_FREQ 5000
#define RESOL 8 // With 8 bit resolution, the duty ranges from 0->255
#define MOTOR_A_CHANNEL 1
#define MOTOR_B_CHANNEL 2


// Motor labels
#define MOTORA 1
#define MOTORB 2

// Distances
#define STOP_DISTANCE 1.5 // wagon will stop when within 1.5 m of user
#define MAX_DISTANCE 3    // wagon will stop accelerating when distance to user exceeds 3 m

//#define PWM_FREQ 78000 //100kHz
//#define PWM_RESOLUTION 10 //will generate 78.1kHz

void setup() {
  // put your setup code here, to run once:

  // Enable PWM communication. Can enable via pin or channel. 
  // Channels can be connected to more than 1 device/motor
  // For now, we can use 1 channel per motor
//  Serial.begin(9600);
//  bool frequencySetA = true;
//  bool frequencySetB = true;
//  frequencySetA = ledcAttach(PWMA, PWM_FREQ, PWM_RESOLUTION);
//  frequencySetB = ledcAttach(PWMB, PWM_FREQ, PWM_RESOLUTION);
//  Serial.print(frequencySetA);
//  Serial.print(frequencySetB);

  ledcAttachChannel(PWMA, BASE_FREQ, RESOL, MOTOR_A_CHANNEL);
  ledcAttachChannel(PWMB, BASE_FREQ, RESOL, MOTOR_B_CHANNEL);

  // MUST initialize pins to be input or output for use
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
}

void loop() {

  brake();
  
  fwd_v2(3, 2.6);

  delay(3000);

  brake();

  delay(3000);


 
  fwd_v2(2.6, 3);

  delay(3000);

  brake();

  delay(3000);



  fwd_v2(3, 3);

  delay(3000);
  
  brake();

  delay(3000);
}

/*
void fwd(int speed, int motor){

  if(motor == MOTORA){
      digitalWrite(STBY, HIGH);
      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN2, LOW);
      ledcWriteChannel(MOTOR_A_CHANNEL,speed);
  }
  
  else{
    digitalWrite(STBY, HIGH);
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    ledcWriteChannel(MOTOR_B_CHANNEL,speed);
  }
 
}
*/

/*
void fwd_median(double median_distance){
    if(median_distance < 1.5)                               // set duty cycle to 0
      return;
    else if(median_distance >= 1.5 && median_distance < 3){ // linearly duty cycle between 1.5 and 3
      int scaled_dc = 170 * median_distance - 255;
      digitalWrite(STBY, HIGH);
      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN2, LOW);
      ledcWriteChannel(MOTOR_A_CHANNEL, scaled_dc);
      digitalWrite(BIN1, HIGH);
      digitalWrite(BIN2, LOW);
      ledcWriteChannel(MOTOR_B_CHANNEL, scaled_dc);
    }
    else{                                                   // max out duty cycle
      digitalWrite(STBY, HIGH);
      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN2, LOW);
      ledcWriteChannel(MOTOR_A_CHANNEL, 255);
      digitalWrite(BIN1, HIGH);
      digitalWrite(BIN2, LOW);
      ledcWriteChannel(MOTOR_B_CHANNEL, 255);
  }
}
*/

void fwd_v2(double a, double b){
    digitalWrite(STBY, HIGH);
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    ledcWriteChannel(MOTOR_A_CHANNEL, 100);
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    ledcWriteChannel(MOTOR_B_CHANNEL, 100);
}

/*
void fwd_v2(double a, double b){
    if(get_median(a, b) < STOP_DISTANCE) // set duty cycle to 0
      return;
    else if(get_median(a, b) >= STOP_DISTANCE && get_median(a, b) < MAX_DISTANCE){ // linearly duty cycle between 1.5 and 3
      
      int scaled_dc = 170 * get_median(a, b) - 255;
      digitalWrite(STBY, HIGH);

      //if angle a < angle b
      if(get_angle_a(a,b) < get_angle_b(a,b)){
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
        ledcWriteChannel(MOTOR_A_CHANNEL, scaled_dc);

        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
        ledcWriteChannel(MOTOR_B_CHANNEL, int(scaled_dc * 1)); //decrease duty cycle of motor b

      //if angle a > angle b
      } else if (get_angle_a(a,b) > get_angle_b(a,b)){
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
        ledcWriteChannel(MOTOR_A_CHANNEL, int(scaled_dc * 1)); //decrease duty cycle of motor a

        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
        ledcWriteChannel(MOTOR_B_CHANNEL, scaled_dc);
      }
    }
    else{ // max out duty cycle
      digitalWrite(STBY, HIGH);
      
      //if angle a < angle b
      if(get_angle_a(a,b) < get_angle_b(a,b)){
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
        ledcWriteChannel(MOTOR_A_CHANNEL, 255);

        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
        ledcWriteChannel(MOTOR_B_CHANNEL, int(255 * 1)); //decrease duty cycle of motor b

      //if angle a > angle b
      } else if (get_angle_a(a,b) > get_angle_b(a,b)){
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
        ledcWriteChannel(MOTOR_A_CHANNEL, int(255 * 1)); //decrease duty cycle of motor a

        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
        ledcWriteChannel(MOTOR_B_CHANNEL, 255);
      }
  }
}
*/

/*
void fwd(int left_speed, int right_speed, double distA, double distB){

  if(motor == MOTORA){
      digitalWrite(STBY, HIGH);
      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN2, LOW);
      ledcWriteChannel(MOTOR_A_CHANNEL,speed);
  }
  
  else{
    digitalWrite(STBY, HIGH);
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    ledcWriteChannel(MOTOR_B_CHANNEL,speed);
  }
}
*/

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





// Side notes: 
// - have to keep common ground for motor driver and ESP32 for PWM to work properly
// - might need a logic level shifter cause the voltage signals out of the ESP32 are hella low
