#include <Arduino.h>

#include "def.h"
#include "Watering.h"
#include "AppTime.h"
#include "AppStorage.h"
#include "AppBlynk.h"
#include "Sensor.h"
#include "Relay.h"
#include "Tools.h"

static WateringIntVariable intVariables[30];
static WateringStringVariable stringVariables[10];
static WateringTargetVariable targetVariables[] = {
    {"s1", "s1Enabled", "s1LstWtrng", "s1MnlWtrng", "s1WtrngDur", false, {0}},
    {"s2", "s2Enabled", "s2LstWtrng", "s2MnlWtrng", "s2WtrngDur", false, {0}},
    {"s3", "s3Enabled", "s3LstWtrng", "s3MnlWtrng", "s3WtrngDur", false, {0}},
    {"s4", "s4Enabled", "s4LstWtrng", "s4MnlWtrng", "s4WtrngDur", false, {0}},
    {"s5", "s5Enabled", "s5LstWtrng", "s5MnlWtrng", "s5WtrngDur", false, {0}},
    {"s6", "s6Enabled", "s6LstWtrng", "s6MnlWtrng", "s6WtrngDur", false, {0}},
    {"s7", "s7Enabled", "s7LstWtrng", "s7MnlWtrng", "s7WtrngDur", false, {0}},
    {"s8", "s8Enabled", "s8LstWtrng", "s8MnlWtrng", "s8WtrngDur", false, {0}},
};
static int blankIntVariable = -1;
static String blankStringVariable = "";

char wateringStartedFor[10];

bool wateringStarted = false;
struct tm wateringStartedAt = {0};
int wateringDuration; // in sec
bool wateringPassed = false;

bool valveIsOpen = false;

int soilMoistureStarted = 0;

Watering::Watering() {}

Watering::~Watering() {}

int &Watering::getIntVariable(const char *key) {
    const int varsLen = *(&intVariables + 1) - intVariables;
    for (int i = 0; i < varsLen; i++) {
        if (intVariables[i].key == key) {
            return *intVariables[i].var;
        }
    }

    return blankIntVariable;
}

String &Watering::getStringVariable(const char *key) {
    const int varsLen = *(&stringVariables + 1) - stringVariables;
    for (int i = 0; i < varsLen; i++) {
        if (stringVariables[i].key == key) {
            return *stringVariables[i].var;
        }
    }

    return blankStringVariable;
}

bool isWateringEnabled() {
    bool manualWateringEnabled = false;
    const int varsLen = *(&targetVariables + 1) - targetVariables;
    for (int i = 0; i < varsLen; i++) {
        if (Watering::getIntVariable(targetVariables[i].manualWateringVar) == 1) {
            manualWateringEnabled = true;
        }
    }
    return manualWateringEnabled || Watering::getIntVariable("autoWatering") == 1;
}

int getSoilMoisture(const char *sensorId) {
    if (strcmp(sensorId, "s1") == 0) {
        return Sensor::getSoilMoisture(SOIL_SENSOR_1);
    }
    if (strcmp(sensorId, "s2") == 0) {
        return Sensor::getSoilMoisture(SOIL_SENSOR_2);
    }
    if (strcmp(sensorId, "s3") == 0) {
        return Sensor::getSoilMoisture(SOIL_SENSOR_3);
    }
    if (strcmp(sensorId, "s4") == 0) {
        return Sensor::getSoilMoisture(SOIL_SENSOR_4);
    }
    if (strcmp(sensorId, "s5") == 0) {
        return Sensor::getSoilMoisture(SOIL_SENSOR_5);
    }
    if (strcmp(sensorId, "s6") == 0) {
        return Sensor::getSoilMoisture(SOIL_SENSOR_6);
    }
    if (strcmp(sensorId, "s7") == 0) {
        return Sensor::getSoilMoisture(SOIL_SENSOR_7);
    }
    if (strcmp(sensorId, "s8") == 0) {
        return Sensor::getSoilMoisture(SOIL_SENSOR_8);
    }

    return 0;
}

int getWateringInterval() {
    int &wateringInterval = Watering::getIntVariable("wInterval");
    return wateringInterval * 60;
}

double getLastTimeWateringForPot(char *lastWateringVar) {
    String &lastWateringTime = Watering::getStringVariable(lastWateringVar);
    if (lastWateringTime == "") {
        return getWateringInterval();
    }
    double lastWateringSec = AppTime::compareDates(lastWateringTime, AppTime::getCurrentTime());
    return lastWateringSec > 0 ? lastWateringSec : 0;
}

void printLastWateringTime(char *name, double prevWateringSec) {
    Serial.print("Prev watering for ");
    Serial.print(name);
    Serial.print(" was only ");
    Serial.print(prevWateringSec);
    Serial.println(" sec ago.");
    Serial.print("Must be at least ");
    Serial.print(getWateringInterval());
    Serial.println(" sec.");
}

void sendNotification(char *name, int potSoilMoisture) {
    int wateringInterval = getWateringInterval();
    const int varsLen = *(&targetVariables + 1) - targetVariables;
    for (int i = 0; i < varsLen; i++) {
        if (strcmp(name, targetVariables[i].name) == 0) {
            bool notificationSent = targetVariables[i].notificationSent;
            struct tm notificationSentAt = targetVariables[i].notificationSentAt;
            struct tm now = AppTime::getCurrentTime();
            if (
                !notificationSent
                || AppTime::compareDates(notificationSentAt, now) >= wateringInterval
            ) {
                targetVariables[i].notificationSent = true;
                targetVariables[i].notificationSentAt = now;
                AppBlynk::notify(
                    "Autowatering low soil moisture at " + String(name) + ": " + String(potSoilMoisture)
                );
            }
        }
    }
}

// stopping (3 stage)

void stopping() {
    if (!wateringStarted) {
        return;
    }

    if (wateringPassed) {
        Watering::stop();
    }
}

// stopping (2 stage)

void valve() {
    if (
        valveIsOpen
        && AppTime::compareDates(wateringStartedAt, AppTime::getCurrentTime()) >= wateringDuration
    ) {
        AppBlynk::println("Stop watering");
        wateringPassed = true;
        return;
    }

    if (!valveIsOpen && Relay::IsValveOpenedFor(wateringStartedFor)) {
        valveIsOpen = true;
        wateringStartedAt = AppTime::getCurrentTime();

        const int varsLen = *(&targetVariables + 1) - targetVariables;
        for (int i = 0; i < varsLen; i++) {
            if (strcmp(wateringStartedFor, targetVariables[i].name) == 0) {
                wateringDuration = Watering::getIntVariable(targetVariables[i].duration);
            }
        }

        AppBlynk::print("Valve was open for: ");
        AppBlynk::println(wateringStartedFor);
        AppBlynk::print("For: ");
        AppBlynk::print(wateringDuration);
        AppBlynk::println(" sec");
        return;
    }

    if (!valveIsOpen) {
        Relay::wateringOpenValve(wateringStartedFor);
        AppBlynk::print("Try to open valve for: ");
        AppBlynk::println(wateringStartedFor);
    }
}

// soil moisture check (1 stage)

void soilMoisture() {
    int &wSoilMstrMin = Watering::getIntVariable("wSoilMstrMin");
    int wateringInterval = getWateringInterval();
    char manualWateringNeededFor[10];
    bool isManualWateringEnabled = false;

    const int varsLen = *(&targetVariables + 1) - targetVariables;
    for (int i = 0; i < varsLen; i++) {
        char *targetVarName = targetVariables[i].name;
        int &enabled = Watering::getIntVariable(targetVariables[i].enabled);
        int &manualWatering = Watering::getIntVariable(targetVariables[i].manualWateringVar);
        if (enabled == 1 && manualWatering == 1) {
            Serial.println(targetVarName);
            isManualWateringEnabled = true;
            strcpy(manualWateringNeededFor, targetVarName);
            break;
        }
    }
    if (!wateringStarted) {
        for (int i = 0; i < varsLen; i++) {
            char *targetVarName = targetVariables[i].name;
            int &enabled = Watering::getIntVariable(targetVariables[i].enabled);
            if (
                (isManualWateringEnabled && strcmp(manualWateringNeededFor, targetVarName) != 0)
                || enabled != 1
            ) {
                continue;
            }
            double lastWateringSec = getLastTimeWateringForPot(targetVariables[i].lastWateringVar);
            int potSoilMoisture = getSoilMoisture(targetVarName);
            if (potSoilMoisture < wSoilMstrMin) {
                if (isWateringEnabled()) {
                    if (lastWateringSec >= wateringInterval) {
                        strcpy(wateringStartedFor, targetVarName);
                        soilMoistureStarted = potSoilMoisture;
                        wateringStarted = true;
                        break;
                    } else {
                        printLastWateringTime(targetVarName, lastWateringSec);
                    }
                } else {
                    sendNotification(targetVarName, potSoilMoisture);
                }
            }
        }
    }

    if (wateringStarted) {
        AppBlynk::print("Start watering for: ");
        AppBlynk::println(wateringStartedFor);
        AppBlynk::print("Initial moisture: ");
        AppBlynk::println(soilMoistureStarted);
        AppBlynk::print("Minimum must be: ");
        AppBlynk::println(wSoilMstrMin);
    }
}

// public

void Watering::setVariable(int *var, const char *key) {
    int varsLen = *(&intVariables + 1) - intVariables;
    for (int i = 0; i < varsLen; i++) {
        if (!intVariables[i].key) {
            intVariables[i] = WateringIntVariable(var, key);
            break;
        }
    }
}

void Watering::setVariable(String *var, const char *key) {
    int varsLen = *(&stringVariables + 1) - stringVariables;
    for (int i = 0; i < varsLen; i++) {
        if (!stringVariables[i].key) {
            stringVariables[i] = WateringStringVariable(var, key);
            break;
        }
    }
}

void Watering::stop() {
    if (wateringStarted) {
        Relay::wateringCloseValve(wateringStartedFor);
        const int varsLen = *(&targetVariables + 1) - targetVariables;
        for (int i = 0; i < varsLen; i++) {
            // save time of last watering for certain pot
            if (strcmp(wateringStartedFor, targetVariables[i].name) == 0) {
                String &lastTimeWatering = Watering::getStringVariable(targetVariables[i].lastWateringVar);
                lastTimeWatering = AppTime::getTimeString(AppTime::getCurrentTime());
                AppStorage::putString(targetVariables[i].lastWateringVar, lastTimeWatering);
            }
            // disable manual watering to do the watering only once
            int &manualWateringEnabled = Watering::getIntVariable(targetVariables[i].manualWateringVar);
            if (manualWateringEnabled == 1) {
                manualWateringEnabled = 0;
                AppBlynk::postDataNoCache(targetVariables[i].manualWateringVar, 0);
            }
        }
        AppBlynk::println("Watering stopped!");
    }

    valveIsOpen = false;
    wateringStarted = false;
    wateringStartedAt = {0};
    wateringPassed = false;
    memset(wateringStartedFor, 0, sizeof wateringStartedFor);
    soilMoistureStarted = 0;
    wateringStarted = false;
}

void Watering::checkProgress() {
    if ((wateringStarted && Tools::millisOverflowIsClose())) {
        Watering::stop();
        return;
    }
    if (!isWateringEnabled() || !wateringStarted) {
        return;
    }
    valve();
    stopping();
}

void Watering::check() {
    if (
        isWateringEnabled()
        && !Tools::millisOverflowIsClose()
        && !wateringStarted
    ) {
        soilMoisture();
    }
}

