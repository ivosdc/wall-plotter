#include <Servo.h>
#include <ArduinoJson.h>
#include <StepperMotor.h>
#include <ESP8266WebServer.h>
#include "FS.h"
#include <DNSServer.h>
#include <ESP8266WiFi.h>

#define SERVO_PIN D9
#define WIFI_INIT_RETRY 20
#define PEN_UP 50
#define PEN_DOWN 70
#define SPOOL_CIRC 94.2 
#define STEPS_PER_ROTATION 4075.7728395
#define STEPS_PER_TICK 10
#define STEPS_PER_MM  (STEPS_PER_ROTATION / SPOOL_CIRC) / STEPS_PER_TICK
#define UPLOAD_PLOT_FILENAME "/wall-plotter.data"

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

const char HeaderUploadPlot[] PROGMEM = "HTTP/1.1 303 OK\r\nLocation:/plot\r\nCache-Control: no-cache\r\n";
const char UploadPlot[] PROGMEM = R"(<form method="POST" action="/plot" enctype="multipart/form-data">
     <input type="file" name="/wall-plotter.data"><input type="submit" value="Upload"></form>Upload a wall-plott.data)";

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
    configFile[0] = 0;
    SPIFFS.begin();
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
            while (pch != NULL && point > 0 && printing) {
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
                    printf ("X: %s\n",pch);
                    x = atof(pch);
                } else {
                    printf ("Y: %s\n",pch);
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

void getPlot() {
    if (SPIFFS.exists(UPLOAD_PLOT_FILENAME)) {
        File f = SPIFFS.open(UPLOAD_PLOT_FILENAME, "r");
        server.streamFile(f, "application/json");
        f.close();
    } else {
      server.send(404, "text/plain", "NotFound");
    }
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
    Serial.println("Upload.");
    static File fsUploadFile;
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        fsUploadFile = SPIFFS.open(UPLOAD_PLOT_FILENAME, "w");
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        printf("Upload Data: %u\n", upload.currentSize);
        if (fsUploadFile)
            fsUploadFile.write(upload.buf, upload.currentSize);
    } else if (upload.status == UPLOAD_FILE_END) {
        if (fsUploadFile)
            fsUploadFile.close();
        printf("Upload Size: %u\n", upload.totalSize);
        server.sendContent(HeaderUploadPlot);
    }
}

void postPlotStop() {
    printing = false;
    server.send(200, "text/plain", "Plot stopped.");
}

void getUpload() {
    server.send(200, "text/html", UploadPlot);
}

void getRoot() {
    server.send(200, "text/plain", "IP: " + WiFi.localIP().toString() + " Wall-plotter AP:" + WiFi.softAPIP().toString());
}


void serverRouting() {
    server.on("/", HTTP_GET, getRoot);
    server.on("/plot", HTTP_POST, []() {}, postFileUpload);
    server.on("/plot", HTTP_GET, getPlot);
    server.on("/plot/stop", HTTP_POST, postPlotStop);
    server.on("/plot/start", HTTP_POST, postPlotStart);
    server.on("/zoomfactor", HTTP_POST, postZoomFactor);
    server.on("/wlan", HTTP_POST, postWlanSettings);
    server.on("/upload", HTTP_GET, getUpload);
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
    Serial.println("Ready!");
}

void loop() {
    server.handleClient();
}

