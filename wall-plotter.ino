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

char svgJson[] = "{\"lines\":[{\"points\":[{\"x\":\"61.98\",\"y\":\"12.67\"},{\"x\":\"55.13\",\"y\":\"14.15\"},{\"x\":\"50.50\",\"y\":\"5.52\"},{\"x\":\"58.11\",\"y\":\"0.75\"},{\"x\":\"68.68\",\"y\":\"-1.32\"},{\"x\":\"69.15\",\"y\":\"11.11\"},{\"x\":\"61.98\",\"y\":\"12.67\"}]},{\"points\":[{\"x\":\"40.00\",\"y\":\"21.00\"},{\"x\":\"39.98\",\"y\":\"23.19\"},{\"x\":\"40.10\",\"y\":\"25.92\"},{\"x\":\"38.98\",\"y\":\"27.87\"},{\"x\":\"37.58\",\"y\":\"30.29\"},{\"x\":\"34.77\",\"y\":\"31.63\"},{\"x\":\"32.02\",\"y\":\"30.92\"},{\"x\":\"29.91\",\"y\":\"30.39\"},{\"x\":\"28.39\",\"y\":\"28.94\"},{\"x\":\"27.67\",\"y\":\"26.95\"},{\"x\":\"26.61\",\"y\":\"24.46\"},{\"x\":\"26.70\",\"y\":\"11.53\"},{\"x\":\"27.67\",\"y\":\"9.06\"},{\"x\":\"28.56\",\"y\":\"7.10\"},{\"x\":\"28.86\",\"y\":\"7.28\"},{\"x\":\"30.23\",\"y\":\"6.02\"},{\"x\":\"41.55\",\"y\":\"2.04\"},{\"x\":\"40.05\",\"y\":\"14.00\"},{\"x\":\"40.00\",\"y\":\"21.00\"}]},{\"points\":[{\"x\":\"121.00\",\"y\":\"21.00\"},{\"x\":\"121.00\",\"y\":\"21.00\"},{\"x\":\"121.00\",\"y\":\"72.00\"},{\"x\":\"121.00\",\"y\":\"72.00\"},{\"x\":\"121.01\",\"y\":\"75.39\"},{\"x\":\"120.74\",\"y\":\"78.99\"},{\"x\":\"122.74\",\"y\":\"81.94\"},{\"x\":\"126.17\",\"y\":\"87.01\"},{\"x\":\"131.02\",\"y\":\"86.22\"},{\"x\":\"133.69\",\"y\":\"80.98\"},{\"x\":\"135.18\",\"y\":\"78.05\"},{\"x\":\"134.99\",\"y\":\"74.22\"},{\"x\":\"135.00\",\"y\":\"71.00\"},{\"x\":\"135.01\",\"y\":\"67.21\"},{\"x\":\"134.36\",\"y\":\"56.18\"},{\"x\":\"136.17\",\"y\":\"53.39\"},{\"x\":\"138.89\",\"y\":\"49.22\"},{\"x\":\"144.50\",\"y\":\"49.98\"},{\"x\":\"146.83\",\"y\":\"54.11\"},{\"x\":\"148.21\",\"y\":\"56.56\"},{\"x\":\"148.00\",\"y\":\"60.23\"},{\"x\":\"148.00\",\"y\":\"63.00\"},{\"x\":\"148.03\",\"y\":\"79.26\"},{\"x\":\"147.75\",\"y\":\"89.58\"},{\"x\":\"141.13\",\"y\":\"105.00\"},{\"x\":\"129.15\",\"y\":\"132.88\"},{\"x\":\"101.83\",\"y\":\"149.35\"},{\"x\":\"72.00\",\"y\":\"149.00\"},{\"x\":\"41.47\",\"y\":\"148.64\"},{\"x\":\"14.58\",\"y\":\"126.29\"},{\"x\":\"5.00\",\"y\":\"98.00\"},{\"x\":\"0.64\",\"y\":\"85.11\"},{\"x\":\"0.76\",\"y\":\"77.32\"},{\"x\":\"1.00\",\"y\":\"64.00\"},{\"x\":\"1.09\",\"y\":\"59.09\"},{\"x\":\"2.77\",\"y\":\"54.08\"},{\"x\":\"8.96\",\"y\":\"55.47\"},{\"x\":\"15.92\",\"y\":\"57.04\"},{\"x\":\"13.99\",\"y\":\"69.47\"},{\"x\":\"14.00\",\"y\":\"75.00\"},{\"x\":\"14.01\",\"y\":\"77.94\"},{\"x\":\"13.79\",\"y\":\"81.26\"},{\"x\":\"15.31\",\"y\":\"83.92\"},{\"x\":\"17.98\",\"y\":\"88.51\"},{\"x\":\"23.34\",\"y\":\"88.76\"},{\"x\":\"25.98\",\"y\":\"83.92\"},{\"x\":\"27.19\",\"y\":\"81.73\"},{\"x\":\"27.00\",\"y\":\"78.47\"},{\"x\":\"27.00\",\"y\":\"76.00\"},{\"x\":\"27.00\",\"y\":\"76.00\"},{\"x\":\"27.00\",\"y\":\"54.00\"},{\"x\":\"27.00\",\"y\":\"54.00\"},{\"x\":\"27.04\",\"y\":\"50.86\"},{\"x\":\"27.18\",\"y\":\"46.93\"},{\"x\":\"29.09\",\"y\":\"44.28\"},{\"x\":\"31.93\",\"y\":\"40.33\"},{\"x\":\"37.49\",\"y\":\"41.08\"},{\"x\":\"39.83\",\"y\":\"45.13\"},{\"x\":\"41.21\",\"y\":\"47.53\"},{\"x\":\"41.00\",\"y\":\"51.28\"},{\"x\":\"41.00\",\"y\":\"54.00\"},{\"x\":\"41.00\",\"y\":\"54.00\"},{\"x\":\"41.00\",\"y\":\"113.00\"},{\"x\":\"41.00\",\"y\":\"113.00\"},{\"x\":\"41.06\",\"y\":\"117.46\"},{\"x\":\"42.57\",\"y\":\"125.24\"},{\"x\":\"48.98\",\"y\":\"123.34\"},{\"x\":\"54.14\",\"y\":\"121.82\"},{\"x\":\"53.99\",\"y\":\"116.31\"},{\"x\":\"54.00\",\"y\":\"112.00\"},{\"x\":\"54.00\",\"y\":\"112.00\"},{\"x\":\"54.00\",\"y\":\"77.00\"},{\"x\":\"54.00\",\"y\":\"77.00\"},{\"x\":\"54.01\",\"y\":\"72.45\"},{\"x\":\"53.74\",\"y\":\"66.46\"},{\"x\":\"59.04\",\"y\":\"64.82\"},{\"x\":\"60.64\",\"y\":\"64.06\"},{\"x\":\"62.41\",\"y\":\"64.22\"},{\"x\":\"63.89\",\"y\":\"64.82\"},{\"x\":\"67.73\",\"y\":\"67.11\"},{\"x\":\"67.94\",\"y\":\"71.16\"},{\"x\":\"68.00\",\"y\":\"75.00\"},{\"x\":\"68.00\",\"y\":\"75.00\"},{\"x\":\"68.00\",\"y\":\"125.00\"},{\"x\":\"68.00\",\"y\":\"125.00\"},{\"x\":\"68.00\",\"y\":\"127.67\"},{\"x\":\"67.78\",\"y\":\"131.45\"},{\"x\":\"69.17\",\"y\":\"133.79\"},{\"x\":\"72.04\",\"y\":\"138.58\"},{\"x\":\"77.40\",\"y\":\"137.48\"},{\"x\":\"79.83\",\"y\":\"132.96\"},{\"x\":\"81.21\",\"y\":\"130.38\"},{\"x\":\"81.00\",\"y\":\"126.86\"},{\"x\":\"81.00\",\"y\":\"124.00\"},{\"x\":\"81.00\",\"y\":\"124.00\"},{\"x\":\"81.00\",\"y\":\"49.00\"},{\"x\":\"81.00\",\"y\":\"49.00\"},{\"x\":\"81.00\",\"y\":\"46.01\"},{\"x\":\"80.69\",\"y\":\"41.84\"},{\"x\":\"82.74\",\"y\":\"39.43\"},{\"x\":\"85.31\",\"y\":\"36.41\"},{\"x\":\"93.91\",\"y\":\"34.20\"},{\"x\":\"94.00\",\"y\":\"47.00\"},{\"x\":\"94.00\",\"y\":\"47.00\"},{\"x\":\"94.00\",\"y\":\"109.00\"},{\"x\":\"94.00\",\"y\":\"109.00\"},{\"x\":\"94.00\",\"y\":\"111.80\"},{\"x\":\"93.87\",\"y\":\"115.38\"},{\"x\":\"94.89\",\"y\":\"118.00\"},{\"x\":\"96.84\",\"y\":\"123.01\"},{\"x\":\"102.22\",\"y\":\"125.95\"},{\"x\":\"105.99\",\"y\":\"120.77\"},{\"x\":\"108.23\",\"y\":\"117.71\"},{\"x\":\"107.99\",\"y\":\"113.60\"},{\"x\":\"108.00\",\"y\":\"110.00\"},{\"x\":\"108.00\",\"y\":\"110.00\"},{\"x\":\"108.00\",\"y\":\"39.00\"},{\"x\":\"108.00\",\"y\":\"39.00\"},{\"x\":\"108.00\",\"y\":\"35.05\"},{\"x\":\"107.56\",\"y\":\"18.63\"},{\"x\":\"108.60\",\"y\":\"16.06\"},{\"x\":\"109.40\",\"y\":\"14.06\"},{\"x\":\"109.76\",\"y\":\"14.25\"},{\"x\":\"111.15\",\"y\":\"13.02\"},{\"x\":\"118.32\",\"y\":\"10.57\"},{\"x\":\"120.87\",\"y\":\"14.20\"},{\"x\":\"121.00\",\"y\":\"21.00\"}]},{\"points\":[{\"x\":\"67.00\",\"y\":\"43.00\"},{\"x\":\"66.99\",\"y\":\"45.05\"},{\"x\":\"67.07\",\"y\":\"48.01\"},{\"x\":\"66.26\",\"y\":\"49.89\"},{\"x\":\"65.02\",\"y\":\"52.79\"},{\"x\":\"62.11\",\"y\":\"54.22\"},{\"x\":\"59.04\",\"y\":\"53.53\"},{\"x\":\"54.25\",\"y\":\"52.44\"},{\"x\":\"54.06\",\"y\":\"48.01\"},{\"x\":\"54.00\",\"y\":\"44.00\"},{\"x\":\"53.91\",\"y\":\"37.52\"},{\"x\":\"51.95\",\"y\":\"27.83\"},{\"x\":\"58.11\",\"y\":\"23.87\"},{\"x\":\"69.60\",\"y\":\"21.23\"},{\"x\":\"67.04\",\"y\":\"35.93\"},{\"x\":\"67.00\",\"y\":\"43.00\"}]}]}";


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


