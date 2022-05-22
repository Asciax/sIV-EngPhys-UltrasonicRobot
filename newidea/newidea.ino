#define M_PI 3.141592653589793238462643383279
#define M_E 2.718281828459045235360287471352
#include <Servo.h>
#include <math.h>

Servo leftservo;  
Servo rightservo;  
const int pingPin = 5; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 6; // Echo Pin of Ultrasonic Sensor

float initDistance;


void setup() {
  leftservo.attach(10);  
  rightservo.attach(9);
   //set up the Serial
  Serial.begin(9600);
  //setupt the pin modes  
  pinMode(pingPin, OUTPUT);
  pinMode(echoPin, INPUT);


  long initDuration;

  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  initDuration = pulseIn(echoPin, HIGH);
  initDistance = initDuration*0.034/2;

  Serial.print(" Initial Distance: ");
  Serial.println(initDistance);

  rightservo.write(0);
  leftservo.write(180);
  delay(2000);

  rightservo.write(90);
  leftservo.write(90);
  //stop();
  //turn(-90);
  //stop();
}

void move(int time) {
  rightservo.write(0);
  leftservo.write(180);
  delay(time);
}

void stop() {
  rightservo.write(90);
  leftservo.write(90);
}

void turn(float angle) {

  float angular_period = 1.95;
  //5.970;
  float angular_velocity_per_second = 2*M_PI/ angular_period;
  //1.954768762;
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

  }
  //Serial.println("The angular orientation is " + String(angular_orientation_radians));

}

float straight_distance() {
  long duration;
  stop();
  float distanceArray[3] = {initDistance, initDistance, initDistance};

  for (int i=0; i<=2; i++){
    digitalWrite(pingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(pingPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(pingPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    float distance = duration*0.034/2;
    Serial.print("Distance: ");
    Serial.println(distance);
    distanceArray[i] = distance;
    Serial.println(distanceArray[i]);
  }


  Serial.println("Distance array: ");
  Serial.println(distanceArray[0]);
  Serial.println(distanceArray[1]);
  Serial.println(distanceArray[2]);
  float finalDistance = (distanceArray[0] + distanceArray[1] + distanceArray[2]) / 3 ;
  return finalDistance;
}

float straight_line(int time) {
  float straight = time * 45/1000;
  float di;
  float df;

  di = straight_distance();
  move(time);
  df = straight_distance();
  //Serial.println(di-df);
  float angle = atan((di-df)/straight)*(360/(2*M_PI));
  if (df > 200) {
      return -90;
  } else {
      return angle;
  }
}

void loop() {
  // 45cm/s
  // 1.954768762 rad/s
  // float distance = straight_distance();
  // turn(90);
  // Serial.println(distance);
  // stop();
  // move(2000);
  // turn(-90);
  // stop(); 
  float angle = straight_line(750);
  Serial.println(angle);
  turn(angle);
}
