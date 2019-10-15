#include <Servo.h>
#include <ArduinoJson.h>
#include <StepperMotor.h>
#include <ESP8266WebServer.h>
#include "FS.h"
#include <DNSServer.h>
#include <ESP8266WiFi.h>

#define SERVO_PIN D9
#define WIFI_INIT_RETRY 20
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
IPAddress accessPointIP(192, 168, 0, 1);  
IPAddress netMask(255, 255, 255, 0);
DNSServer dnsServer;
StaticJsonDocument<20000> plotJson;
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

void initDNS() {
    Serial.println("Starting DNS-Server.");
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(53, "*", accessPointIP);
}

void initAccessPoint() {  
    Serial.print("Starting AccessPoint: ");
    static char szSSID[12];
    sprintf(szSSID, "WallPlotter %02d", ESP.getChipId() % 100);
    Serial.println(szSSID);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    bool exists = true;
    while(exists) {
        int n = WiFi.scanNetworks();
        exists = false;
        for (int i=0; i<n; i++) {
            String ssid = WiFi.SSID(i);
            if(strcmp(szSSID, ssid.c_str())==0)
                exists = true;
        }
        if(exists) {
            char accesPointInUse[50];
            sprintf(accesPointInUse, "AccessPoint '%s' in use, waiting...", szSSID);
            Serial.println(accesPointInUse);
            delay(5000);
        }
    }
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(accessPointIP, accessPointIP, netMask);
    WiFi.softAP(szSSID);
    yield();
    WiFi.persistent(false);
    WiFi.begin();
    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());
}

void initConfig() {
    configJson["server"]["ssid"] = ssid;
    configJson["server"]["password"] = password;
    configJson["plotter"]["canvasWidth"] = canvasWidth;
    configJson["plotter"]["currentLeft"] = currentLeft;
    configJson["plotter"]["currentRight"] = currentRight;
    configJson["plotter"]["centerX"] = centerX;
    configJson["plotter"]["centerY"] = centerY;
    configJson["plotter"]["zoomFactor"] = zoomFactor;
    zoom = getZoom(zoomFactor);
    serializeJson(configJson, configData);
}

bool setConfig() {
    StaticJsonDocument<1000> newConfigJson;
    char newConfigData[strlen(configData)];
    memcpy(newConfigData,configData, strlen(configData) + 1);
    if (DeserializationError error = deserializeJson(newConfigJson, newConfigData)) {
        Serial.println("error parsing json");       
        return false;
    }
    ssid = newConfigJson["server"]["ssid"];
    password = newConfigJson["server"]["password"];
    canvasWidth = newConfigJson["plotter"]["canvasWidth"];
    currentLeft = newConfigJson["plotter"]["currentLeft"];
    currentRight = newConfigJson["plotter"]["currentRight"];
    centerX = newConfigJson["plotter"]["centerX"];
    centerY = newConfigJson["plotter"]["centerY"];
    zoomFactor = newConfigJson["plotter"]["zoomFactor"];
    zoom = getZoom(zoomFactor);
    initConfig();
    
    return true;
}

void initServer() {
    int retries = 0;
    Serial.print("Connecting: ");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while ((WiFi.status() != WL_CONNECTED) && (retries < WIFI_INIT_RETRY)) {
        retries++;
        delay(1000);
        Serial.print(".");
    }
    Serial.println(WiFi.status() == 1 ? " failed" : " ok");
    if (WiFi.status() == 1) {
        initDNS();
        initAccessPoint();
    } else {
        Serial.println(WiFi.localIP());
    }
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
        Serial.println("read file failed");
        Serial.println("Please wait... formatting");
        SPIFFS.format();
        Serial.println("Done.");
        writeConfig();
    } else {
        f.readStringUntil('\n').toCharArray(configFile, 1000);
        f.close();
        Serial.println();        
        if (strlen(configFile) == 0) {
            Serial.println("writing config.json");
            writeConfig();
        } else {
            Serial.println("config.json found:");
            memcpy(configData,configFile, strlen(configFile) + 1);
            setConfig();
        }
    }
}

void writeConfig() {
    initConfig();
    File f = SPIFFS.open("/config.json", "w");
    int bytesWritten = f.println(configData);
    f.close();
    if (bytesWritten > 0) {
        Serial.println("Config written");
    } else {
        Serial.println("Config write failed");
    }
}

void readPlot() {
    File f = SPIFFS.open("/wall-plotter.data", "r");
    f.readStringUntil('\n').toCharArray(plotData, 10000);
}

void getPlot() {
    readPlot();
    server.send(201, "text/plain", plotData);
}

bool initPlot(String json) {
    Serial.println("Start plotting...");
    if (DeserializationError error = deserializeJson(plotJson, json)) {
        Serial.println("error parsing json");
        server.send(400);
        return false;
    }
    printing = true;
    server.send(201, "text/plain", "plot starting");

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
    server.send(201, "text/plain", "zoom:" + String(zoomFactor));
    writeConfig();

    return true;
}

bool postWlanSettings() {
    StaticJsonDocument<100> wlanJson;
    String body = server.arg("plain");
    if (DeserializationError error = deserializeJson(wlanJson, body)) {
        Serial.println("error parsing json");
        server.send(400);
        return false;
    }
    ssid = wlanJson["ssid"];
    password = wlanJson["password"];
    server.send(201, "text/plain", "wlan:" + String(ssid));
    writeConfig();
    initServer();

    return true;
}

void postFileUpload(){
    File fsUploadFile;
    server.send(200);
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        String filename = "wall-plotter.data";
        fsUploadFile = SPIFFS.open(filename, "w");
        filename = String();
    } else if((upload.status == UPLOAD_FILE_WRITE) && fsUploadFile) {
        fsUploadFile.write(upload.buf, upload.currentSize);
    } else if(upload.status == UPLOAD_FILE_END) {
        if(fsUploadFile) {
            fsUploadFile.close();
            Serial.print(" Size: ");
            Serial.println(upload.totalSize);
            server.send(303, "text/plain", "Uploaded.");
        } else {
            server.send(400, "text/plain", "Upload failed.");
        }
    }
}


void serverRouting() {
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", "POST: /plot /zoomfactor /wlan");
    });
    server.on("/plot", HTTP_POST, postPlot);
    server.on("/plot", HTTP_GET, getPlot);
    server.on("/zoomfactor", HTTP_POST, postZoomFactor);
    server.on("/wlan", HTTP_POST, postWlanSettings);
    server.on("/upload", HTTP_POST, postFileUpload);
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
    initServer();
    server.begin();
    serverRouting();

    initPlot(plotData);
    delay(4000);
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

