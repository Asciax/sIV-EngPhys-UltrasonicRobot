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
  stop();
}

void move(int time) {
  /*
  A function for moving the car in a straight line for a certain direction.
  This can be convenient for calibration because we only have to change the speeds once.
  Inputs:
    time: time for moving forward (ms)
  Returns: None
  */
  rightservo.write(0);
  leftservo.write(180);
  delay(time);
}

void stop() {
  /*
  A function that stops the car for convenience
  Inputs: None
  Returns: None
  */
  rightservo.write(90);
  leftservo.write(90);
}

void turn(float angle) {
  /*
  A function for turning the car by a certain degree.
  This can be done by calculating the required delay from the angular velocity
  Inputs:
    angle: angle to turn by (degree)
  Returns: None
  */

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
  /*
  A function for calculating the distance measured by the ultrasonic sensor
  Inputs: None
  Returns:
    float: distance measured
  */
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
  /*
  A function for moving the car in a straight line for a certain direction and
  calculate the correct angular correction
  Inputs:
    time: time for moving forward (ms)
  Returns: 
    float: the turn the robot should make in degrees
  */
  float straight = time * 45/1000;
  float di;
  float df;

  di = straight_distance();//we first calculate the initial distance
  move(time);//we move in a straight line
  df = straight_distance();//we calculate the final distance
  //Serial.println(di-df);
  float angle = atan((di-df)/straight)*(360/(2*M_PI));//we use some trigonometry to calculate the angle the robot makes relative to the wall
  if (df > 200) {
    //if the distance is greater than 200cm (there's a gap) then the robot should make a(n almost) sharp turn
      return -90;
  } else {
    //if the distance is not greater than 200cm then the robot should just correct its orientation to be parallel to the wall
      return angle;
  }
}

void loop() {
  // speed 45cm/s
  // angular speed 1.954768762 rad/s
  float angle = straight_line(1000);//the robot moves in a straight line for 1 second and we calculate the appropriate turn according to our logic
  Serial.println(angle);
  turn(angle);//we make the correction given by our function
}
