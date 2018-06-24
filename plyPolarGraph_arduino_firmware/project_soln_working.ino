/*
   Drawbot position function

   Inverse Kinematics -
   Function for the angle (length) of members to determine the position of an effector
   A drawbot (a.k.a. polargraph) is robot that uses a pen as an effector tethered by two stepper motors from two distant postions
   the drawbot traverses its working area by shortening and or lengthening the tethers of from the steppers.
   Video: https://www.youtube.com/watch?v=usqarqGnFog

*/

// TODO - Task 1: Write a function that changes the length cable from a stepper motor. ie void changeLength(int millimeters)
// TODO - Task 2: write a function that takes a boolean and writes two different positions (actual values to be determined later) for a servo. ie void marker(boolean draw)
// TODO - Task 3: write a function that takes an (x,y) co-ordinate where (0,0) is the top left corner of the drawing canvas and converts to an equivalent (L1, L2) where L1 is the length of tether 1 and L2 is the length of teather 2
// TODO - Task 4: overload the function from task 2 to take a second boolean parameter that changes the length of the stepper by an absolute or relative ammount.
//        def: relative the length of the tether changes BY the value of the argument passed.
//        def: absolute the length of the tether changes TO the value of the argument passed.
//        e.g.: void changeLength(int millimeters, boolean absoulte)
// TODO - next week!

#include<Servo.h>
#include "A4988.h"

// Program configuration
#define DEBUG 1
#define WINCH_SPACING 1000
#define WINCH_CIRCUMFRENCE 31.4

#define X_LIM_MIN 165
#define X_LIM_MAX 834
#define Y_LIM_MIN 223
#define Y_LIM_MAX 618
#define PEN_DOWN 180
#define PEN_UP 0

// Hardware configuration

#define SERVO_PIN 9
#define STEPPER_1_DIR_PIN 2
#define STEPPER_1_STEP_PIN 3
#define STEPPER_2_DIR_PIN 4
#define STEPPER_2_STEP_PIN 5
#define MOTOR_STEPS_PIN 200
#define STEPPER_RPM_PIN 15

// State variables
String input = "";
boolean stringComplete = false;

const String X_QUALIFIER = "X:";
const String Y_QUALIFIER = "Y:";
const String DRAWING_QUALIFIER = "D:

float xCoord = 0;
float yCoord = 0;
boolean draw = false;

// Co-ordinate system
float len1 = 0;
float len2 = 0;

// Actual physical 
float winchLen1 = 0;
float winchLen2 = 0;

// Hardware objects
Servo drawServo;
A4988 stepper1;
A4988 stepper2;

//Stepper Driving Contp;er
stepper1(MOTOR_STEPS, STEPPER_1_DIR_PIN, STEPPER_1_STEP_PIN); // MS1-3 define the step mode on the A4988. Once solution found, set in HWR.
stepper2(MOTOR_STEPS, STEPPER_2_DIR_PIN, STEPPER_2_STEP_PIN);

// synchronized move, trajectory is a straight line
SyncDriver controller(stepper1, stepper2);

// Methods
void parseString();
boolean translateCoordinatesABStoPolar();
void establishContact();
void draw();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  input.reserve(10);
  establishContact();

  // Initialise hardware
  drawServo.attach(DRAW_SERVO);

  stepper1.begin(STEPPER_RPM, 1); //RPM and stepping mode, 1 normal stepping mode

  stepper2.begin(STEPPER_RPM, 1)
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    // get incoming byte and cast to char
    char inChar = (char)Serial.read();
    input = input + inChar;

    if (inChar == ';') {
      stringComplete = true;
    }

    //Completed instruction
    if (stringComplete) {
      if (DEBUG) {
        Serial.println(input);
        Serial.println(stringComplete);
      }

      parseString();
      stringComplete = false;
      input = "";
      translateCoordinatesABStoPolar();
    }
  }
}

boolean translateCoordinatesABStoPolar() {
  float len1;
  float len2;

  //Exit condition
  if (xCoord < X_LIM_MIN || xCoord > X_LIM_MAX || yCoord < Y_LIM_MIN || yCoord > Y_LIM_MAX) {

  if (DEBUG) {
    Serial.println("Co-ordinates out of range");
  }
    return false;
  }

  len1 = sqrt(xCoord * xCoord + yCoord * yCoord); //^ 0.5
  len2 = sqrt((WINCH_SPACING - xCoord) * (WINCH_SPACING - xCoord) + yCoord * yCoord);// ^ 0.5

  if (DEBUG) {
    Serial.print(len1);
    Serial.print("\t");
    Serial.println(len2);
  }
  return true;
}

void parseString() {
  int xIndex = input.indexOf(X_QUALIFIER);
  int yIndex = input.indexOf(Y_QUALIFIER, xIndex);
  int drawIndex = input.indexOf(DRAW_QUALIFIER, yIndex);

  String xString = input.substring(xIndex + 2, yIndex);
  xString.trim();
  String yString = input.substring(yIndex + 2);
  yString.trim();

  String drawString = input.substring(drawIndex + 2);
  drawString.trim();

  xCoord = xString.toFloat();
  yCoord = yString.toFloat();
  draw = (boolean)drawString.toInt();


  if (DEBUG) {
    Serial.print(xCoord);
    Serial.print("\t");
    Serial.print(yCoord);
    Serial.print("\t");
    Serial.println(draw);
  }
}

void establishContact() {
  if (Serial.available() <= 0) {
    Serial.println("serial connection established");   // send an initial string
    delay(300);
  }
}

void startDraw(){

  if(draw){
    drawServo.write(PEN_DOWN);
  }else{
    drawServo.write(PEN_UP);
  }

}

void driveSteppersAbsolute(float length1, float length2){

  float deltaL1 = length1 - winchLen1;
  float deltaL2 = length2 - winchLen2;
  
  controller.move((deltaL1/WINCH_CIRCUMFRENCE * MOTOR_STEPS),(deltaL2/WINCH_CIRCUMFRENCE * MOTOR_STEPS));

  //Update state values to the new lengths
  winchLen1 = winchLen1-deltaL1;
  winchLen2 = winchLen2-deltaL2;

  if(DEBUG){
    Serial.println("Input Lengths");
    Serial.print(length1);
    Serial.print("\t");
    Serial.println(length2);

    Serial.println("change Lengths");
    Serial.print(deltaL1);
    Serial.print("\t");
    Serial.println(deltaL2);
    
    Serial.println("final lengths");
    Serial.print(winchLen1);
    Serial.print("\t");
    Serial.println(winchLen2);

  }
  }
  
  
}

void driveSteppersRelative(float length1, float length2){
  controller.move((length1/WINCH_CIRCUMFRENCE * MOTOR_STEPS),(length1/WINCH_CIRCUMFRENCE * MOTOR_STEPS));
}

