#include <Arduino.h>

#include "def.h"
#include "Tools.h"
#include "AppTime.h"

static char megaUptimeString[10];

Tools::Tools() {}

Tools::~Tools() {}

void Tools::parseSerialCommand(const char *command, const char *param) {
    if (strcmp(command, "uptime") == 0) {
        strcpy(megaUptimeString, param);
    }
}

char *Tools::getUptime() {
    static char uptimeString[10];
    uint32_t overflowMillis = AppTime::getOverFlowCounter() * 4294968;
    const float uptime = (overflowMillis + millis()) / 1000.0L / 60.0L;
    dtostrf(uptime > 60 ? uptime / 60 : uptime, 3, 1, uptimeString);
    if (uptime > 60) {
        strcat(uptimeString, "h");
    } else {
        strcat(uptimeString, "m");
    }
    return uptimeString;
}

char *Tools::getCharArray(char *args[], int len) {
    static char result[50];
    strcpy(result, "");

    for (int i = 0; i < len; i++) {
        strcat(result, args[i]);
    }

    return result;
}

char *Tools::intToChar(int value) {
    static char result[10];
    int strLength = 1;
    if (value > 9999) {
        strLength = 5;
    } else if (value > 999) {
        strLength = 4;
    } else if (value > 99) {
        strLength = 3;
    } else if (value > 9) {
        strLength = 2;
    }
    dtostrf(value, strLength, 0, result);

    return result;
}

char *Tools::stringReplace(char *str, char *find, char *replace) {
    int len  = strlen(str);
    int lena = strlen(find);
    int lenb = strlen(replace);

    for (char *p = str; p = strstr(p, find); ++p) {
        if (lena != lenb) { // shift end as needed
            memmove(p + lenb, p + lena, len - (p - str) + lenb);
        }

        memcpy(p, replace, lenb);
    }
    return str;
}

bool Tools::millisOverflowIsClose() {
    static const unsigned long millisOverflow = 4294967295;
    static const unsigned long overflowFreeTime = 10UL * 1000UL; // ten seconds before and after overflow do nothing
    const unsigned long t = millis();

    return t <= millisOverflow && (t < overflowFreeTime || t > millisOverflow - overflowFreeTime);
}

char *Tools::getMegaUptime() {
    return megaUptimeString;
}

void Tools::megaRestart() {
    digitalWrite(MEGA_RESET_PIN, LOW);
    delay(10);
    digitalWrite(MEGA_RESET_PIN, HIGH);
}
