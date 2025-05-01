//include statements
#include <math.h>

// Motor Pin Definitions
#define AIN1 27
#define AIN2 26
#define PWMA 25 

#define BIN1 12
#define BIN2 13
#define PWMB 15

#define STBY 14

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
  
  Serial.begin(115200);
  bool frequencySetA = true;
  bool frequencySetB = true;
  
  
  // MUST initialize pins to be input or output for use 
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  
  frequencySetA = ledcAttachChannel(PWMA, BASE_FREQ, RESOL, MOTOR_A_CHANNEL);
  frequencySetB = ledcAttachChannel(PWMB, BASE_FREQ, RESOL, MOTOR_B_CHANNEL);
  Serial.print(frequencySetA);
  Serial.print(frequencySetB);
}

void loop() {
  /*
  //  fwd_v2(3, 2.6);
  //
  //  delay(1000);
  //
  //  fwd_v2(2.6, 3);
  //
  //  delay(1000);
  //
  //  fwd_v2(3, 3);
  //  
  //  delay(1000);
  */
  
  
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(STBY, HIGH);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  
  ledcWriteChannel(MOTOR_B_CHANNEL, 10);
  ledcWriteChannel(MOTOR_A_CHANNEL, 10);
  
  
}


void fwd_v2(double a, double b){
    double angle_a = get_angle_a(a, b);
    double angle_b = get_angle_b(a, b);
    double difference = abs(angle_a - angle_b);
    double median_dist = get_median(a, b);
    int scaled_dc = fmin(int(84.67 * median_dist - 127), (127));
    
    // is user within 1.5 m of wagon?
    if(median_dist < 1.5){
        return;
    }else if (median_dist >= 1.5){
        // case 1: a and b within .2 radians (about 11.5 degrees)
        if(difference < .2){
            // go straight
            straight(a, b, scaled_dc);
        }
        // case 2: a > b
        else if(angle_a > angle_b && difference >= .2){
            // decrease duty cycle of a 
            left(a, b, scaled_dc);
        }
        // case 3: a < b
        else if (angle_a < angle_b && difference >= .2){
            // decrease duty cycle of b
            right(a, b, scaled_dc);
        }
    }
    
}

void left(double a, double b, int scaled_dc){
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    ledcWriteChannel(MOTOR_A_CHANNEL, int(scaled_dc * .7)); //decrease duty cycle of motor a
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    ledcWriteChannel(MOTOR_B_CHANNEL, scaled_dc);
}

void straight(double a, double b, int scaled_dc){
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    ledcWriteChannel(MOTOR_A_CHANNEL, scaled_dc); //decrease duty cycle of motor a
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    ledcWriteChannel(MOTOR_B_CHANNEL, scaled_dc);
}

void right(double a, double b, int scaled_dc){
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

double get_angle_a(double a, double b){
  return acos((a * a + 0.2 * 0.2 - b * b)/(2 * a * 0.2));
}

double get_angle_b(double a, double b){
  return acos((b * b + 0.2 * 0.2 - a * a)/(2 * b * 0.2));
}

double get_median(double a, double b){
  return 0.5 * sqrt((2 * a * a) + (2 * b * b) - (0.2 * 0.2));
}
