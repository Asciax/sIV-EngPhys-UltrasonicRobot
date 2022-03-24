#define M_PI 3.141592653589793238462643383279
#define M_E 2.718281828459045235360287471352
#include <Servo.h>
#include <math.h>

Servo leftservo;  
Servo rightservo;  
const int pingPin = 5; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 6; // Echo Pin of Ultrasonic Sensor
float distances[3];
float kernel[] = {0.3333,0.3333,0.3333};
int distances_counter = 0;

float angular_orientation_radians = 0;
int hasTurn = 0;

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

float angle(float position) {
  float a = (position-110)*(-1.8)*pow(M_E,(-1/200)*pow(position-110,2))/M_PI;
  return a;
}

float delay_func(float orientation) {
  return 0.5*pow(M_E,(-M_PI/4)*pow(orientation,2));
}

void turn(float angle) {

  float angular_period = 5.970;
  float angular_velocity_per_second = 2*M_PI/ angular_period;
  float rad_angle = (angle/360) *(2*M_PI);
  
  //Serial.print("Rad_angle: ");
  //Serial.println(rad_angle);

  float angular_time = rad_angle / angular_velocity_per_second;

  if (angular_time > 0) {

    //Serial.println("angular time is positive");

    leftservo.write(0);
    rightservo.write(0);
    delay(angular_time*1000);

    //Serial.print("delay left = ");
    //Serial.println(angular_time*1000);

    // leftservo.write(90);
    // rightservo.write(90);
    // delay(100);
    //Serial.println("I finished turning left");
    angular_orientation_radians += rad_angle;
  }

  else if (angular_time < 0) {
    //Serial.println("angular time is negative");
    //Serial.println(angular_time,4);

    leftservo.write(180);
    rightservo.write(180);
    delay(abs(angular_time*1000));

    //Serial.print("delay right = ");
    //Serial.println(abs(angular_time*1000));

    // leftservo.write(90);
    // rightservo.write(90);
    // delay(100);

    //Serial.println("I finished turning right");
  
    angular_orientation_radians += rad_angle;
  }
  //Serial.println("The angular orientation is " + String(angular_orientation_radians));

}


void loop() {
  long duration;
  float u_s_distance;
  float distance;
  float avg_distance;

  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);

  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  u_s_distance = duration*0.034/2;

  // Serial.print("Angular orientation of the car: ");
  // Serial.println(angular_orientation_radians);

  // Serial.print("Sensor distance: ");
  // Serial.println(u_s_distance);


  if (angular_orientation_radians == 0) {
    distance = u_s_distance;
  }

  else if (angular_orientation_radians > 0) {
    distance =  (cosf(angular_orientation_radians-((15/360)*2*M_PI)) * u_s_distance);
  }

  else if (angular_orientation_radians < 0) {
    distance = (cosf(abs(angular_orientation_radians)-((15/360)*2*M_PI)) * u_s_distance);
  }

  // Serial.print("Actual distance: ");
  // Serial.println(distance);

  distances[distances_counter] = distance;
  distances_counter = (distances_counter+1)%3;

  for (byte i = 0; i < 3; i++) {
    if (distances[i] == 0.0) {
      avg_distance = distances[(distances_counter-1)%3];
      break;
    } else {
      avg_distance += distances[i]*kernel[i];
    }
  }

  Serial.println("Avg distance: ");
  Serial.print(avg_distance);

  float turn_in_deg = angle((distance+avg_distance)/2);
  Serial.println("The robot should turn by " + String(turn_in_deg));

  turn(-turn_in_deg);
  rightservo.write(0);
  leftservo.write(180);
  delay(200);

}
