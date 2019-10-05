#include <Servo.h>
#include <ArduinoJson.h>
#include <StepperMotor.h>
#include <ESP8266WebServer.h>


#define SERVO_PIN D9
#define WIFI_INIT_RETRY 10
#define STEPS_PER_TICK 10
#define PEN_UP 70
#define PEN_DOWN 25

const char* ssid  = "SSID";
const char* password = "PASSWORD";
StepperMotor motorL(D1,D2,D3,D4); // IN1, IN2, IN3, IN4
StepperMotor motorR(D5, D6, D7, D8);
ESP8266WebServer server(80);

Servo servoPen;
StaticJsonDocument<10000> doc;
int motorSpeed = 3;
bool printing = true;

char svgJson[] = "{\"lines\":[{\"points\":[{\"x\":\"20.0\",\"y\":\"30.0\"},{\"x\":\"8.31\",\"y\":\"-22.00\"},{\"x\":\"-3.22\",\"y\":\"0.00\"},{\"x\":\"-6.76\",\"y\":\"17.97\"},{\"x\":\"-6.80\",\"y\":\"-17.97\"},{\"x\":\"-3.22\",\"y\":\"0.00\"},{\"x\":\"8.33\",\"y\":\"22.00\"},{\"x\":\"3.36\",\"y\":\"0.00\"}]},{\"points\":[{\"x\":\"15.86\",\"y\":\"0.00\"},{\"x\":\"3.75\",\"y\":\"0.00\"},{\"x\":\"4.72\",\"y\":\"-19.09\"},{\"x\":\"4.67\",\"y\":\"19.09\"},{\"x\":\"3.75\",\"y\":\"0.00\"},{\"x\":\"5.56\",\"y\":\"-22.00\"},{\"x\":\"-3.09\",\"y\":\"0.00\"},{\"x\":\"-4.86\",\"y\":\"19.19\"},{\"x\":\"-4.69\",\"y\":\"-19.19\"},{\"x\":\"-2.69\",\"y\":\"0.00\"},{\"x\":\"-4.73\",\"y\":\"19.19\"},{\"x\":\"-4.85\",\"y\":\"-19.19\"},{\"x\":\"-3.09\",\"y\":\"0.00\"},{\"x\":\"5.55\",\"y\":\"22.00\"}]},{\"points\":[{\"x\":\"36.20\",\"y\":\"-21.75\"},{\"x\":\"-1.06\",\"y\":\"-0.16\"},{\"x\":\"-2.05\",\"y\":\"-0.09\"},{\"x\":\"-4.26\",\"y\":\"0.55\"},{\"x\":\"-1.35\",\"y\":\"1.11\"},{\"x\":\"-1.35\",\"y\":\"1.09\"},{\"x\":\"-0.68\",\"y\":\"1.51\"},{\"x\":\"0.00\",\"y\":\"1.91\"},{\"x\":\"0.00\",\"y\":\"1.65\"},{\"x\":\"0.48\",\"y\":\"1.29\"},{\"x\":\"0.97\",\"y\":\"0.92\"},{\"x\":\"0.97\",\"y\":\"0.93\"},{\"x\":\"1.57\",\"y\":\"0.68\"},{\"x\":\"2.18\",\"y\":\"0.44\"},{\"x\":\"0.00\",\"y\":\"0.00\"},{\"x\":\"1.75\",\"y\":\"0.34\"},{\"x\":\"0.00\",\"y\":\"0.00\"},{\"x\":\"1.45\",\"y\":\"0.26\"},{\"x\":\"1.04\",\"y\":\"0.39\"},{\"x\":\"0.64\",\"y\":\"0.52\"},{\"x\":\"0.63\",\"y\":\"0.52\"},{\"x\":\"0.32\",\"y\":\"0.71\"},{\"x\":\"0.00\",\"y\":\"0.91\"},{\"x\":\"0.00\",\"y\":\"1.05\"},{\"x\":\"-0.45\",\"y\":\"0.81\"},{\"x\":\"-0.91\",\"y\":\"0.57\"},{\"x\":\"-0.90\",\"y\":\"0.56\"},{\"x\":\"-1.27\",\"y\":\"0.28\"},{\"x\":\"-1.66\",\"y\":\"0.00\"},{\"x\":\"-1.09\",\"y\":\"0.00\"},{\"x\":\"-1.12\",\"y\":\"-0.08\"},{\"x\":\"-1.13\",\"y\":\"-0.17\"},{\"x\":\"-1.14\",\"y\":\"-0.16\"},{\"x\":\"-1.15\",\"y\":\"-0.25\"},{\"x\":\"-1.18\",\"y\":\"-0.34\"},{\"x\":\"0.00\",\"y\":\"0.00\"},{\"x\":\"0.00\",\"y\":\"3.00\"},{\"x\":\"0.00\",\"y\":\"0.00\"},{\"x\":\"1.23\",\"y\":\"0.34\"},{\"x\":\"1.18\",\"y\":\"0.25\"},{\"x\":\"1.14\",\"y\":\"0.16\"},{\"x\":\"1.14\",\"y\":\"0.17\"},{\"x\":\"1.08\",\"y\":\"0.08\"},{\"x\":\"1.03\",\"y\":\"0.00\"},{\"x\":\"2.68\",\"y\":\"0.00\"},{\"x\":\"2.04\",\"y\":\"-0.52\"},{\"x\":\"1.39\",\"y\":\"-1.04\"},{\"x\":\"1.39\",\"y\":\"-1.04\"},{\"x\":\"0.70\",\"y\":\"-1.54\"},{\"x\":\"0.00\",\"y\":\"-2.03\"},{\"x\":\"0.01\",\"y\":\"-1.74\"},{\"x\":\"-0.51\",\"y\":\"-1.38\"},{\"x\":\"-1.02\",\"y\":\"-1.02\"},{\"x\":\"-1.01\",\"y\":\"-1.03\"},{\"x\":\"-1.56\",\"y\":\"-0.72\"},{\"x\":\"-2.13\",\"y\":\"-0.41\"},{\"x\":\"0.00\",\"y\":\"0.00\"},{\"x\":\"-1.73\",\"y\":\"-0.34\"},{\"x\":\"0.00\",\"y\":\"0.00\"},{\"x\":\"-1.56\",\"y\":\"-0.29\"},{\"x\":\"-1.06\",\"y\":\"-0.36\"},{\"x\":\"-0.57\",\"y\":\"-0.43\"},{\"x\":\"-0.56\",\"y\":\"-0.43\"},{\"x\":\"-0.29\",\"y\":\"-0.62\"},{\"x\":\"-0.01\",\"y\":\"-0.81\"},{\"x\":\"0.00\",\"y\":\"-1.04\"},{\"x\":\"0.43\",\"y\":\"-0.80\"},{\"x\":\"0.87\",\"y\":\"-0.57\"},{\"x\":\"0.86\",\"y\":\"-0.56\"},{\"x\":\"1.23\",\"y\":\"-0.28\"},{\"x\":\"1.59\",\"y\":\"0.00\"},{\"x\":\"0.93\",\"y\":\"0.00\"},{\"x\":\"0.95\",\"y\":\"0.09\"},{\"x\":\"0.98\",\"y\":\"0.16\"},{\"x\":\"0.99\",\"y\":\"0.17\"},{\"x\":\"1.05\",\"y\":\"0.25\"},{\"x\":\"1.12\",\"y\":\"0.33\"},{\"x\":\"0.00\",\"y\":\"0.00\"},{\"x\":\"0.00\",\"y\":\"-3.00\"},{\"x\":\"0.00\",\"y\":\"0.00\"},{\"x\":\"-1.11\",\"y\":\"-0.33\"},{\"x\":\"-1.08\",\"y\":\"-0.25\"},{\"x\":\"-1.06\",\"y\":\"-0.17\"}]}]}";

long canvasWidth = 1000;
long currentLeft = 1000; 
long currentRight = 1000;
float centerX = 500;
float centerY = 866; //the height in the triangle
float zoom = 10.0;
float lastX = 0;
float lastY = 0;

int initWifi() {
    int retries = 0;
    Serial.println("Connecting...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while ((WiFi.status() != WL_CONNECTED) && (retries < WIFI_INIT_RETRY)) {
        retries++;
        delay(1000);
        Serial.print("#");
    }

    return WiFi.status();
}

void setup()
{
  Serial.begin(9600); 
  Serial.println("Setup");
  Serial.print("Canvas width:");
  Serial.println(canvasWidth);
  if (DeserializationError error = deserializeJson(doc, svgJson)) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  Serial.println(initWifi());

  server.begin();
  Serial.println(WiFi.localIP());
  serverRouting();

  motorL.setStepDuration(motorSpeed);
  motorR.setStepDuration(motorSpeed);

  servoPen.attach(SERVO_PIN);
  servoPen.write(PEN_UP);
  delay(5000);
}

bool getPoint(int line, int point, float *x, float* y)
{
  float newX = doc["lines"][line]["points"][point]["x"];
  float newY = doc["lines"][line]["points"][point]["y"];
  if (newX == 0 && newY == 0) {
    
    return false;
  }
  *x = newX;
  *y = newY;
    
  return true; 
}

void drawLine(long distanceR, long distanceL){
  Serial.println("NEWLine");
  int lDirection = 1;
  int rDirection = -1;
  if (distanceL < 0) {
    lDirection = lDirection * -1;
  }
  if (distanceR < 0) {
    rDirection = rDirection * -1;
  }

  long distL = distanceL;
  long distR = distanceR;
  if (distanceL < 0) {
    distL = distL * -1;
  }
  if (distanceR < 0) {
    distR = distR * -1;
  }
  long ticks = distL * distR;
  for (long i = 0; i < ticks; i++) {
    if (i % distR == 0) {
       motorL.step(STEPS_PER_TICK * lDirection);
    }
    if (i % distL == 0) {
       motorR.step(STEPS_PER_TICK * rDirection);
    }
  }
}

void postPlot() {
    String body = server.arg("plain");
    if (DeserializationError error = deserializeJson(doc, body)) {
        Serial.println("error parsing json body");
        server.send(400);
        return;
    } else {
        if (server.method() == HTTP_POST) {
            printing = true;
            server.sendHeader("Location", "/plot/");
            server.send(201);
        }
    }
}

void serverRouting() {
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html",
            "/plot");
    });
    server.on("/plot", HTTP_POST, postPlot);
}

void loop() {
    server.handleClient();
    if (printing) {
      float tmpX, tmpY = 0;
      for (int line = 0; line < doc["lines"].size(); line++) {
        for (int point = 0; point < doc["lines"][line]["points"].size(); point++) {
          if(!getPoint(line, point, &tmpX, &tmpY)) {
            servoPen.write(PEN_UP);
            Serial.println("Plot done");
            printing = false;
          } else {
              if (point == 0) {
                servoPen.write(PEN_UP);
              } else {
                servoPen.write(PEN_DOWN);
              }
              float nextX = tmpX * zoom + lastX;
              float nextY = tmpY * zoom + lastY;
              float xPosL = nextX + centerX;
              float xPosR = nextX + centerX - canvasWidth;
              float yPos  = nextY + centerY;
              long newLeft  = sqrt(xPosL * xPosL + yPos * yPos);
              long newRight = sqrt(xPosR * xPosR + yPos * yPos);
              long distanceLeft  = (newLeft  - currentLeft);
              long distanceRight = (newRight - currentRight);
              currentLeft = newLeft;
              currentRight = newRight;
              lastX = nextX;
              lastY = nextY;
              drawLine(distanceLeft, distanceRight);
           }
        }
      }
    }
}


