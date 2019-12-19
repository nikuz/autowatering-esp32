#include <Arduino.h>
#include <WiFi.h>

#include "def.h"
#include "AppWiFiDef.h"
#include "AppWiFi.h"

static WifiStringVariable stringVariables[2];
String wifiFishStringCache = "fish";

AppWiFi::AppWiFi() {}

AppWiFi::~AppWiFi() {}

void AppWiFi::setVariable(String *var, const char *key) {
    int stringVarsLen = *(&stringVariables + 1) - stringVariables;
    for (int i = 0; i < stringVarsLen; i++) {
        if (!stringVariables[i].key) {
            stringVariables[i] = WifiStringVariable(var, key);
            break;
        }
    }
}

String &AppWiFi::getStringVariable(const char *key) {
    const int stringVarsLen = *(&stringVariables + 1) - stringVariables;
    for (int i = 0; i < stringVarsLen; i++) {
        if (stringVariables[i].key == key) {
            return *stringVariables[i].var;
        }
    }

    return wifiFishStringCache;
}

void AppWiFi::connect() {
    String &wifiSSIDVariable = AppWiFi::getStringVariable("wifiSSID");
    String &wifiPasswordVariable = AppWiFi::getStringVariable("wifiPassword");

    const char *wifiSSID = wifiSSIDVariable != "fish" && wifiSSIDVariable != ""
        ? wifiSSIDVariable.c_str()
        : SSID;
    const char *wifiPassword = wifiPasswordVariable != "fish" && wifiPasswordVariable != ""
        ? wifiPasswordVariable.c_str()
        : PSWD;

    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiSSID, wifiPassword);

    for (int loops = 10; loops > 0; loops--) {
        if (WiFi.isConnected()) {
            DEBUG_PRINTLN("");
            DEBUG_PRINT("SSID: ");
            DEBUG_PRINTLN(wifiSSID);
            DEBUG_PRINT("Password: ");
            DEBUG_PRINTLN(wifiPassword);
            DEBUG_PRINT("IP address: ");
            DEBUG_PRINTLN(WiFi.localIP());
            DEBUG_PRINT("DNS address: ");
            DEBUG_PRINTLN(WiFi.dnsIP());
            break;
        } else {
            DEBUG_PRINTLN(loops);
            delay(1000);
        }
    }
    if (!WiFi.isConnected()) {
        DEBUG_PRINTLN("WiFi connect failed");
    }
}

bool AppWiFi::isConnected() {
    return WiFi.isConnected();
}
