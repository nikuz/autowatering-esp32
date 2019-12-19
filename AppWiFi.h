#ifndef AppWiFi_h
#define AppWiFi_h

#include <Arduino.h>

struct WifiStringVariable {
    String *var;
    const char *key;

    WifiStringVariable() {}

    WifiStringVariable(String *_var, const char *_key) : var(_var), key(_key) {}
};

class AppWiFi {
public:
    AppWiFi();

    ~AppWiFi();

    static void setVariable(String *var, const char *key);

    static String &getStringVariable(const char *key);

    static void connect();

    static bool isConnected();
};

#endif /* AppWiFi_h */
