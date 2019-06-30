#ifndef Screen_h
#define Screen_h

#include <Arduino.h>

struct ScreenIntVariable {
    int *var;
    const char *key;

    ScreenIntVariable() {}

    ScreenIntVariable(int *_var, const char *_key) : var(_var), key(_key) {}
};

struct ScreenTargetVariable {
    char *name;
    int sensorVar;
    int line;
    int row;
};

class Screen {
public:
    Screen();

    ~Screen();

    static void setVariable(int *var, const char *key);

    static int &getIntVariable(const char *key);

    static void initiate();

    static void clearBuffer();

    static void sendBuffer();

    static void printSoilMoisture(int number, int value, int line, int row);

    static void printAppVersion();

    static void printTime(struct tm localtime);

    static void printUptime();

    static void refresh();
};

#endif /* Screen_h */
