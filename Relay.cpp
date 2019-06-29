#include <Arduino.h>

#include "def.h"
#include "Relay.h"
#include "AppSerial.h"
#include "Sensor.h"
#include "AppTime.h"

const char relayOnSerialCommand[] = "rOn";
const char relayOffSerialCommand[] = "rOf";

// watering
bool watering1 = false;
bool watering2 = false;
bool watering3 = false;
bool watering4 = false;
bool watering5 = false;
bool watering6 = false;
bool watering7 = false;
bool watering8 = false;

Relay::Relay() {}

Relay::~Relay() {}

void Relay::parseSerialCommand(const char *command, const char *param) {
    if (strcmp(command, "rOn") == 0) {
        if (strcmp(param, "s1") == 0) {
            watering1 = true;
            DEBUG_PRINTLN("Open valve s1.");
        }
        if (strcmp(param, "s2") == 0) {
            watering2 = true;
            DEBUG_PRINTLN("Open valve s2.");
        }
        if (strcmp(param, "s3") == 0) {
            watering3 = true;
            DEBUG_PRINTLN("Open valve s3.");
        }
        if (strcmp(param, "s4") == 0) {
            watering4 = true;
            DEBUG_PRINTLN("Open valve s4.");
        }
        if (strcmp(param, "s5") == 0) {
            watering5 = true;
            DEBUG_PRINTLN("Open valve s5.");
        }
        if (strcmp(param, "s6") == 0) {
            watering6 = true;
            DEBUG_PRINTLN("Open valve s6.");
        }
        if (strcmp(param, "s7") == 0) {
            watering7 = true;
            DEBUG_PRINTLN("Open valve s7.");
        }
        if (strcmp(param, "s8") == 0) {
            watering8 = true;
            DEBUG_PRINTLN("Open valve s8.");
        }
    } else if (strcmp(command, "rOf") == 0) {
        if (strcmp(param, "s1") == 0) {
            watering1 = false;
            DEBUG_PRINTLN("Close valve s1.");
        }
        if (strcmp(param, "s2") == 0) {
            watering2 = false;
            DEBUG_PRINTLN("Close valve s2.");
        }
        if (strcmp(param, "s3") == 0) {
            watering3 = false;
            DEBUG_PRINTLN("Close valve s3.");
        }
        if (strcmp(param, "s4") == 0) {
            watering4 = false;
            DEBUG_PRINTLN("Close valve s4.");
        }
        if (strcmp(param, "s5") == 0) {
            watering5 = false;
            DEBUG_PRINTLN("Close valve s5.");
        }
        if (strcmp(param, "s6") == 0) {
            watering6 = false;
            DEBUG_PRINTLN("Close valve s6.");
        }
        if (strcmp(param, "s7") == 0) {
            watering7 = false;
            DEBUG_PRINTLN("Close valve s7.");
        }
        if (strcmp(param, "s8") == 0) {
            watering8 = false;
            DEBUG_PRINTLN("Close valve s8.");
        }
    }
}

// watering

bool Relay::isWateringEnabled() {
    return watering1
           || watering2
           || watering3
           || watering4
           || watering5
           || watering6
           || watering7
           || watering8;
}

bool Relay::IsValveOpenedFor(char *valveId) {
    if (strcmp(valveId, "s1") == 0) {
        return watering1;
    }
    if (strcmp(valveId, "s2") == 0) {
        return watering2;
    }
    if (strcmp(valveId, "s3") == 0) {
        return watering3;
    }
    if (strcmp(valveId, "s4") == 0) {
        return watering4;
    }
    if (strcmp(valveId, "s5") == 0) {
        return watering5;
    }
    if (strcmp(valveId, "s6") == 0) {
        return watering6;
    }
    if (strcmp(valveId, "s7") == 0) {
        return watering7;
    }
    if (strcmp(valveId, "s8") == 0) {
        return watering8;
    }

    return false;
}

void Relay::wateringOn(char *valveId) {
    SerialFrame openValveFrame = SerialFrame(relayOnSerialCommand, valveId);
    AppSerial::sendFrame(&openValveFrame);
}

void Relay::wateringOff(char *valveId) {
    SerialFrame closeValveFrame = SerialFrame(relayOffSerialCommand, valveId);
    AppSerial::sendFrame(&closeValveFrame);
}
