#ifndef Relay_h
#define Relay_h

#include <Arduino.h>

class Relay {
public:
    Relay();

    ~Relay();

    static void parseSerialCommand(const char *command, const char *param);

    // watering
    static bool isWateringEnabled();

    static bool IsValveOpenedFor(char *valveId);

    static void wateringOn(char *valveId);

    static void wateringOff(char *valveId);
};

#endif /* Relay_h */
