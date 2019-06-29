#include <Arduino.h>

#include "def.h"
#include "Relay.h"
#include "AppSerial.h"
#include "Sensor.h"
#include "AppTime.h"

const char relayOnSerialCommand[] = "rOn";
const char relayOffSerialCommand[] = "rOf";

// watering
bool wateringEnabled = false;
bool wateringOpenedValve1 = false;
bool wateringOpenedValve2 = false;
bool wateringOpenedValve3 = false;
bool wateringOpenedValve4 = false;
bool wateringOpenedValve5 = false;
bool wateringOpenedValve6 = false;
bool wateringOpenedValve7 = false;
bool wateringOpenedValve8 = false;

Relay::Relay() {}

Relay::~Relay() {}

void Relay::parseSerialCommand(const char *command, const char *param) {
    if (strcmp(command, "rOn") == 0) {
        if (strcmp(param, "s1") == 0) {
            wateringOpenedValve1 = true;
            DEBUG_PRINTLN("Open valve s1.");
        }
        if (strcmp(param, "s2") == 0) {
            wateringOpenedValve2 = true;
            DEBUG_PRINTLN("Open valve s2.");
        }
        if (strcmp(param, "s3") == 0) {
            wateringOpenedValve3 = true;
            DEBUG_PRINTLN("Open valve s3.");
        }
        if (strcmp(param, "s4") == 0) {
            wateringOpenedValve4 = true;
            DEBUG_PRINTLN("Open valve s4.");
        }
        if (strcmp(param, "s5") == 0) {
            wateringOpenedValve5 = true;
            DEBUG_PRINTLN("Open valve s5.");
        }
        if (strcmp(param, "s6") == 0) {
            wateringOpenedValve6 = true;
            DEBUG_PRINTLN("Open valve s6.");
        }
        if (strcmp(param, "s7") == 0) {
            wateringOpenedValve7 = true;
            DEBUG_PRINTLN("Open valve s7.");
        }
        if (strcmp(param, "s8") == 0) {
            wateringOpenedValve8 = true;
            DEBUG_PRINTLN("Open valve s8.");
        }
        if (strcmp(param, "water") == 0) {
            wateringEnabled = true;
            DEBUG_PRINTLN("Watering ON.");
        }
    } else if (strcmp(command, "rOf") == 0) {
        if (strcmp(param, "s1") == 0) {
            wateringOpenedValve1 = false;
            DEBUG_PRINTLN("Close valve s1.");
        }
        if (strcmp(param, "s2") == 0) {
            wateringOpenedValve2 = false;
            DEBUG_PRINTLN("Close valve s2.");
        }
        if (strcmp(param, "s3") == 0) {
            wateringOpenedValve3 = false;
            DEBUG_PRINTLN("Close valve s3.");
        }
        if (strcmp(param, "s4") == 0) {
            wateringOpenedValve4 = false;
            DEBUG_PRINTLN("Close valve s4.");
        }
        if (strcmp(param, "s5") == 0) {
            wateringOpenedValve5 = false;
            DEBUG_PRINTLN("Close valve s5.");
        }
        if (strcmp(param, "s6") == 0) {
            wateringOpenedValve6 = false;
            DEBUG_PRINTLN("Close valve s6.");
        }
        if (strcmp(param, "s7") == 0) {
            wateringOpenedValve7 = false;
            DEBUG_PRINTLN("Close valve s7.");
        }
        if (strcmp(param, "s8") == 0) {
            wateringOpenedValve8 = false;
            DEBUG_PRINTLN("Close valve s8.");
        }
        // do not need to indicate wateringEnabled as false when wmixing off,
        // because watering process is complex, and going in cascade
        // wateringEnabled will be off on water off, this event happen on any watering terminating event
        if (strcmp(param, "water") == 0) {
            wateringEnabled = false;
            DEBUG_PRINTLN("Watering OFF.");
        }
    }
}

// watering

bool Relay::isWateringOn() {
    return wateringEnabled;
}

bool Relay::wateringValveIsOpen(char *valveId) {
    if (strcmp(valveId, "s1") == 0) {
        return wateringOpenedValve1;
    }
    if (strcmp(valveId, "s2") == 0) {
        return wateringOpenedValve2;
    }
    if (strcmp(valveId, "s3") == 0) {
        return wateringOpenedValve3;
    }
    if (strcmp(valveId, "s4") == 0) {
        return wateringOpenedValve4;
    }
    if (strcmp(valveId, "s5") == 0) {
        return wateringOpenedValve5;
    }
    if (strcmp(valveId, "s6") == 0) {
        return wateringOpenedValve6;
    }
    if (strcmp(valveId, "s7") == 0) {
        return wateringOpenedValve7;
    }
    if (strcmp(valveId, "s8") == 0) {
        return wateringOpenedValve8;
    }

    return false;
}

void Relay::wateringOpenValve(char *valveId) {
    SerialFrame openValveFrame = SerialFrame(relayOnSerialCommand, valveId);
    AppSerial::sendFrame(&openValveFrame);
}

void Relay::wateringCloseValve(char *valveId) {
    SerialFrame closeValveFrame = SerialFrame(relayOffSerialCommand, valveId);
    AppSerial::sendFrame(&closeValveFrame);
}

void Relay::wateringOn() {
    SerialFrame waterFrame = SerialFrame(relayOnSerialCommand, "water");
    AppSerial::sendFrame(&waterFrame);
}

void Relay::wateringOff() {
    SerialFrame waterFrame = SerialFrame(relayOffSerialCommand, "water");
    AppSerial::sendFrame(&waterFrame);
}
