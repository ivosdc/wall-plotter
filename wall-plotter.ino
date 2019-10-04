#include <Servo.h>
#include <ArduinoJson.h>
#include <StepperMotor.h>


#define SERVO_PIN D9
#define spoolCirc 94.2 
#define stepsPerRotation 4075.7728395
#define stepsPerMM (stepsPerRotation/spoolCirc) / 5

static int PEN_UP = 70;
static int PEN_DOWN = 20;


Servo servoPen;
StaticJsonDocument<10000> doc;
StepperMotor motorL(D1,D2,D3,D4); // IN1, IN2, IN3, IN4
StepperMotor motorR(D5, D6, D7, D8);
int motorSpeed = 3; 

char svgJson[] = "{\"lines\": [{\"points\": [{\"x\": 158.21149, \"y\":280.34488},{\"x\":3.94531,\"y\":0},{\"x\":0,\"y\":11.95312},{\"x\":14.33594,\"y\":0},{\"x\":0,\"y\":-11.95312},{\"x\":3.94531,\"y\":0},{\"x\":0,\"y\":29.16016},{\"x\":-3.94531,\"y\":0},{\"x\":0,\"y\":-13.88672},{\"x\":-14.33594,\"y\":0},{\"x\":0,\"y\":13.88672},{\"x\":-3.94531,\"y\":0},{\"x\":0,\"y\":-29.16016}]}, {\"points\": [{\"x\":212.74274, \"y\":279.11441},{\"x\":3.59375,\"y\":0},{\"x\":0,\"y\":30.39063},{\"x\":-3.59375,\"y\":0},{\"x\":0,\"y\":-30.39063},{\"x\":3.94531,\"y\":0},{\"x\":0,\"y\":29.16016},{\"x\":-3.94531,\"y\":0},{\"x\":0,\"y\":-13.88672},{\"x\":-14.33594,\"y\":0},{\"x\":0,\"y\":13.88672},{\"x\":-3.94531,\"y\":0},{\"x\":0,\"y\":-29.16016}]}]}";


long disparity = 1000;  //distance between anchor points 
long currentLeftSteps = 1000*stepsPerMM; 
long currentRightSteps = 1000*stepsPerMM;
float centerX = 500; //starting x pos
float centerY = 866; //starting x pos
float printSize = 1.0;
static float min_x = 100000000.0;
static float min_y = 100000000.0;
static float lastX,lastY;

void setup()
{
  Serial.begin(9600); 
  Serial.println("Setup");
  if (DeserializationError error = deserializeJson(doc, svgJson)) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  //initialize servo
  servoPen.attach(SERVO_PIN);
  servoPen.write(PEN_UP);
  delay(1000);

  motorL.setStepDuration(motorSpeed);
  motorR.setStepDuration(motorSpeed);

  Serial.print("Disparity=");
  Serial.println(disparity);
}


/*
void setOrigo() {
    float currentLeft  = currentLeftSteps / stepsPerMM;
    float currentRight = currentRightSteps / stepsPerMM;
    float tmp1 = (currentRight*currentRight-disparity*disparity-currentLeft*currentLeft);
    float tmp2 = (-2*currentLeft*disparity);
    float a = acos(tmp1/tmp2);    
    centerX = currentLeft*cos(a);
    centerY = currentLeft*sin(a);
}
*/

bool getNextXY(int line, int point, float *x, float *y)
{
  static float lastX, lastY;
  if(point == 0) {
    lastX = lastY = 0.0;
  }
  float newX = doc["lines"][line]["points"][point]["x"];
  float newY = doc["lines"][line]["points"][point]["y"];
  if (newX == 0 && newY == 0) {

    return false;
  }
  *x = newX;
  *y = newY;
  *x += lastX;
  *y += lastY;
  lastX = *x;
  lastY = *y;
    
  return true; 
}

bool getSvgData(int line, int point, float *x, float* y)
{
  if(point == 0) {
      float max_x, max_y;
  
      min_y = min_x = 100000000.0;
      max_y = max_x = -100000000.0;
  
      if(getNextXY(line, point, x, y)) {
          min_x = min(min_x, *x);
          max_x = max(max_x, *x);
          min_y = min(min_y, *y);
          max_y = max(max_y, *y);
      }
    //scaleFactor = (disparity*0.4) / (max_x-min_x); //fill 40% of disparity as default
  }
  
  if(getNextXY(line, point, x, y)) {
    *x = (*x - min_x);
    *y = (*y - min_y);
    lastX = *x;
    lastY = *y;
  }
  else {
    lastX = 0;
    lastY = 0;

    return false;
  }    

  return true;
}

void drawLine(long distanceL, long distanceR){
  Serial.println("NEWLine");
  StepperMotor leader = motorL;
  StepperMotor runner = motorR;
  int leaderDirection = 1;
  int runnerDirection = 1;
  if (distanceL < 0) {
    leaderDirection = -1;
  }
  if (distanceR < 0) {
    runnerDirection = -1;
  }

  long distL = distanceL;
  long distR = distanceR;
  if (distanceL < 0) {
    distL = distL * -1;
  }
  if (distanceR < 0) {
    distR = distR * -1;
  }
  long leaderSteps = distL;
  long runnerSteps = distR;
  if (distL < distR) {
    leaderDirection = leaderDirection * -1;
    runnerDirection = runnerDirection * -1;
    leaderSteps = distR;
    runnerSteps = distL;
    leader = motorR;
    runner = motorL;
  }
  long ticks = distL * distR;
  for (long i = 0; i < ticks; i++) {
    if (i % leaderSteps == 0) {
       leader.step(10 * leaderDirection);
    }
    if (i % runnerSteps == 0) {
       runner.step(-10 * runnerDirection);
    }
  }
}

void loop() {
  float tmpX, tmpY = 0;

  for (int line = 0; line < doc["lines"].size(); line++) {
    for (int point = 0; point < doc["lines"][line]["points"].size(); point++) {
      if(!getSvgData(line, point, &tmpX, &tmpY)) {
        servoPen.write(PEN_UP);
        Serial.println("Plot done");
        delay(500);
      } else {
        
  Serial.print(tmpX);
  Serial.print(" : ");
  Serial.println(tmpY);
  
          if (point == 0) {
            servoPen.write(PEN_UP);
            delay(500);
          } else {
            servoPen.write(PEN_DOWN);
          }

          float nextX = tmpX*printSize;
          float nextY = tmpY*printSize;
      
          float xL = nextX+centerX;
          float xR = nextX+centerX-disparity;
          float y = nextY+centerY;
      
          long newLeft  = sqrt(xL*xL + y*y)*stepsPerMM;
          long newRight = sqrt(xR*xR + y*y)*stepsPerMM;
          
          long distanceLeft  = (newLeft  - currentLeftSteps);            
          long distanceRight = (newRight - currentRightSteps);
                      
          currentLeftSteps = newLeft;
          currentRightSteps = newRight;
          drawLine(distanceLeft, distanceRight);
       }
    }
  }

}


