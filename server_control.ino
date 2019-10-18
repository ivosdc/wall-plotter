#include "Config.h"


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


void serverRouting() {
    server.on("/", HTTP_GET, getRoot);
    server.on("/plot", HTTP_POST, []() {}, postFileUpload);
    server.on("/plot", HTTP_GET, getPlot);
    server.on("/plot/stop", HTTP_POST, postPlotStop);
    server.on("/plot/start", HTTP_POST, postPlotStart);
    server.on("/zoomfactor", HTTP_POST, postZoomFactor);
    server.on("/wlan", HTTP_POST, postWlanSettings);
    server.on("/upload", HTTP_GET, getUpload);
    server.on("/config", HTTP_POST, postPlotterConfig);
}


