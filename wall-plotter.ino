#include <ArduinoJson.h>
#include "FS.h"
#include <Servo.h>
#include <StepperMotor.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266WiFi.h>

#include "Config.h"

ESP8266WebServer server(80);
IPAddress accessPointIP(192, 168, 0, 1);
IPAddress netMask(255, 255, 255, 0);
DNSServer dnsServer;

StepperMotor motorLeft(MOTOR_LEFT_1, MOTOR_LEFT_2, MOTOR_LEFT_3, MOTOR_LEFT_4);
StepperMotor motorRight(MOTOR_RIGHT_1, MOTOR_RIGHT_2, MOTOR_RIGHT_3, MOTOR_RIGHT_4);

const int motorLeftDirection = -1;
const int motorRightDirection = 1;
const int motorSpeed = 2;
Servo servoPen;
bool printing = true;

StaticJsonDocument<1000> configJson;
const char* ssid  = "SSID";
const char* password = "PASSWORD";
long canvasWidth = 1000;
long currentLeft = canvasWidth;
long currentRight = canvasWidth;
float centerX = canvasWidth / 2;
float centerY = sqrt(pow(canvasWidth, 2) - pow(centerX, 2));
float zoomFactor = 1;

static float lastX = 0;
static float lastY = 0;
static float homeX = 0;
static float homeY = 0;
char configData[] = "{\"server\":{\"ssid\":\"ssid\",\"password\":\"password\"},\"plotter\":{\"canvasWidth\":\"canvasWidth\",\"currentLeft\":\"currentLeft\",\"currentRight\":\"currentRight\",\"zoomFactor\":\"zoomFactor\"}}";


void writeConfig();

void setup() {
    Serial.begin(9600);
    Serial.println("Setup");
    initConfig();
    initFileSystem();
    Serial.print("Canvas width:");
    Serial.println(canvasWidth);
    initMotors();
    initServer();
    server.begin();
    serverRouting();
    Serial.println("Ready!");
}

void loop() {
    server.handleClient();
}

void drawLine(long distanceLeft, long distanceRight){
    int directionLeft = motorLeftDirection;
    int directionRight = motorRightDirection;
    int tmpDirection = 0;
    long distL = distanceLeft;
    long distR = distanceRight;
    if (distanceLeft < 0) {
        directionLeft = directionLeft * -1;
        distL = distL * -1;
    }
    if (distanceRight < 0) {
        directionRight = directionRight * -1;
        distR = distR * -1;
    }
    long ticks = distL * distR;
    if (distL == 0) {
        ticks = distR * distR;
        distL = distR;
        distR = 0;
    } else if (distR == 0) {
        ticks = distL * distL;
        distR = distL;
        distL = 0;
    }
    Serial.print(distanceLeft);
    Serial.print(" dist ");
    Serial.println(distanceRight);
    int countLeft = 0;
    int countRight = 0;
    for (long i = 0; i < ticks; i++) {
        if (distL != 0 && i % distL == 0) {
            countLeft = countLeft + (1 * directionRight);
            motorLeft.step(STEPS_PER_TICK * directionRight);
        }
        if (distR != 0 && i % distR == 0) {
            countRight = countRight + (1 * directionLeft);
            motorRight.step(STEPS_PER_TICK * directionLeft);
        }
        yield();
    }
    Serial.print(countRight * motorLeftDirection);
    Serial.print(" count ");
    Serial.println(countLeft * motorRightDirection);
    
}

void getDistance(float x, float y, long *distanceLeft, long *distanceRight) {
    float nextX = x + lastX;
    float nextY = y + lastY;
    float leftX = nextX + centerX;
    float rightX = nextX - centerX;
    float yPos  = nextY + centerY;
    long newLeft  = sqrt(pow(leftX, 2) + pow(yPos, 2));
    long newRight = sqrt(pow(rightX, 2) + pow(yPos, 2));
    *distanceLeft  = (newLeft - currentLeft) * zoomFactor;
    *distanceRight = (newRight - currentRight) * zoomFactor;
    currentLeft = newLeft;
    currentRight = newRight;
    lastX = nextX;
    lastY = nextY;
}

void goHome() {
    long distanceLeft = 0;
    long distanceRight = 0;
    homeX = homeX * -1;
    homeY = homeY * -1;
    getDistance(homeX,homeY, &distanceLeft, &distanceRight);
    drawLine(distanceLeft, distanceRight);
    homeX = 0;
    homeY = 0;
}

void postPlotStart() {
    printing = true;
    if (SPIFFS.exists(UPLOAD_PLOT_FILENAME)) {
        server.send(200);
        char newPlotData[2000];
        File f = SPIFFS.open(UPLOAD_PLOT_FILENAME, "r");
        int point = 0;
        float x = 0;
        float y = 0;
        while(f.available()) {
            if (!printing) {
                Serial.println("Plot stopped.");
                break;
            }
            f.readStringUntil('\n').toCharArray(newPlotData, 2000);
            char * pch;
            pch = strtok (newPlotData,",");
            int counter = 0;
            if (String(pch).indexOf("m") != -1) {
                servoPen.write(PEN_UP);
                Serial.println("PEN_UP:");
                point = 0;
            }
            while (pch != NULL && point > 0) {
                if (!printing) {
                    servoPen.write(PEN_UP);
                    break;
                }
                if (counter % 2 == 0) {
                    if (point < 3) {
                        servoPen.write(PEN_UP);
                    } else {
                        servoPen.write(PEN_DOWN);
                    }
                    yield();
                    long distanceLeft = 0;
                    long distanceRight = 0;
                    homeX = homeX + x;
                    homeY = homeY + y;
                    getDistance(x,y, &distanceLeft, &distanceRight);
                    drawLine(distanceLeft, distanceRight);
                    server.handleClient();
                    //printf ("X: %s\n",pch);
                    x = atof(pch);
                } else {
                    //printf ("Y: %s\n",pch);
                    y = atof(pch);
                }
                pch = strtok (NULL, ",");
                counter++;
            }
            point++;
        }
        f.close();
        servoPen.write(PEN_UP);
        Serial.println("Plot done.");
        printing = false;
        goHome();
    } else {
       server.send(404, "text/plain", "NotFound");
    }
}
