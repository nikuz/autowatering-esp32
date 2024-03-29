#ifndef Watering_h
#define Watering_h

#include <Arduino.h>

struct WateringIntVariable {
    int *var;
    const char *key;

    WateringIntVariable() {}

    WateringIntVariable(int *_var, const char *_key) : var(_var), key(_key) {}
};

struct WateringStringVariable {
    String *var;
    const char *key;

    WateringStringVariable() {}

    WateringStringVariable(String *_var, const char *_key) : var(_var), key(_key) {}
};

struct WateringTargetVariable {
    char *name;
    char *enabled;
    char *lastWateringVar;
    char *manualWateringVar;
    char *duration;
    bool notificationSent;
    struct tm notificationSentAt;
};

class Watering {
public:
    Watering();

    ~Watering();

    static void setVariable(int *var, const char *key);

    static void setVariable(String *var, const char *key);

    static int &getIntVariable(const char *key);

    static String &getStringVariable(const char *key);

    static void check();

    static void checkProgress();

    static void stop();
};

#endif /* Watering_h */
