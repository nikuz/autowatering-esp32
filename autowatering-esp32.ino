#include <EspOta.h>

#include "def.h"
#include "AppWiFi.h"
#include "AppStorage.h"
#include "AppTime.h"
#include "Sensor.h"
#include "Tools.h"
#include "Screen.h"
#include "Relay.h"
#include "AppSerial.h"
#include "AppBlynk.h"
#include "Watering.h"

static const char *TAG = "autowatering";
AppTime timer;

// OTA settings
String otaHost = "selfproduct.com";
const int otaPort = 80;  // Non https. For HTTPS 443. As of today, HTTPS doesn't work.
#if PRODUCTION
String otaBin = "/esp32-updates/autowatering.bin";
#else
String otaBin = "/esp32-updates/autowatering-dev.bin";
#endif
#ifdef DEBUG
EspOta otaUpdate(otaHost, otaPort, otaBin, TAG, true);
#else
EspOta otaUpdate(otaHost, otaPort, otaBin, TAG);
#endif
const unsigned long otaCheckUpdateInterval = 60UL * 1000UL;  // check OTA update every minute

const unsigned long screenRefreshInterval = 2UL * 1000UL; // refresh screen every 2 seconds
int screenEnabled = 1;

// watering
const unsigned long wateringInterval = 5UL * 1000UL; // check every 5 seconds
const unsigned long wateringProgressCheckInterval = 1UL * 1000UL;  // check every second
int autoWatering = 0;   // auto watering disabled by default
int wSoilMstrMin = 30;
int wInterval = 5;
int s1Enabled = 0;
int s2Enabled = 0;
int s3Enabled = 0;
int s4Enabled = 0;
int s5Enabled = 0;
int s6Enabled = 0;
int s7Enabled = 0;
int s8Enabled = 0;
int s1MnlWtrng = 0;
int s2MnlWtrng = 0;
int s3MnlWtrng = 0;
int s4MnlWtrng = 0;
int s5MnlWtrng = 0;
int s6MnlWtrng = 0;
int s7MnlWtrng = 0;
int s8MnlWtrng = 0;
int s1WtrngDur = 5;      // 5 sec
int s2WtrngDur = 5;      // 5 sec
int s3WtrngDur = 5;      // 5 sec
int s4WtrngDur = 5;      // 5 sec
int s5WtrngDur = 5;      // 5 sec
int s6WtrngDur = 5;      // 5 sec
int s7WtrngDur = 5;      // 5 sec
int s8WtrngDur = 5;      // 5 sec
String s1LstWtrng = "";
String s2LstWtrng = "";
String s3LstWtrng = "";
String s4LstWtrng = "";
String s5LstWtrng = "";
String s6LstWtrng = "";
String s7LstWtrng = "";
String s8LstWtrng = "";

const unsigned long blynkSyncInterval = 2UL * 1000UL;  // sync blynk state every second
const unsigned long blynkCheckConnectInterval = 30UL * 1000UL;  // check blynk connection every 30 seconds

const unsigned long uptimePrintInterval = 1UL * 1000UL;

void otaUpdateHandler() {
    if (Tools::millisOverflowIsClose()) {
        return;
    }
    if (otaUpdate._host != otaHost || otaUpdate._bin != otaBin) {
        otaUpdate.updateEntries(otaHost, otaPort, otaBin);
    }
    otaUpdate.begin(AppTime::getTimeString(AppTime::getCurrentTime()));
}

void setup() {
    // initiate screen first to show loading state
    Screen::setVariable(&s1Enabled, "s1Enabled");
    Screen::setVariable(&s2Enabled, "s2Enabled");
    Screen::setVariable(&s3Enabled, "s3Enabled");
    Screen::setVariable(&s4Enabled, "s4Enabled");
    Screen::setVariable(&s5Enabled, "s5Enabled");
    Screen::setVariable(&s6Enabled, "s6Enabled");
    Screen::setVariable(&s7Enabled, "s7Enabled");
    Screen::setVariable(&s8Enabled, "s8Enabled");
    Screen::setVariable(&screenEnabled, "screenEnabled");
    Screen::initiate();

    // Begin debug Serial
    Serial.begin(115200);
    while (!Serial) {
        ;
    }

    // Begin Mega communication Serial
    Serial2.begin(115200);
    while (!Serial2) {
        ;
    }

    pinMode(MEGA_RESET_PIN, OUTPUT);

    // initially off all the loads
    for (int i = 1; i <= 8; i++) {
        char command[] = "s";
        char *commandEnd = Tools::intToChar(i);
        strcat(command, commandEnd);
        Relay::wateringCloseValve(command);
    }

    // restore preferences
    AppStorage::setVariable(&otaHost, "otaHost");
    AppStorage::setVariable(&otaBin, "otaBin");
    AppStorage::setVariable(&screenEnabled, "screenEnabled");
    AppStorage::setVariable(&wSoilMstrMin, "wSoilMstrMin");
    AppStorage::setVariable(&autoWatering, "autoWatering");
    AppStorage::setVariable(&s1LstWtrng, "s1LstWtrng");
    AppStorage::setVariable(&s2LstWtrng, "s2LstWtrng");
    AppStorage::setVariable(&s3LstWtrng, "s3LstWtrng");
    AppStorage::setVariable(&s4LstWtrng, "s4LstWtrng");
    AppStorage::setVariable(&s5LstWtrng, "s5LstWtrng");
    AppStorage::setVariable(&s6LstWtrng, "s6LstWtrng");
    AppStorage::setVariable(&s7LstWtrng, "s7LstWtrng");
    AppStorage::setVariable(&s8LstWtrng, "s8LstWtrng");
    AppStorage::setVariable(&s1WtrngDur, "s1WtrngDur");
    AppStorage::setVariable(&s2WtrngDur, "s2WtrngDur");
    AppStorage::setVariable(&s3WtrngDur, "s3WtrngDur");
    AppStorage::setVariable(&s4WtrngDur, "s4WtrngDur");
    AppStorage::setVariable(&s5WtrngDur, "s5WtrngDur");
    AppStorage::setVariable(&s6WtrngDur, "s6WtrngDur");
    AppStorage::setVariable(&s7WtrngDur, "s7WtrngDur");
    AppStorage::setVariable(&s8WtrngDur, "s8WtrngDur");
    AppStorage::setVariable(&s1Enabled, "s1Enabled");
    AppStorage::setVariable(&s2Enabled, "s2Enabled");
    AppStorage::setVariable(&s3Enabled, "s3Enabled");
    AppStorage::setVariable(&s4Enabled, "s4Enabled");
    AppStorage::setVariable(&s5Enabled, "s5Enabled");
    AppStorage::setVariable(&s6Enabled, "s6Enabled");
    AppStorage::setVariable(&s7Enabled, "s7Enabled");
    AppStorage::setVariable(&s8Enabled, "s8Enabled");
    AppStorage::restore();

    // setup wifi ip address etc.
    AppWiFi::connect();

    //get internet time
    AppTime::obtainSNTP();

    // update RTC time on Mega by internet time
    struct tm ntpTime = {0};
    if (AppTime::localTime(&ntpTime)) {
        const char *timeParam = AppTime::getTimeString(ntpTime);
        SerialFrame timeFrame = SerialFrame("time", timeParam);
        AppSerial::sendFrame(&timeFrame);
    }

    Watering::setVariable(&autoWatering, "autoWatering");
    Watering::setVariable(&wSoilMstrMin, "wSoilMstrMin");
    Watering::setVariable(&wInterval, "wInterval");
    Watering::setVariable(&s1MnlWtrng, "s1MnlWtrng");
    Watering::setVariable(&s2MnlWtrng, "s2MnlWtrng");
    Watering::setVariable(&s3MnlWtrng, "s3MnlWtrng");
    Watering::setVariable(&s4MnlWtrng, "s4MnlWtrng");
    Watering::setVariable(&s5MnlWtrng, "s5MnlWtrng");
    Watering::setVariable(&s6MnlWtrng, "s6MnlWtrng");
    Watering::setVariable(&s7MnlWtrng, "s7MnlWtrng");
    Watering::setVariable(&s8MnlWtrng, "s8MnlWtrng");
    Watering::setVariable(&s1LstWtrng, "s1LstWtrng");
    Watering::setVariable(&s2LstWtrng, "s2LstWtrng");
    Watering::setVariable(&s3LstWtrng, "s3LstWtrng");
    Watering::setVariable(&s4LstWtrng, "s4LstWtrng");
    Watering::setVariable(&s5LstWtrng, "s5LstWtrng");
    Watering::setVariable(&s6LstWtrng, "s6LstWtrng");
    Watering::setVariable(&s7LstWtrng, "s7LstWtrng");
    Watering::setVariable(&s8LstWtrng, "s8LstWtrng");
    Watering::setVariable(&s1WtrngDur, "s1WtrngDur");
    Watering::setVariable(&s2WtrngDur, "s2WtrngDur");
    Watering::setVariable(&s3WtrngDur, "s3WtrngDur");
    Watering::setVariable(&s4WtrngDur, "s4WtrngDur");
    Watering::setVariable(&s5WtrngDur, "s5WtrngDur");
    Watering::setVariable(&s6WtrngDur, "s6WtrngDur");
    Watering::setVariable(&s7WtrngDur, "s7WtrngDur");
    Watering::setVariable(&s8WtrngDur, "s8WtrngDur");
    Watering::setVariable(&s1Enabled, "s1Enabled");
    Watering::setVariable(&s2Enabled, "s2Enabled");
    Watering::setVariable(&s3Enabled, "s3Enabled");
    Watering::setVariable(&s4Enabled, "s4Enabled");
    Watering::setVariable(&s5Enabled, "s5Enabled");
    Watering::setVariable(&s6Enabled, "s6Enabled");
    Watering::setVariable(&s7Enabled, "s7Enabled");
    Watering::setVariable(&s8Enabled, "s8Enabled");

    // register Blynk variables
    AppBlynk::setVariable(&otaHost, "otaHost");
    AppBlynk::setVariable(&otaBin, "otaBin");
    AppBlynk::setVariable(&screenEnabled, "screenEnabled");
    AppBlynk::setVariable(&wSoilMstrMin, "wSoilMstrMin");
    AppBlynk::setVariable(&autoWatering, "autoWatering");
    AppBlynk::setVariable(&wInterval, "wInterval");
    AppBlynk::setVariable(&s1MnlWtrng, "s1MnlWtrng");
    AppBlynk::setVariable(&s2MnlWtrng, "s2MnlWtrng");
    AppBlynk::setVariable(&s3MnlWtrng, "s3MnlWtrng");
    AppBlynk::setVariable(&s4MnlWtrng, "s4MnlWtrng");
    AppBlynk::setVariable(&s5MnlWtrng, "s5MnlWtrng");
    AppBlynk::setVariable(&s6MnlWtrng, "s6MnlWtrng");
    AppBlynk::setVariable(&s7MnlWtrng, "s7MnlWtrng");
    AppBlynk::setVariable(&s8MnlWtrng, "s8MnlWtrng");
    AppBlynk::setVariable(&s1WtrngDur, "s1WtrngDur");
    AppBlynk::setVariable(&s2WtrngDur, "s2WtrngDur");
    AppBlynk::setVariable(&s3WtrngDur, "s3WtrngDur");
    AppBlynk::setVariable(&s4WtrngDur, "s4WtrngDur");
    AppBlynk::setVariable(&s5WtrngDur, "s5WtrngDur");
    AppBlynk::setVariable(&s6WtrngDur, "s6WtrngDur");
    AppBlynk::setVariable(&s7WtrngDur, "s7WtrngDur");
    AppBlynk::setVariable(&s8WtrngDur, "s8WtrngDur");
    AppBlynk::setVariable(&s1Enabled, "s1Enabled");
    AppBlynk::setVariable(&s2Enabled, "s2Enabled");
    AppBlynk::setVariable(&s3Enabled, "s3Enabled");
    AppBlynk::setVariable(&s4Enabled, "s4Enabled");
    AppBlynk::setVariable(&s5Enabled, "s5Enabled");
    AppBlynk::setVariable(&s6Enabled, "s6Enabled");
    AppBlynk::setVariable(&s7Enabled, "s7Enabled");
    AppBlynk::setVariable(&s8Enabled, "s8Enabled");

    // start Blynk connection
    AppBlynk::initiate();

    timer.setInterval("watering", wateringInterval, Watering::check);
    timer.setInterval("wateringProgress", wateringProgressCheckInterval, Watering::checkProgress);
    timer.setInterval("screenRefresh", screenRefreshInterval, Screen::refresh);
    timer.setInterval("ota", otaCheckUpdateInterval, otaUpdateHandler);
    timer.setInterval("blynkCheckConnect", blynkCheckConnectInterval, AppBlynk::checkConnect);
    timer.setInterval("blynkSync", blynkSyncInterval, AppBlynk::sync);

    Tools::megaRestart();
}

void loop() {
    // to have ability to write serial commands manually
    while (Serial.available() > 0) {
        Serial2.write(char(Serial.read()));
    }
    // read serial data from Mega
    SerialFrame serialFrame = AppSerial::getFrame();
    if (strcmp(serialFrame.command, "") != 0) {
        AppTime::parseSerialCommand(serialFrame.command, serialFrame.param);
        Sensor::parseSerialCommand(serialFrame.command, serialFrame.param);
        Relay::parseSerialCommand(serialFrame.command, serialFrame.param);
        Tools::parseSerialCommand(serialFrame.command, serialFrame.param);
    }

    AppBlynk::run();

    timer.run();
}
