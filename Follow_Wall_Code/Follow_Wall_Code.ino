// Follow the wall exercise
// by Caterev Robert, Ma Coty, Raharison Toky
// 01-03-2022


#define M_PI 3.141592653589793238462643383279
#include <Servo.h>
#include <math.h>

Servo leftservo;  
Servo rightservo;  
const int pingPin = 5; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 6; // Echo Pin of Ultrasonic Sensor
const int target_d = 110; // Target Distance
const int lower_bound_d = 95; // Distance at which the car needs readjusting.


float angular_orientation_radians = 0; // Angular orientation of the car in radians. 0 = poiting straight to the right
float total_angle = 0; // Total angle at which the car has turned.
int d_buffer = 15; //Buffer for the turn_angle adjustment when the car gets closer to the target distance.

void setup() {
  leftservo.attach(9);  
  rightservo.attach(10);
   //set up the Serial
  Serial.begin(9600);
  //setupt the pin modes  
  pinMode(pingPin, OUTPUT);
  pinMode(echoPin, INPUT);



  leftservo.write(90);
  rightservo.write(90);


}


void turn(float angle) {

  float angular_period = 8.772;
  float angular_velocity_per_second = 2*M_PI/ angular_period;
  float rad_angle = (angle/360) *(2*M_PI);
  
  //Serial.print("Rad_angle: ");
  //Serial.println(rad_angle);

  float angular_time = rad_angle / angular_velocity_per_second;

  if (angular_time > 0) {

    //Serial.println("angular time is positive");

    leftservo.write(30);
    rightservo.write(30);
    delay(angular_time*1000);

    //Serial.print("delay left = ");
    //Serial.println(angular_time*1000);

    leftservo.write(90);
    rightservo.write(90);
    delay(100);
    //Serial.println("I finished turning left");
    angular_orientation_radians += rad_angle;
  }

  else if (angular_time < 0) {
    //Serial.println("angular time is negative");
    //Serial.println(angular_time,4);

    leftservo.write(150);
    rightservo.write(150);
    delay(abs(angular_time*1000));

    //Serial.print("delay right = ");
    //Serial.println(abs(angular_time*1000));

    leftservo.write(90);
    rightservo.write(90);
    delay(100);

    //Serial.println("I finished turning right");
  
    angular_orientation_radians += rad_angle;
  }

}


void loop() {
  long duration;
  float u_s_distance;
  float distance;

  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);

  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  u_s_distance = duration*0.034/2;

  Serial.print("Angular orientation of the car: ");
  Serial.println(angular_orientation_radians);

  Serial.print("Sensor distance: ");
  Serial.println(u_s_distance);


  if (angular_orientation_radians == 0) {
    distance = u_s_distance;
  }

  else if (angular_orientation_radians > 0) {
    distance =  (cosf(angular_orientation_radians-((15/360)*2*M_PI)) * u_s_distance);
  }

  else if (angular_orientation_radians < 0) {
    distance = (cosf(abs(angular_orientation_radians)-((15/360)*2*M_PI)) * u_s_distance);
  }


  Serial.print("Actual distance: ");
  Serial.println(distance); 
    
  float turn_angle;

  if (distance > target_d) {

    turn_angle = 10 * (distance/ (target_d + d_buffer));
    turn(turn_angle);
    
    rightservo.write(0);
    leftservo.write(180);
    delay(500);
    
    total_angle += turn_angle;
    Serial.print("Total Angle Turned :");
    Serial.println(total_angle);

  }

  else if ((distance <= target_d) && (distance >= lower_bound_d)) {

    if (total_angle > 0) {
      int correction_factor = 2; //Due to inaccuracy, a 2 degree correction factor has to be introduced.
      turn((-1 * total_angle) + correction_factor);
      total_angle = 0;
    }
    
    rightservo.write(0);
    leftservo.write(180);
    delay(500);
    
  }

  else if (distance < lower_bound_d) {
    
    Serial.print("Correcting direction because we are below");
    Serial.print(lower_bound_d);
    Serial.println("distance from the wall");
    turn(-5);
    rightservo.write(0);
    leftservo.write(180);
    delay(500);
  }
  rightservo.write(90);
  leftservo.write(90);
  delay(100);


}
