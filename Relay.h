#ifndef Relay_h
#define Relay_h

#include <Arduino.h>

class Relay {
public:
    Relay();

    ~Relay();

    static void parseSerialCommand(const char *command, const char *param);

    // watering
    static bool isWateringOn();

    static bool wateringValveIsOpen(char *valveId);

    static void wateringOpenValve(char *valveId);

    static void wateringCloseValve(char *valveId);

    static void wateringOn();

    static void wateringOff();
};

#endif /* Relay_h */
