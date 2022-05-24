// Class Challenge
// by Caterev Robert, Ma Coty, Raharison Toky
// 01-04-2022


#define M_PI 3.141592653589793238462643383279
#include <Servo.h>
#include <math.h>

Servo leftservo;  
Servo rightservo;  
const int pingPin = 5; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 6; // Echo Pin of Ultrasonic Sensor

const int target_d_1 = 50; // Target distance from the first wall.
const int target_d_2 = 20;  // Target distance from the second wall.
const int target_d_3 = 20; // Target distance from the third wall.
int target_d; // Target distance that the car currently tries to achieve.
int lower_bound_d; // Distance at which the car needs readjusting.
int higher_bound_d; // Distance at which the car assumes there is no wall.

float angular_orientation_radians = 0; // Angular orientation of the car in radians. 0 = pointing straight to the right
float total_angle = 0; // Total angle at which the car has turned.

bool turned_Wall = false; // Variable used to indicate that the robot must perform the turn when a gap is detected in the wall
bool turn_done_1 = false;  // Variable used to indicate whether the first turn was completed
bool turn_done_2 = false; // Variable used to indicate whether the second turn was completed
bool angular_correction = false; // Variable used to shift the angular position in radians of the car by 90 degrees to account for the turns it makes.

float u_s_distance; // Distance detected by the ultrasonic sensor
float distance; // Calculated shortest distance from the wall


float angle(float position) {
  /*
   Function that takes the position from the wall as
   an argument and returns an angle of a magnitude based on the derivative of
   a gaussian function that depends on the distance of the car from the target distance.
   The further the car is, the bigger the magnitude of the angle will be.
   */
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


}


void turn(float angle) {

  /*
   * Turn function that takes the angle that the car should turn as an input, 
   * and turns the car accordingly, accounting for the specific angular velocity
   * of the car.
   */

  float angular_period = 8.772; //Experimentally determined angular period.
  float angular_velocity_per_second = 2*M_PI/ angular_period; // Angular velocity of the car, calculated from the experimental period.
  float rad_angle = (angle/360) *(2*M_PI);
  
  //Serial.print("Rad_angle: ");
  //Serial.println(rad_angle);

  float angular_time = rad_angle / angular_velocity_per_second;

  if (angular_time > 0) {  
    /*
     * If angular_time >0, it means that the inputted angle was counterclockwise, 
     * therefore the turn will be counterclockwise.
     */

    //Serial.println("angular time is positive");

    leftservo.write(30); 
    rightservo.write(30);
    delay(angular_time*1000);
    // Reduced servo speed to account for slipping due to momentum at maximum speed. 

    //Serial.print("delay left = ");
    //Serial.println(angular_time*1000);

    leftservo.write(150);
    rightservo.write(150);
    delay(10);
    // Opposite direction of servo to nullify angular momentum of the car.
    
    //Serial.println("I finished turning left");
    angular_orientation_radians += rad_angle;
  }

  else if (angular_time < 0) {
    //Serial.println("angular time is negative");
    //Serial.println(angular_time,4);

    leftservo.write(150);
    rightservo.write(150);
    delay(abs(angular_time*1000));
    // Reduced servo speed for the same reason.

    //Serial.print("delay right = ");
    //Serial.println(abs(angular_time*1000));

    leftservo.write(30);
    rightservo.write(30);
    delay(10);
    // Opposite direction of servo to nullify angular momentum of the car.

    //Serial.println("I finished turning right");
  
    angular_orientation_radians += rad_angle;
  }

}


//Function for determining constants depending on whether car has turned to second wall or not
void distances_assignment(bool turned1, bool turned2) {
   if (turned1 == false) { // If the car is trying to collect coins located parallel to the first wall
     target_d = target_d_1;
     lower_bound_d = (target_d_1 - 10);
     higher_bound_d = (target_d_1 + 40);
  }
  else if ((turned1 == true) && (turned2 == false)) { // If the car is trying to collect coins parallel to the second wall
    target_d = target_d_2;
    lower_bound_d = (target_d_2 - 10);
    higher_bound_d = (target_d_2 + 40);
  }
  else if ((turned1 == true) && (turned2 == true)) {
    target_d = target_d_3;
    lower_bound_d = (target_d_3 - 10);
    higher_bound_d = (target_d_3 + 40);
  }
}


/* 
 *  Function to determine the true distance from the
 *  wall, whether the car is at angle to the wall or
 *  parallel to it. If it is at an angle, the distance
 *  recorded by the sensor is the hypothenuse of the
 *  triangle, and the true distance is the adjacent
 *  side of the triangle. 
*/
void true_distance(float angular_orientation) {
 if (angular_orientation == 0) {
    distance = u_s_distance;
  }
 /*
  * Since the angular orientation
  * is known, trigonometry can be applied to show that
  * the angular orientation of the car (-15 degrees to account for
  * the fact that the ultrasonic sensor in fact transmits a
  * 30 degrees arc of sound waves, therefore -15 takes the middle
  * of the arc) is equal to the angle between the shortest distance to
  * the wall and the beam of the sensor. 
  */
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


  // Calling distances_assignment to assign the correct distances
  distances_assignment(turn_done_1, turn_done_2);

  if (turn_done_1 == true) {
    turned_Wall = false; //turned_Wall is reset to false such that the car can turn a second time
    if (angular_correction == true) {
      angular_orientation_radians += 1.570796327;
      angular_correction = false; //Reset to false to only happen once every turn
    }

  }

    if (turn_done_2 == true) { // Here turned_Wall is not reset, because only 2 turns have to be accomplished
    if (angular_correction == true) { //Once again, a singular angular correction is made.
      angular_orientation_radians += 1.570796327;
      angular_correction = false;
    }

  }
  
  // Calling the function that determines the true distance from the wall
  true_distance(angular_orientation_radians);
  
  Serial.print("Actual distance: ");
  Serial.println(distance);
  // Feedback for testing


  if ((distance > target_d) && (distance < higher_bound_d )) {
    //Behavior when the distance is above the target_d, but below the higher bound
    
    float turn_angle = -angle(distance); //Using angle function, we determine by which angle to turn towards the wall
    turn(turn_angle);
    
    rightservo.write(0);
    leftservo.write(180);
    delay(400);
    // After turning, the car advances forward for 400ms
    
    total_angle += turn_angle; //Overall orientation of the car is recorded in total_angle
    Serial.print("Total Angle Turned :");
    Serial.println(total_angle);

  }

  else if ((distance <= target_d) && (distance >= lower_bound_d)) {
    //Behavior when the car is located belwo the target_d but above the lower bound
    
    if (total_angle > 0) { //Condition that corrects the angle of the car only when it is going towards the wall.
   
      turn((-1 * total_angle));
      total_angle = 0;

    }
    
    rightservo.write(0);
    leftservo.write(180);
    delay(1000);
    
  }

  else if ((distance < lower_bound_d) && (distance > 0)) {
    //Behavior when the car is below the lower bound
    
    Serial.print("Correcting direction because we are below ");
    Serial.print(lower_bound_d);
    Serial.println(" distance from the wall");
    turn(-3 * (((-9 / (target_d*target_d))* pow(distance, 2))+10));
    /* 
     * -3 degrees times a concave parabola that has a y-intercept of 10, meaning
     * that at distance=0, the car will turn by -3 * 10 degrees. at distance = target_d,
     * the function is equal to 1, therefore if the sensor detects a distance = target_d,
     * the car will only turn by 3 degrees.
     */
    rightservo.write(0);
    leftservo.write(180);
    delay(500);
  }

  else if ((higher_bound_d <= distance) || (distance == 0)) { //When the distance is too big, sometimes it detects a value of 0, thus why there is an or operator.
    turned_Wall = true; //Initiating turning if a distance higher than the high bound is detected
  }

  if ((turned_Wall == true) && (turn_done_1 == false)) { //First turn if statement
    
    rightservo.write(0);
    leftservo.write(180);
    delay(700);
    turn(-90);
    turn_done_1 = true;
    angular_correction = true;
    Serial.println("TURN 1 DONE");
  }
  else if ((turned_Wall == true) && (turn_done_2 == false)) { // Second turn if statement
    turn(-90);
    rightservo.write(0);
    leftservo.write(180);
    delay(700);
    turn_done_2 = true;
    angular_correction = true;
    Serial.println("TURN 2 DONE");
  }

  Serial.print("Target distance: ");
  Serial.println(target_d);

  rightservo.write(90);
  leftservo.write(90);



}
