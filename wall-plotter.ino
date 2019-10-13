#include <Servo.h>
#include <ArduinoJson.h>
#include <StepperMotor.h>
#include <ESP8266WebServer.h>
#include "FS.h"


#define SERVO_PIN D9
#define WIFI_INIT_RETRY 10
#define PEN_UP 30
#define PEN_DOWN 70
#define SPOOL_CIRC 94.2 
#define STEPS_PER_ROTATION 4075.7728395
#define STEPS_PER_TICK 10
#define STEPS_PER_MM  (STEPS_PER_ROTATION / SPOOL_CIRC) / STEPS_PER_TICK

float zoomFactor = 1;

float getZoom(float zoomFactor) {
    return (zoomFactor / STEPS_PER_TICK) * STEPS_PER_MM;
}

float zoom = getZoom(zoomFactor);
StepperMotor motorLeft(D5, D6, D7, D8); // IN1, IN2, IN3, IN4
StepperMotor motorRight(D1,D2,D3,D4);
const int motorLeftDirection = -1;
const int motorRightDirection = 1;
int motorSpeed = 2;
Servo servoPen;

const char* ssid  = "SSID";
const char* password = "PASSWORD";
ESP8266WebServer server(80);

StaticJsonDocument<10000> plotJson;
StaticJsonDocument<1000> configJson;
bool printing = true;
long canvasWidth = 1000;
long currentLeft = canvasWidth;
long currentRight = canvasWidth;
float centerX = canvasWidth / 2;
float centerY = 866; //the height in the triangle
static float lastX = 0;
static float lastY = 0;
static float homeX = 0;
static float homeY = 0;

char configData[] = "{\"server\":{\"ssid\":\"ssid\",\"password\":\"password\"},\"plotter\":{\"canvasWidth\":\"canvasWidth\",\"currentLeft\":\"currentLeft\",\"currentRight\":\"currentRight\",\"centerX\":\"centerX\",\"centerY\":\"centerY\",\"zoomFactor\":\"zoomFactor\"}}";

char plotData[] = "{\"lines\":[{\"points\":[{\"x\":\"243.98\",\"y\":\"102.51\"},{\"x\":\"6.00\",\"y\":\"-2.00\"},{\"x\":\"5.80\",\"y\":\"-7.39\"},{\"x\":\"-4.52\",\"y\":\"-7.10\"},{\"x\":\"-8.77\",\"y\":\"-3.81\"},{\"x\":\"-26.60\",\"y\":\"-8.78\"},{\"x\":\"-3.10\",\"y\":\"-22.43\"},{\"x\":\"4.09\",\"y\":\"-10.17\"},{\"x\":\"16.57\",\"y\":\"-6.44\"},{\"x\":\"12.55\",\"y\":\"2.22\"},{\"x\":\"2.81\",\"y\":\"0.50\"},{\"x\":\"7.51\",\"y\":\"1.21\"},{\"x\":\"1.66\",\"y\":\"2.24\"},{\"x\":\"1.38\",\"y\":\"1.87\"},{\"x\":\"-0.36\",\"y\":\"5.19\"},{\"x\":\"0.00\",\"y\":\"2.38\"},{\"x\":\"-8.59\",\"y\":\"-3.64\"},{\"x\":\"-10.00\",\"y\":\"-4.57\"},{\"x\":\"-9.41\",\"y\":\"3.47\"},{\"x\":\"-6.16\",\"y\":\"2.27\"},{\"x\":\"-5.21\",\"y\":\"7.11\"},{\"x\":\"4.11\",\"y\":\"6.32\"},{\"x\":\"7.34\",\"y\":\"9.28\"},{\"x\":\"23.81\",\"y\":\"4.64\"},{\"x\":\"6.55\",\"y\":\"7.12\"},{\"x\":\"0.95\",\"y\":\"2.49\"},{\"x\":\"-0.87\",\"y\":\"14.51\"},{\"x\":\"-6.63\",\"y\":\"10.76\"},{\"x\":\"-17.36\",\"y\":\"3.77\"},{\"x\":\"-12.53\",\"y\":\"-0.53\"},{\"x\":\"-3.06\",\"y\":\"-2.62\"},{\"x\":\"-7.11\",\"y\":\"-1.31\"},{\"x\":\"-1.81\",\"y\":\"-2.58\"},{\"x\":\"-1.38\",\"y\":\"-1.96\"},{\"x\":\"0.36\",\"y\":\"-5.08\"},{\"x\":\"0.00\",\"y\":\"-2.45\"},{\"x\":\"10.03\",\"y\":\"4.31\"},{\"x\":\"9.75\",\"y\":\"5.12\"},{\"x\":\"11.22\",\"y\":\"-3.65\"}]},{\"points\":[{\"x\":\"-193.00\",\"y\":\"-64.78\"},{\"x\":\"9.32\",\"y\":\"26.00\"},{\"x\":\"13.68\",\"y\":\"35.00\"},{\"x\":\"15.32\",\"y\":\"-42.00\"},{\"x\":\"1.23\",\"y\":\"-3.32\"},{\"x\":\"4.13\",\"y\":\"-12.19\"},{\"x\":\"2.02\",\"y\":\"-1.89\"},{\"x\":\"2.27\",\"y\":\"-2.14\"},{\"x\":\"5.03\",\"y\":\"0.54\"},{\"x\":\"3.00\",\"y\":\"0.00\"},{\"x\":\"-21.19\",\"y\":\"56.00\"},{\"x\":\"-2.08\",\"y\":\"5.47\"},{\"x\":\"-1.94\",\"y\":\"10.91\"},{\"x\":\"-6.81\",\"y\":\"0.56\"},{\"x\":\"-9.13\",\"y\":\"0.76\"},{\"x\":\"0.56\",\"y\":\"-4.58\"},{\"x\":\"-4.81\",\"y\":\"-12.12\"},{\"x\":\"-21.60\",\"y\":\"-57.00\"},{\"x\":\"11.00\",\"y\":\"0.00\"}]},{\"points\":[{\"x\":\"70.37\",\"y\":\"0.13\"},{\"x\":\"-0.90\",\"y\":\"1.25\"},{\"x\":\"3.40\",\"y\":\"13.62\"},{\"x\":\"2.52\",\"y\":\"10.08\"},{\"x\":\"5.33\",\"y\":\"27.45\"},{\"x\":\"4.28\",\"y\":\"7.47\"},{\"x\":\"14.00\",\"y\":\"-60.00\"},{\"x\":\"12.00\",\"y\":\"0.00\"},{\"x\":\"7.63\",\"y\":\"32.00\"},{\"x\":\"7.37\",\"y\":\"29.00\"},{\"x\":\"10.13\",\"y\":\"-42.00\"},{\"x\":\"0.77\",\"y\":\"-3.09\"},{\"x\":\"2.71\",\"y\":\"-12.56\"},{\"x\":\"1.53\",\"y\":\"-1.78\"},{\"x\":\"1.86\",\"y\":\"-2.17\"},{\"x\":\"5.31\",\"y\":\"0.60\"},{\"x\":\"2.69\",\"y\":\"0.00\"},{\"x\":\"-18.00\",\"y\":\"73.00\"},{\"x\":\"-13.00\",\"y\":\"0.00\"},{\"x\":\"-14.00\",\"y\":\"-61.00\"},{\"x\":\"-2.00\",\"y\":\"0.00\"},{\"x\":\"-14.00\",\"y\":\"61.00\"},{\"x\":\"-13.00\",\"y\":\"0.00\"},{\"x\":\"-18.00\",\"y\":\"-73.00\"},{\"x\":\"4.00\",\"y\":\"0.00\"}]}]}";

void initConfig() {
    // defaults
    configJson["server"]["ssid"] = ssid;
    configJson["server"]["password"] = password;
    configJson["plotter"]["canvasWidth"] = canvasWidth;
    configJson["plotter"]["currentLeft"] = canvasWidth;
    configJson["plotter"]["currentRight"] = canvasWidth;
    configJson["plotter"]["centerX"] = canvasWidth / 2;
    configJson["plotter"]["centerY"] = 866; // TODO the height in the triangle
    configJson["plotter"]["zoomFactor"] = zoomFactor;
    zoom = getZoom(zoomFactor);
    serializeJson(configJson, configData);
    Serial.println(configData);
}

bool setConfig() {
    if (DeserializationError error = deserializeJson(configJson, configData)) {
        Serial.println("error parsing json");       
        return false;
    }
    Serial.println(configData);
    ssid = configJson["server"]["ssid"];
    password = configJson["server"]["password"];
    canvasWidth = configJson["plotter"]["canvasWidth"];
    currentLeft = configJson["plotter"]["currentLeft"];
    currentRight = configJson["plotter"]["currentRight"];
    centerX = configJson["plotter"]["centerX"];
    centerY = configJson["plotter"]["centerY"];
    zoomFactor = configJson["plotter"]["zoomFactor"];
    zoom = getZoom(zoomFactor);
    
    return true;
}

int initServer() {
    int retries = 0;
    Serial.println("Connecting...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while ((WiFi.status() != WL_CONNECTED) && (retries < WIFI_INIT_RETRY)) {
      retries++;
      delay(1000);
      Serial.print("#");
    }

    server.begin();
    Serial.println(WiFi.localIP());
    serverRouting();

    return WiFi.status();
}

void initMotors() {
    motorLeft.setStepDuration(motorSpeed);
    motorRight.setStepDuration(motorSpeed);

    servoPen.attach(SERVO_PIN);
    servoPen.write(PEN_UP);
}

void initFileSystem() {
    char configFile[1000];
    configFile[0] = 0; // init configFile
    SPIFFS.begin();
    // check if valid config.json exist else create one
    File f = SPIFFS.open("/config.json", "r");
    if (!f) {
        Serial.println("file open r failed");
        Serial.println("Please wait 30 secs for SPIFFS to be formatted");
        SPIFFS.format();
        Serial.println("Spiffs formatted");
        f = SPIFFS.open("/config.json", "w");
        Serial.println("writing config.json");
        Serial.println(configData);
        f.println(configData);
    } else {
        f.readStringUntil('\n').toCharArray(configFile, 1000);
        f.close();
        f = SPIFFS.open("/config.json", "w");
        if (strlen(configFile) == 0) {
          Serial.println("writing config.json");
          Serial.println(configData);
          f.println(configData);
        } else {
          Serial.println("config.json found:");
          Serial.println(configFile);
          memcpy(configData,configFile, strlen(configFile));
          setConfig();
        }
    }
    f.close();
}

bool initPlot(String json) {
    if (DeserializationError error = deserializeJson(plotJson, json)) {
        Serial.println("error parsing json");
        server.send(400);
        return false;
    }
    printing = true;
    server.sendHeader("Location", "/plot/");
    server.send(201);

    return true;
}

void postPlot() {
    String body = server.arg("plain");
    initPlot(body);
}

bool postZoomFactor() {
    StaticJsonDocument<50> zoomJson;
    String body = server.arg("plain");
    if (DeserializationError error = deserializeJson(zoomJson, body)) {
        Serial.println("error parsing json");
        server.send(400);
        return false;
    }
    zoomFactor = zoomJson["zoomFactor"];
    server.sendHeader("Location", "zoom:" + String(zoomFactor));
    server.send(201);

    return true;
}

void serverRouting() {
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", "/plot /zoomfactor");
    });
    server.on("/plot", HTTP_POST, postPlot);
    server.on("/zoomfactor", HTTP_POST, postZoomFactor);
}

bool getPoint(int line, int point, float *x, float* y)
{
    float newX = plotJson["lines"][line]["points"][point]["x"];
    float newY = plotJson["lines"][line]["points"][point]["y"];
    if (plotJson["lines"][line]["points"][point]["x"] == nullptr && plotJson["lines"][line]["points"][point]["y"] == nullptr) {

        return false;
    }
    *x = newX;
    *y = newY;
    homeX = homeX + newX;
    homeY = homeY + newY;

    return true;
}

void drawLine(long distanceLeft, long distanceRight){
    int directionLeft = motorLeftDirection;
    int directionRight = motorRightDirection;
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
    for (long i = 1; i <= ticks * zoom; i++) {
        if (i % distL == 0) {
            motorLeft.step(STEPS_PER_TICK * directionRight);
        }
        if (i % distR == 0) {
            motorRight.step(STEPS_PER_TICK * directionLeft);
        }
        yield();
    }
}

void getDistance(float x, float y, long *distanceLeft, long *distanceRight) {
    float nextX = x + lastX;
    float nextY = y + lastY;
    float leftX = nextX + centerX;
    float rightX = nextX - centerX;
    float yPos  = nextY + centerY;
    long newLeft  = sqrt(pow(leftX, 2) + pow(yPos, 2));
    long newRight = sqrt(pow(rightX, 2) + pow(yPos, 2));
    *distanceLeft  = (newLeft - currentLeft) * STEPS_PER_MM;
    *distanceRight = (newRight - currentRight) * STEPS_PER_MM;
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

void setup()
{
    Serial.begin(9600);
    Serial.println("Setup");
    initConfig();
    initFileSystem();

    Serial.print("Canvas width:");
    Serial.println(canvasWidth);

    initMotors();
    Serial.println(initServer());
    Serial.println(initPlot(plotData));

    delay(5000);
}

void loop() {
    server.handleClient();
    if (printing) {
        for (int line = 0; line < plotJson["lines"].size(); line++) {
            for (int point = 0; point < plotJson["lines"][line]["points"].size(); point++) {
                float tmpX = 0;
                float tmpY = 0;
                if(!getPoint(line, point, &tmpX, &tmpY)) {
                    servoPen.write(PEN_UP);
                    Serial.println("Plot error.");
                    printing = false;
                    goHome();
                    break;
                } else {
                    if (point == 0) {
                        servoPen.write(PEN_UP);
                    } else {
                        servoPen.write(PEN_DOWN);
                    }
                    long distanceLeft = 0;
                    long distanceRight = 0;
                    getDistance(tmpX,tmpY, &distanceLeft, &distanceRight);
                    drawLine(distanceLeft, distanceRight);
                }
            }
        }
        servoPen.write(PEN_UP);
        Serial.println("Plot done.");
        printing = false;
        goHome();
    }
}
