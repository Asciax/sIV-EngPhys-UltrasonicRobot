#define M_PI 3.141592653589793238462643383279
#include <Servo.h>
#include <math.h>

Servo leftservo;  
Servo rightservo;  
const int pingPin = 5; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 6; // Echo Pin of Ultrasonic Sensor

const int target_d_1 = 110; // Target distance from the first wall.
const int target_d_2 = 70;  // Target distance from the second wall.
int target_d; // Target distance that the car currently tries to achieve.
int lower_bound_d; // Distance at which the car needs readjusting.
int higher_bound_d; // Distance at which the car assumes there is no wall.

float angular_orientation_radians = 0; // Angular orientation of the car in radians. 0 = poiting straight to the right
float total_angle = 0; // Total angle at which the car has turned.

bool turned_Wall = false; // Variable used to indicate that the robot must perform the turn when a gap is detected in the wall
bool turn_done = false;  // Variable used to prevent the car from turning more than once in case it detects a high distance again.
bool angular_correction = false; // Variable used to set the angular position in radians of DOWNWARDS as 0 when the car has turned, in order for the code to work for the second part

float u_s_distance; // Distance detected by the ultrasonic sensor
float distance; // Calculated shortest distance from the wall

float angle(float position) {
  return (position-target_d)*(-1.8)*pow(M_E,(-1/200)*pow((position-target_d),2))/M_PI;
}

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

  turn(90);


}


void turn(float angle) {

  float angular_period = 1.65;
  float angular_velocity_per_second = 2*M_PI/ angular_period;
  float rad_angle = (angle/360) *(2*M_PI);
  
  //Serial.print("Rad_angle: ");
  //Serial.println(rad_angle);

  float angular_time = rad_angle / angular_velocity_per_second;

  if (angular_time > 0) {

    //Serial.println("angular time is positive");

    leftservo.write(50);
    rightservo.write(50);
    delay(angular_time*1000);

    //Serial.print("delay left = ");
    //Serial.println(angular_time*1000);

    leftservo.write(130);
    rightservo.write(130);
    delay(10);

    leftservo.write(90);
    rightservo.write(90);
    delay(10);
    //Serial.println("I finished turning left");
    angular_orientation_radians += rad_angle;
  }

  else if (angular_time < 0) {
    //Serial.println("angular time is negative");
    //Serial.println(angular_time,4);

    leftservo.write(130);
    rightservo.write(130);
    delay(abs(angular_time*1000));

    //Serial.print("delay right = ");
    //Serial.println(abs(angular_time*1000));

    leftservo.write(50);
    rightservo.write(50);
    delay(10);

    leftservo.write(90);
    rightservo.write(90);
    delay(10);

    //Serial.println("I finished turning right");
  
    angular_orientation_radians += rad_angle;
  }

}


//Function for determining constants depending on whether car has turned to second wall or not
void distances_assignment(bool turned) {
   if (turned == false) { // If the car is trying to collect coins located parallel to the first wall
     target_d = target_d_1;
     lower_bound_d = (target_d_1 - 25);
     higher_bound_d = (target_d_1 + 125);
  }
  else if (turned == true) { // If the car is trying to collect coins parallel to the second wall
    target_d = target_d_2;
    lower_bound_d = (target_d_2 - 25);
    higher_bound_d = (target_d_2 + 125);
  }
}


/* Function to determine the true distance from the
wall, whether the car is at angle to the wall or
parallel to it.
*/
void true_distance(float angular_orientation) {
 if (angular_orientation == 0) {
    distance = u_s_distance;
  }

  else if (angular_orientation > 0) {
    distance =  (cosf(angular_orientation_radians-((15/360)*2*M_PI)) * u_s_distance);
  }

  else if (angular_orientation < 0) {
    distance = (cosf(abs(angular_orientation_radians)-((15/360)*2*M_PI)) * u_s_distance);
  }
}


void loop() {
  long duration;


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

}
