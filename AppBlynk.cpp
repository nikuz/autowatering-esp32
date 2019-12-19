#include <Arduino.h>
#include <EspOta.h>

#include "def.h"

#ifdef DEBUG
#define BLYNK_DEBUG // Optional, this enables lots of prints
#define BLYNK_PRINT Serial
#endif
#define BLYNK_NO_BUILTIN   // Disable built-in analog & digital pin operations
#define BLYNK_NO_FLOAT     // Disable float operations
#define BLYNK_MSG_LIMIT 50

#include <BlynkSimpleEsp32.h>

#include "AppBlynkDef.h"
#include "AppBlynk.h"
#include "AppWiFi.h"
#include "AppStorage.h"
#include "Tools.h"
#include "Sensor.h"
#include "Relay.h"
#include "AppTime.h"
#include "Watering.h"

// Blynk virtual pins
const int pinVersion = V5;
const int pinRtcBattery = V9;
const int wifiSSID = V25;
const int wifiPassword = V27;
const int pinOtaHost = V20;
const int pinOtaBin = V21;
const int pinOtaLastUpdateTime = V22;
const int pinUptime = V11;
const int pinRtcTemperature = V12;
const int pinMegaUptime = V14;
const int pinScreenEnabled = V15;
const int pinSoilMoisture1 = V40;
const int pinSoilMoisture2 = V41;
const int pinSoilMoisture3 = V42;
const int pinSoilMoisture4 = V43;
const int pinSoilMoisture5 = V44;
const int pinSoilMoisture6 = V45;
const int pinSoilMoisture7 = V46;
const int pinSoilMoisture8 = V47;
const int pinWSoilMstrMin = V23;
const int pinAutoWatering = V29;
const int pinWatering = V24;
const int pinWateringInterval = V26;
const int pinS1LstWtrng = V50;
const int pinS2LstWtrng = V51;
const int pinS3LstWtrng = V52;
const int pinS4LstWtrng = V53;
const int pinS5LstWtrng = V54;
const int pinS6LstWtrng = V55;
const int pinS7LstWtrng = V56;
const int pinS8LstWtrng = V57;
const int pinS1MnlWtrng = V60;
const int pinS2MnlWtrng = V61;
const int pinS3MnlWtrng = V62;
const int pinS4MnlWtrng = V63;
const int pinS5MnlWtrng = V64;
const int pinS6MnlWtrng = V65;
const int pinS7MnlWtrng = V66;
const int pinS8MnlWtrng = V67;
const int pinS1WtrngDur = V70;
const int pinS2WtrngDur = V71;
const int pinS3WtrngDur = V72;
const int pinS4WtrngDur = V73;
const int pinS5WtrngDur = V74;
const int pinS6WtrngDur = V75;
const int pinS7WtrngDur = V76;
const int pinS8WtrngDur = V77;
const int pinS1Enabled = V80;
const int pinS2Enabled = V81;
const int pinS3Enabled = V82;
const int pinS4Enabled = V83;
const int pinS5Enabled = V84;
const int pinS6Enabled = V85;
const int pinS7Enabled = V86;
const int pinS8Enabled = V87;

// cache
int fishIntCache = -32000;
int versionCache = 0;
int rtcBatteryCache = 0;
int rtcTemperatureCache = 0;
int soilMoistureCache1 = 0;
int soilMoistureCache2 = 0;
int soilMoistureCache3 = 0;
int soilMoistureCache4 = 0;
int soilMoistureCache5 = 0;
int soilMoistureCache6 = 0;
int soilMoistureCache7 = 0;
int soilMoistureCache8 = 0;
int wSoilMstrMinCache = 0;
int autoWateringCache = 0;
int wateringCache = 0;
String fishStringCache = "fish";
String wifiSSIDCache = "";
String wifiPasswordCache = "";
String otaHostCache = "";
String otaBinCache = "";
String otaLastUpdateTimeCache = "";
String uptimeCache = "";
String s1LstWtrngCache = "";
String s2LstWtrngCache = "";
String s3LstWtrngCache = "";
String s4LstWtrngCache = "";
String s5LstWtrngCache = "";
String s6LstWtrngCache = "";
String s7LstWtrngCache = "";
String s8LstWtrngCache = "";
String megaUptimeCache = "";

const unsigned long blynkConnectAttemptTime = 5UL * 1000UL;  // try to connect to blynk server only 5 seconds
bool blynkConnectAttemptFirstTime = true;
WidgetTerminal blynkTerminal(V30);

static BlynkIntVariable intVariables[30];
static BlynkStringVariable stringVariables[15];
static BlynkSyncVariable syncVariables[] = {
    {"wifiSSID",          false},
    {"wifiPassword",      false},
    {"otaHost",           false},
    {"otaBin",            false},
    {"otaLastUpdateTime", false},
    {"uptime",            false},
    {"version",           false},
    {"rtcBattery",        false},
    {"rtcTemperature",    false},
    {"soilMoisture1",     false},
    {"soilMoisture2",     false},
    {"soilMoisture3",     false},
    {"soilMoisture4",     false},
    {"soilMoisture5",     false},
    {"soilMoisture6",     false},
    {"soilMoisture7",     false},
    {"soilMoisture8",     false},
    {"watering",          false},
    {"s1LstWtrng",        false},
    {"s2LstWtrng",        false},
    {"s3LstWtrng",        false},
    {"s4LstWtrng",        false},
    {"s5LstWtrng",        false},
    {"s6LstWtrng",        false},
    {"s7LstWtrng",        false},
    {"s8LstWtrng",        false},
    {"megaUptime",        false},
};
const int syncValuesPerSecond = 5;

AppBlynk::AppBlynk() {};

AppBlynk::~AppBlynk() {};

// private

int AppBlynk::getPinById(const char *pinId) {
    if (strcmp(pinId, "version") == 0) return pinVersion;
    if (strcmp(pinId, "rtcBattery") == 0) return pinRtcBattery;
    if (strcmp(pinId, "wifiSSID") == 0) return wifiSSID;
    if (strcmp(pinId, "wifiPassword") == 0) return wifiPassword;
    if (strcmp(pinId, "otaHost") == 0) return pinOtaHost;
    if (strcmp(pinId, "otaBin") == 0) return pinOtaBin;
    if (strcmp(pinId, "otaLastUpdateTime") == 0) return pinOtaLastUpdateTime;
    if (strcmp(pinId, "uptime") == 0) return pinUptime;
    if (strcmp(pinId, "screenEnabled") == 0) return pinScreenEnabled;
    if (strcmp(pinId, "rtcTemperature") == 0) return pinRtcTemperature;
    if (strcmp(pinId, "soilMoisture1") == 0) return pinSoilMoisture1;
    if (strcmp(pinId, "soilMoisture2") == 0) return pinSoilMoisture2;
    if (strcmp(pinId, "soilMoisture3") == 0) return pinSoilMoisture3;
    if (strcmp(pinId, "soilMoisture4") == 0) return pinSoilMoisture4;
    if (strcmp(pinId, "soilMoisture5") == 0) return pinSoilMoisture5;
    if (strcmp(pinId, "soilMoisture6") == 0) return pinSoilMoisture6;
    if (strcmp(pinId, "soilMoisture7") == 0) return pinSoilMoisture7;
    if (strcmp(pinId, "soilMoisture8") == 0) return pinSoilMoisture8;
    if (strcmp(pinId, "wSoilMstrMin") == 0) return pinWSoilMstrMin;
    if (strcmp(pinId, "autoWatering") == 0) return pinAutoWatering;
    if (strcmp(pinId, "watering") == 0) return pinWatering;
    if (strcmp(pinId, "wInterval") == 0) return pinWateringInterval;
    if (strcmp(pinId, "s1LstWtrng") == 0) return pinS1LstWtrng;
    if (strcmp(pinId, "s2LstWtrng") == 0) return pinS2LstWtrng;
    if (strcmp(pinId, "s3LstWtrng") == 0) return pinS3LstWtrng;
    if (strcmp(pinId, "s4LstWtrng") == 0) return pinS4LstWtrng;
    if (strcmp(pinId, "s5LstWtrng") == 0) return pinS5LstWtrng;
    if (strcmp(pinId, "s6LstWtrng") == 0) return pinS6LstWtrng;
    if (strcmp(pinId, "s7LstWtrng") == 0) return pinS7LstWtrng;
    if (strcmp(pinId, "s8LstWtrng") == 0) return pinS8LstWtrng;
    if (strcmp(pinId, "s1MnlWtrng") == 0) return pinS1MnlWtrng;
    if (strcmp(pinId, "s2MnlWtrng") == 0) return pinS2MnlWtrng;
    if (strcmp(pinId, "s3MnlWtrng") == 0) return pinS3MnlWtrng;
    if (strcmp(pinId, "s4MnlWtrng") == 0) return pinS4MnlWtrng;
    if (strcmp(pinId, "s5MnlWtrng") == 0) return pinS5MnlWtrng;
    if (strcmp(pinId, "s6MnlWtrng") == 0) return pinS6MnlWtrng;
    if (strcmp(pinId, "s7MnlWtrng") == 0) return pinS7MnlWtrng;
    if (strcmp(pinId, "s8MnlWtrng") == 0) return pinS8MnlWtrng;
    if (strcmp(pinId, "s1WtrngDur") == 0) return pinS1WtrngDur;
    if (strcmp(pinId, "s2WtrngDur") == 0) return pinS2WtrngDur;
    if (strcmp(pinId, "s3WtrngDur") == 0) return pinS3WtrngDur;
    if (strcmp(pinId, "s4WtrngDur") == 0) return pinS4WtrngDur;
    if (strcmp(pinId, "s5WtrngDur") == 0) return pinS5WtrngDur;
    if (strcmp(pinId, "s6WtrngDur") == 0) return pinS6WtrngDur;
    if (strcmp(pinId, "s7WtrngDur") == 0) return pinS7WtrngDur;
    if (strcmp(pinId, "s8WtrngDur") == 0) return pinS8WtrngDur;
    if (strcmp(pinId, "s1Enabled") == 0) return pinS1Enabled;
    if (strcmp(pinId, "s2Enabled") == 0) return pinS2Enabled;
    if (strcmp(pinId, "s3Enabled") == 0) return pinS3Enabled;
    if (strcmp(pinId, "s4Enabled") == 0) return pinS4Enabled;
    if (strcmp(pinId, "s5Enabled") == 0) return pinS5Enabled;
    if (strcmp(pinId, "s6Enabled") == 0) return pinS6Enabled;
    if (strcmp(pinId, "s7Enabled") == 0) return pinS7Enabled;
    if (strcmp(pinId, "s8Enabled") == 0) return pinS8Enabled;
    if (strcmp(pinId, "megaUptime") == 0) return pinMegaUptime;

    return -1;
}

int &AppBlynk::getIntCacheValue(const char *pinId) {
    if (strcmp(pinId, "version") == 0) return versionCache;
    if (strcmp(pinId, "rtcBattery") == 0) return rtcBatteryCache;
    if (strcmp(pinId, "rtcTemperature") == 0) return rtcTemperatureCache;
    if (strcmp(pinId, "soilMoisture1") == 0) return soilMoistureCache1;
    if (strcmp(pinId, "soilMoisture2") == 0) return soilMoistureCache2;
    if (strcmp(pinId, "soilMoisture3") == 0) return soilMoistureCache3;
    if (strcmp(pinId, "soilMoisture4") == 0) return soilMoistureCache4;
    if (strcmp(pinId, "soilMoisture5") == 0) return soilMoistureCache5;
    if (strcmp(pinId, "soilMoisture6") == 0) return soilMoistureCache6;
    if (strcmp(pinId, "soilMoisture7") == 0) return soilMoistureCache7;
    if (strcmp(pinId, "soilMoisture8") == 0) return soilMoistureCache8;
    if (strcmp(pinId, "wSoilMstrMin") == 0) return wSoilMstrMinCache;
    if (strcmp(pinId, "autoWatering") == 0) return autoWateringCache;
    if (strcmp(pinId, "watering") == 0) return wateringCache;

    return fishIntCache;
}

String &AppBlynk::getStringCacheValue(const char *pinId) {
    if (strcmp(pinId, "wifiSSID") == 0) return wifiSSIDCache;
    if (strcmp(pinId, "wifiPassword") == 0) return wifiPasswordCache;
    if (strcmp(pinId, "otaHost") == 0) return otaHostCache;
    if (strcmp(pinId, "otaBin") == 0) return otaBinCache;
    if (strcmp(pinId, "otaLastUpdateTime") == 0) return otaLastUpdateTimeCache;
    if (strcmp(pinId, "uptime") == 0) return uptimeCache;
    if (strcmp(pinId, "s1LstWtrng") == 0) return s1LstWtrngCache;
    if (strcmp(pinId, "s2LstWtrng") == 0) return s2LstWtrngCache;
    if (strcmp(pinId, "s3LstWtrng") == 0) return s3LstWtrngCache;
    if (strcmp(pinId, "s4LstWtrng") == 0) return s4LstWtrngCache;
    if (strcmp(pinId, "s5LstWtrng") == 0) return s5LstWtrngCache;
    if (strcmp(pinId, "s6LstWtrng") == 0) return s6LstWtrngCache;
    if (strcmp(pinId, "s7LstWtrng") == 0) return s7LstWtrngCache;
    if (strcmp(pinId, "s8LstWtrng") == 0) return s8LstWtrngCache;
    if (strcmp(pinId, "megaUptime") == 0) return megaUptimeCache;

    return fishStringCache;
}

int &AppBlynk::getIntVariable(const char *pin) {
    const int intVarsLen = *(&intVariables + 1) - intVariables;
    for (int i = 0; i < intVarsLen; i++) {
        if (intVariables[i].pin == pin) {
            return *intVariables[i].var;
        }
    }

    return fishIntCache;
}

String &AppBlynk::getStringVariable(const char *pin) {
    const int stringVarsLen = *(&stringVariables + 1) - stringVariables;
    for (int i = 0; i < stringVarsLen; i++) {
        if (stringVariables[i].pin == pin) {
            return *stringVariables[i].var;
        }
    }

    return fishStringCache;
}

void AppBlynk::sync() { // every second
    DEBUG_PRINTLN("Check connect:");
    DEBUG_PRINT("Wifi connected: ");
    DEBUG_PRINTLN(AppWiFi::isConnected());
    DEBUG_PRINT("Blynk connected: ");
    DEBUG_PRINTLN(Blynk.connected());
    DEBUG_PRINT("Millis: ");
    DEBUG_PRINTLN(millis());
    DEBUG_PRINT("Overflow is close: ");
    DEBUG_PRINTLN(Tools::millisOverflowIsClose());
    if (!AppWiFi::isConnected() || !Blynk.connected() || Tools::millisOverflowIsClose()) {
        return;
    }

    int syncCounter = 0;
    const int varsLen = *(&syncVariables + 1) - syncVariables;
    DEBUG_PRINT("Vars to sync: ");
    DEBUG_PRINTLN(varsLen);
    for (int i = 0; i < varsLen; i++) {
        if (syncCounter < syncValuesPerSecond) {
            if (syncVariables[i].synced) {
                continue;
            }

            const char *pin = syncVariables[i].pin;
            DEBUG_PRINT("Sync pin: ");
            DEBUG_PRINT(pin);
            DEBUG_PRINT(": ");
            if (strcmp(pin, "wifiSSID") == 0) {
                String &wifiSSIDVariable = AppBlynk::getStringVariable(pin);
                AppBlynk::postData(pin, wifiSSIDVariable);
            };
            if (strcmp(pin, "wifiPassword") == 0) {
                String &wifiPasswordVariable = AppBlynk::getStringVariable(pin);
                AppBlynk::postData(pin, wifiPasswordVariable);
            };
            if (strcmp(pin, "otaHost") == 0) {
                String &otaHostVariable = AppBlynk::getStringVariable(pin);
                AppBlynk::postData(pin, otaHostVariable);
            };
            if (strcmp(pin, "otaBin") == 0) {
                String &otaBinVariable = AppBlynk::getStringVariable(pin);
                AppBlynk::postData(pin, otaBinVariable);
            };
            if (strcmp(pin, "otaLastUpdateTime") == 0) {
                AppBlynk::postData(pin, EspOta::getUpdateTime());
            };
            if (strcmp(pin, "uptime") == 0) {
                AppBlynk::postData(pin, String(Tools::getUptime()));
            };
            if (strcmp(pin, "version") == 0) {
                AppBlynk::postData(pin, VERSION);
            };
            if (strcmp(pin, "rtcBattery") == 0) {
                AppBlynk::postData(pin, AppTime::RTCBattery() ? 255 : 0);
            };
            if (strcmp(pin, "rtcTemperature") == 0) {
                AppBlynk::postData(pin, AppTime::RTCGetTemperature());
            };
            if (strcmp(pin, "soilMoisture1") == 0) {
                AppBlynk::postData(
                    pin,
                    AppBlynk::getIntVariable("s1Enabled") == 1 ? Sensor::getSoilMoisture(SOIL_SENSOR_1) : 0
                );
            };
            if (strcmp(pin, "soilMoisture2") == 0 && AppBlynk::getIntVariable("s2Enabled")) {
                AppBlynk::postData(
                    pin,
                    AppBlynk::getIntVariable("s2Enabled") == 1 ? Sensor::getSoilMoisture(SOIL_SENSOR_2) : 0
                );
            };
            if (strcmp(pin, "soilMoisture3") == 0) {
                AppBlynk::postData(
                    pin,
                    AppBlynk::getIntVariable("s3Enabled") == 1 ? Sensor::getSoilMoisture(SOIL_SENSOR_3) : 0
                );
            };
            if (strcmp(pin, "soilMoisture4") == 0) {
                AppBlynk::postData(
                    pin,
                    AppBlynk::getIntVariable("s4Enabled") == 1 ? Sensor::getSoilMoisture(SOIL_SENSOR_4) : 0
                );
            };
            if (strcmp(pin, "soilMoisture5") == 0) {
                AppBlynk::postData(
                    pin,
                    AppBlynk::getIntVariable("s5Enabled") == 1 ? Sensor::getSoilMoisture(SOIL_SENSOR_5) : 0
                );
            };
            if (strcmp(pin, "soilMoisture6") == 0) {
                AppBlynk::postData(
                    pin,
                    AppBlynk::getIntVariable("s6Enabled") == 1 ? Sensor::getSoilMoisture(SOIL_SENSOR_6) : 0
                );
            };
            if (strcmp(pin, "soilMoisture7") == 0) {
                AppBlynk::postData(
                    pin,
                    AppBlynk::getIntVariable("s7Enabled") == 1 ? Sensor::getSoilMoisture(SOIL_SENSOR_7) : 0
                );
            };
            if (strcmp(pin, "soilMoisture8") == 0) {
                AppBlynk::postData(
                    pin,
                    AppBlynk::getIntVariable("s8Enabled") == 1 ? Sensor::getSoilMoisture(SOIL_SENSOR_8) : 0
                );
            };
            if (strcmp(pin, "watering") == 0) {
                AppBlynk::postData(pin, Relay::isWateringEnabled() ? 255 : 0);
            };
            if (strcmp(pin, "s1LstWtrng") == 0) {
                AppBlynk::postData(pin, Watering::getStringVariable(pin));
            };
            if (strcmp(pin, "s2LstWtrng") == 0) {
                AppBlynk::postData(pin, Watering::getStringVariable(pin));
            };
            if (strcmp(pin, "s3LstWtrng") == 0) {
                AppBlynk::postData(pin, Watering::getStringVariable(pin));
            };
            if (strcmp(pin, "s4LstWtrng") == 0) {
                AppBlynk::postData(pin, Watering::getStringVariable(pin));
            };
            if (strcmp(pin, "s5LstWtrng") == 0) {
                AppBlynk::postData(pin, Watering::getStringVariable(pin));
            };
            if (strcmp(pin, "s6LstWtrng") == 0) {
                AppBlynk::postData(pin, Watering::getStringVariable(pin));
            };
            if (strcmp(pin, "s7LstWtrng") == 0) {
                AppBlynk::postData(pin, Watering::getStringVariable(pin));
            };
            if (strcmp(pin, "s8LstWtrng") == 0) {
                AppBlynk::postData(pin, Watering::getStringVariable(pin));
            };
            if (strcmp(pin, "megaUptime") == 0) {
                AppBlynk::postData(pin, String(Tools::getMegaUptime()));
            };
            syncVariables[i].synced = true;
            syncCounter++;
        }
    }

    if (syncCounter < syncValuesPerSecond) {
        for (int i = 0; i < varsLen; i++) {
            syncVariables[i].synced = false;
        }
    }

#if PRODUCTION

#endif
}

void writeHandler(const char *pin, int value, bool store) {
    int &variable = AppBlynk::getIntVariable(pin);
    AppBlynk::getData(variable, pin, value, store);
}

void writeHandler(const char *pin, String value, bool store) {
    String &variable = AppBlynk::getStringVariable(pin);
    AppBlynk::getData(variable, pin, value, store);
}

BLYNK_WRITE(V25) { // wifiSSID
    writeHandler("wifiSSID", param.asStr(), true);
};
BLYNK_WRITE(V27) { // wifiPassword
    writeHandler("wifiPassword", param.asStr(), true);
};
BLYNK_WRITE(V20) { // otaHost
    writeHandler("otaHost", param.asStr(), true);
};
BLYNK_WRITE(V21) { // otaBin
    writeHandler("otaBin", param.asStr(), true);
};
BLYNK_WRITE(V15) { // screenEnabled
    writeHandler("screenEnabled", param.asInt(), true);
};
BLYNK_WRITE(V23) { // wSoilMstrMin
    writeHandler("wSoilMstrMin", param.asInt(), true);
};
BLYNK_WRITE(V26) { // wInterval
    writeHandler("wInterval", param.asInt(), true);
};
BLYNK_WRITE(V29) { // autoWatering
    int value = param.asInt();
    if (value == 0) {
        Watering::stop();
    }
    writeHandler("autoWatering", value, true);
};
BLYNK_WRITE(V60) {
    int value = param.asInt();
    if (value == 0) {
        Watering::stop();
    }
    writeHandler("s1MnlWtrng", value, false);
};
BLYNK_WRITE(V61) {
    int value = param.asInt();
    if (value == 0) {
        Watering::stop();
    }
    writeHandler("s2MnlWtrng", value, false);
};
BLYNK_WRITE(V62) {
    int value = param.asInt();
    if (value == 0) {
        Watering::stop();
    }
    writeHandler("s3MnlWtrng", value, false);
};
BLYNK_WRITE(V63) {
    int value = param.asInt();
    if (value == 0) {
        Watering::stop();
    }
    writeHandler("s4MnlWtrng", value, false);
};
BLYNK_WRITE(V64) {
    int value = param.asInt();
    if (value == 0) {
        Watering::stop();
    }
    writeHandler("s5MnlWtrng", value, false);
};
BLYNK_WRITE(V65) {
    int value = param.asInt();
    if (value == 0) {
        Watering::stop();
    }
    writeHandler("s6MnlWtrng", value, false);
};
BLYNK_WRITE(V66) {
    int value = param.asInt();
    if (value == 0) {
        Watering::stop();
    }
    writeHandler("s7MnlWtrng", value, false);
};
BLYNK_WRITE(V67) {
    int value = param.asInt();
    if (value == 0) {
        Watering::stop();
    }
    writeHandler("s8MnlWtrng", value, false);
};
BLYNK_WRITE(V70) {
    writeHandler("s1WtrngDur", param.asInt(), true);
};
BLYNK_WRITE(V71) {
    writeHandler("s2WtrngDur", param.asInt(), true);
};
BLYNK_WRITE(V72) {
    writeHandler("s3WtrngDur", param.asInt(), true);
};
BLYNK_WRITE(V73) {
    writeHandler("s4WtrngDur", param.asInt(), true);
};
BLYNK_WRITE(V74) {
    writeHandler("s5WtrngDur", param.asInt(), true);
};
BLYNK_WRITE(V75) {
    writeHandler("s6WtrngDur", param.asInt(), true);
};
BLYNK_WRITE(V76) {
    writeHandler("s7WtrngDur", param.asInt(), true);
};
BLYNK_WRITE(V77) {
    writeHandler("s8WtrngDur", param.asInt(), true);
};
BLYNK_WRITE(V80) {
    writeHandler("s1Enabled", param.asInt(), true);
};
BLYNK_WRITE(V81) {
    writeHandler("s2Enabled", param.asInt(), true);
};
BLYNK_WRITE(V82) {
    writeHandler("s3Enabled", param.asInt(), true);
};
BLYNK_WRITE(V83) {
    writeHandler("s4Enabled", param.asInt(), true);
};
BLYNK_WRITE(V84) {
    writeHandler("s5Enabled", param.asInt(), true);
};
BLYNK_WRITE(V85) {
    writeHandler("s6Enabled", param.asInt(), true);
};
BLYNK_WRITE(V86) {
    writeHandler("s7Enabled", param.asInt(), true);
};
BLYNK_WRITE(V87) {
    writeHandler("s8Enabled", param.asInt(), true);
};
BLYNK_WRITE(V10) { // ping
    if (param.asInt() == 1) {
        AppBlynk::notify("PONG");
        Blynk.virtualWrite(V10, 0);
    }
};
BLYNK_WRITE(V13) { // get time
    if (param.asInt() == 1) {
        AppTime::print();
        Blynk.virtualWrite(V13, 0);
    }
};
BLYNK_WRITE(V31) { // restart
    if (param.asInt() == 1) {
        Blynk.virtualWrite(V31, 0);
        delay(2000);
        ESP.restart();
    }
};
BLYNK_WRITE(V35) { // mega restart
    if (param.asInt() == 1) {
        Blynk.virtualWrite(V35, 0);
        Tools::megaRestart();
        delay(2000);
    }
};

BLYNK_CONNECTED() {
    Blynk.syncAll();
};

// public

void AppBlynk::setVariable(int *var, const char *pin) {
    int varsCount = *(&intVariables + 1) - intVariables;
    for (int i = 0; i < varsCount; i++) {
        if (!intVariables[i].pin) {
            intVariables[i] = BlynkIntVariable(var, pin);
            break;
        }
    }
}

void AppBlynk::setVariable(String *var, const char *pin) {
    int varsCount = *(&stringVariables + 1) - stringVariables;
    for (int i = 0; i < varsCount; i++) {
        if (!stringVariables[i].pin) {
            stringVariables[i] = BlynkStringVariable(var, pin);
            break;
        }
    }
}

void AppBlynk::checkConnect() {
    DEBUG_PRINTLN("Check connect:");
    DEBUG_PRINT("Wifi connected: ");
    DEBUG_PRINTLN(AppWiFi::isConnected());
    DEBUG_PRINT("Blynk connected: ");
    DEBUG_PRINTLN(Blynk.connected());
    DEBUG_PRINT("Millis: ");
    DEBUG_PRINTLN(millis());
    DEBUG_PRINT("Overflow is close: ");
    DEBUG_PRINTLN(Tools::millisOverflowIsClose());
    if (!blynkConnectAttemptFirstTime && Tools::millisOverflowIsClose()) {
        return;
    }
    if (AppWiFi::isConnected() && !Blynk.connected()) {
        unsigned long startConnecting = millis();
        while (!Blynk.connected()) {
            Blynk.connect();
            if (millis() > startConnecting + blynkConnectAttemptTime) {
                Serial.println("Unable to connect to Blynk server.\n");
                break;
            }
        }
        if (Blynk.connected() && blynkConnectAttemptFirstTime) {
            blynkTerminal.clear();
        }
        blynkConnectAttemptFirstTime = false;
    }
}

void AppBlynk::initiate() {
    Blynk.config(blynkAuth, blynkDomain, blynkPort);
    AppBlynk::checkConnect();
}

void AppBlynk::run() {
    if (Blynk.connected()) {
        Blynk.run();
    }
}

void AppBlynk::getData(int &localVariable, const char *pinId, int pinData, const bool storePreferences) {
    int blynkPin = AppBlynk::getPinById(pinId);
    if (localVariable == -1 || blynkPin == -1) {
        return;
    }
    if (pinData != localVariable) {
        localVariable = pinData;
        if (storePreferences) {
            AppStorage::putUInt(pinId, pinData);
        }
    }
}

void AppBlynk::getData(String &localVariable, const char *pinId, String pinData, const bool storePreferences) {
    int blynkPin = AppBlynk::getPinById(pinId);
    if (localVariable == "fish" || blynkPin == -1) {
        return;
    }
    if (pinData != localVariable) {
        localVariable = pinData;
        if (storePreferences) {
            AppStorage::putString(pinId, pinData);
        }
    }
}

void AppBlynk::postData(const char *pinId, int value) {
    int blynkPin = AppBlynk::getPinById(pinId);
    if (blynkPin == -1) {
        return;
    }
    int &cacheValue = AppBlynk::getIntCacheValue(pinId);
    if (cacheValue != -32000 || cacheValue != value) { // post data also if cache not applied for pin
        if (Blynk.connected()) {
            Blynk.virtualWrite(blynkPin, value);
        }
        if (cacheValue != -32000) {
            cacheValue = value;
        }
    }
}

void AppBlynk::postData(const char *pinId, String value) {
    int blynkPin = AppBlynk::getPinById(pinId);
    if (blynkPin == -1) {
        return;
    }
    String &cacheValue = AppBlynk::getStringCacheValue(pinId);
    if (cacheValue != "fish" || cacheValue != value) { // post data also if cache not applied for pin
        if (Blynk.connected()) {
            Blynk.virtualWrite(blynkPin, value);
        }
        if (cacheValue != "fish") {
            cacheValue = value;
        }
    }
}

void AppBlynk::postDataNoCache(const char *pinId, int value) {
    int blynkPin = AppBlynk::getPinById(pinId);
    if (blynkPin == -1) {
        return;
    }
    if (Blynk.connected()) {
        Blynk.virtualWrite(blynkPin, value);
    }
}

void AppBlynk::postDataNoCache(const char *pinId, String value) {
    int blynkPin = AppBlynk::getPinById(pinId);
    if (blynkPin == -1) {
        return;
    }
    if (Blynk.connected()) {
        Blynk.virtualWrite(blynkPin, value);
    }
}

void AppBlynk::print(String value) {
    Serial.print(value);
    if (AppWiFi::isConnected() && Blynk.connected() && !Tools::millisOverflowIsClose()) {
        blynkTerminal.print(value);
    }
}

void AppBlynk::print(char *value) {
    Serial.print(value);
    if (AppWiFi::isConnected() && Blynk.connected() && !Tools::millisOverflowIsClose()) {
        blynkTerminal.print(value);
    }
}

void AppBlynk::print(int value) {
    Serial.print(value);
    if (AppWiFi::isConnected() && Blynk.connected() && !Tools::millisOverflowIsClose()) {
        blynkTerminal.print(value);
    }
}

void AppBlynk::print(double value) {
    Serial.print(value);
    if (AppWiFi::isConnected() && Blynk.connected() && !Tools::millisOverflowIsClose()) {
        blynkTerminal.print(value);
    }
}

void AppBlynk::println(String value) {
    Serial.println(value);
    if (AppWiFi::isConnected() && Blynk.connected() && !Tools::millisOverflowIsClose()) {
        blynkTerminal.print(value);
        blynkTerminal.flush();
    }
}

void AppBlynk::println(char *value) {
    Serial.println(value);
    if (AppWiFi::isConnected() && Blynk.connected() && !Tools::millisOverflowIsClose()) {
        blynkTerminal.println(value);
        blynkTerminal.flush();
    }
}

void AppBlynk::println(int value) {
    Serial.println(value);
    if (AppWiFi::isConnected() && Blynk.connected() && !Tools::millisOverflowIsClose()) {
        blynkTerminal.println(value);
        blynkTerminal.flush();
    }
}

void AppBlynk::println(double value) {
    Serial.println(value);
    if (AppWiFi::isConnected() && Blynk.connected() && !Tools::millisOverflowIsClose()) {
        blynkTerminal.println(value);
        blynkTerminal.flush();
    }
}

void AppBlynk::notify(String value) {
    if (AppWiFi::isConnected() && Blynk.connected() && !Tools::millisOverflowIsClose()) {
        Blynk.notify(value);
    }
}
