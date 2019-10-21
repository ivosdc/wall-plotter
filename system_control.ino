#include "Config.h"

void setOrigo() {
    // law of cosines
    float enumerator = pow(canvasWidth, 2) + pow(currentLeft, 2) - pow(currentRight, 2);
    float denominator = 2 * canvasWidth * currentLeft;
    float angle = acos(enumerator / denominator);
    origoX = currentLeft * cos(angle);
    origoY = currentLeft * sin(angle);
    Serial.print(origoX);
    Serial.print(" origo ");
    Serial.println(origoY);
}

void initConfig() {
    configJson["server"]["ssid"] = ssid;
    configJson["server"]["password"] = password;
    configJson["plotter"]["canvasWidth"] = canvasWidth;
    configJson["plotter"]["currentLeft"] = currentLeft;
    configJson["plotter"]["currentRight"] = currentRight;
    configJson["plotter"]["zoomFactor"] = zoomFactor;
    serializeJson(configJson, configData);
    setOrigo();
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
    zoomFactor = newConfigJson["plotter"]["zoomFactor"];
    initConfig();

    return true;
}

void initMotors() {
    motorLeft.setMaxSpeed(MOTOR_MAX_SPEED);
    motorRight.setMaxSpeed(MOTOR_MAX_SPEED);
    plotter.addStepper(motorLeft);
    plotter.addStepper(motorRight);
    servoPen.attach(SERVO_PIN);
    servoPen.write(PEN_UP);
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
