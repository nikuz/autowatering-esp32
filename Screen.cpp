#include <Arduino.h>
#include <U8g2lib.h>

#include "def.h"
#include "Screen.h"
#include "AppTime.h"
#include "Tools.h"
#include "Sensor.h"

U8G2_SSD1327_WS_128X128_F_4W_SW_SPI u8g2(U8G2_R0, 18, 23, 5, 19, U8X8_PIN_NONE);

static const int centerOfScreen = 64;

static ScreenIntVariable intVariables[10];
static ScreenTargetVariable targetVariables[] = {
    {"s1Enabled", SOIL_SENSOR_1, 15, 0},
    {"s2Enabled", SOIL_SENSOR_2, 15, centerOfScreen},
    {"s3Enabled", SOIL_SENSOR_3, 40, 0},
    {"s4Enabled", SOIL_SENSOR_4, 40, centerOfScreen},
    {"s5Enabled", SOIL_SENSOR_5, 65, 0},
    {"s6Enabled", SOIL_SENSOR_6, 65, centerOfScreen},
    {"s7Enabled", SOIL_SENSOR_7, 90, 0},
    {"s8Enabled", SOIL_SENSOR_8, 90, centerOfScreen},
};
static int blankIntVariable = -1;

Screen::Screen() {}

Screen::~Screen() {}

void Screen::setVariable(int *var, const char *key) {
    int varsLen = *(&intVariables + 1) - intVariables;
    for (int i = 0; i < varsLen; i++) {
        if (!intVariables[i].key) {
            intVariables[i] = ScreenIntVariable(var, key);
            break;
        }
    }
}

int &Screen::getIntVariable(const char *key) {
    const int varsLen = *(&intVariables + 1) - intVariables;
    for (int i = 0; i < varsLen; i++) {
        if (intVariables[i].key == key) {
            return *intVariables[i].var;
        }
    }

    return blankIntVariable;
}

void Screen::initiate() {
    u8g2.begin();
    u8g2.enableUTF8Print();
    u8g2.clearDisplay();
    u8g2.setFont(u8g2_font_crox2cb_tr);

    const char loadingStr[] = "...";
    const u8g2_uint_t displayWidth = u8g2.getDisplayWidth();
    const u8g2_uint_t displayHeight = u8g2.getDisplayHeight();
    const int strWidth = u8g2.getStrWidth(loadingStr);

    u8g2.clearBuffer();
    u8g2.setCursor((displayWidth / 2) - (strWidth / 2), displayHeight / 2);
    u8g2.print(loadingStr);
    u8g2.sendBuffer();
}

void Screen::clearBuffer() {
    u8g2.clearBuffer();
}

void Screen::sendBuffer() {
    u8g2.sendBuffer();
}

void Screen::printSoilMoisture(int number, int value, int line, int row) {
    if (isnan(value)) {
        return;
    }
    u8g2.setFont(u8g2_font_8x13B_tr);

    static char numberStr[10];
    const char *percent = "%";

    strcpy(numberStr, Tools::intToChar(number));
    strcat(numberStr, ": ");
    u8g2.drawStr(row, line, numberStr);
    const int numberWidth = u8g2.getStrWidth(numberStr);

    const char *valueString = Tools::intToChar(value);
    u8g2.drawStr(row + numberWidth, line, valueString);
    const int valueWidth = u8g2.getStrWidth(valueString);
    u8g2.drawStr(row + numberWidth + valueWidth, line, percent);
}

void Screen::printAppVersion() {
    u8g2_uint_t displayWidth = u8g2.getDisplayWidth();
    u8g2_uint_t displayHeight = u8g2.getDisplayHeight();
    u8g2.setFont(u8g2_font_u8glib_4_tf);
    u8g2.setCursor(displayWidth - 15, displayHeight);
    u8g2.print(VERSION);
}

void Screen::printTime(struct tm localtime) {
    u8g2_uint_t displayHeight = u8g2.getDisplayHeight();

    u8g2.setFont(u8g2_font_6x10_tn);
    u8g2.setCursor(0, displayHeight);
    u8g2.print(AppTime::getTimeString(localtime, "%02u/%02u/%04u %02u:%02u"));
}

void Screen::printUptime() {
    u8g2_uint_t displayHeight = u8g2.getDisplayHeight();

    u8g2.setFont(u8g2_font_crox2cb_tr);
    u8g2.setCursor(0, displayHeight);
    u8g2.print(Tools::getUptime());
}

void Screen::refresh() {
    clearBuffer();
    int screenEnabled = Screen::getIntVariable("screenEnabled");

    if (screenEnabled == 1) {
        const int varsLen = *(&targetVariables + 1) - targetVariables;
        for (int i = 0; i < varsLen; i++) {
            if (Screen::getIntVariable(targetVariables[i].name) == 1) {
                printSoilMoisture(
                    i + 1,
                    Sensor::getSoilMoisture(targetVariables[i].sensorVar),
                    targetVariables[i].line,
                    targetVariables[i].row
                );
            }
        }

        printAppVersion();
        printUptime();
    }

    sendBuffer();
}
