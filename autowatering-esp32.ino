//#include <EspOta.h>

#include "def.h"
#include "AppWiFi.h"
#include "AppStorage.h"
#include "AppTime.h"
#include "Sensor.h"
#include "Tools.h"
#include "Screen.h"
#include "Relay.h"
#include "AppSerial.h"
//#include "AppBlynk.h"
//#include "Watering.h"

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
//EspOta otaUpdate(otaHost, otaPort, otaBin, TAG, true);
#else
//EspOta otaUpdate(otaHost, otaPort, otaBin, TAG);
#endif
const unsigned long otaCheckUpdateInterval = 60UL * 1000UL;  // check OTA update every minute

const unsigned long screenRefreshInterval = 2UL * 1000UL; // refresh screen every 2 seconds

// watering
const unsigned long wateringInterval = 5UL * 1000UL; // check every 5 seconds
const unsigned long wateringProgressCheckInterval = 1UL * 1000UL;  // check every second
int autoWatering = 0; // auto watering disabled by default
int wSoilMstrMin = 30;
String lastWatering = "";

const unsigned long blynkSyncInterval = 2UL * 1000UL;  // sync blynk state every second
const unsigned long blynkCheckConnectInterval = 30UL * 1000UL;  // check blynk connection every 30 seconds

const unsigned long uptimePrintInterval = 1UL * 1000UL;

//void otaUpdateHandler() {
//    if (Tools::millisOverflowIsClose()) {
//        return;
//    }
//    if (otaUpdate._host != otaHost || otaUpdate._bin != otaBin) {
//        otaUpdate.updateEntries(otaHost, otaPort, otaBin);
//    }
//    otaUpdate.begin(AppTime::getTimeString(AppTime::getCurrentTime()));
//}

void checkMoisture() {
    Serial.print("Soil moisture: ");
    Serial.println(Sensor::getSoilMoisture(1));
    if (Sensor::getSoilMoisture(1) < wSoilMstrMin) {
        Relay::wateringOpenValve("s1");
    } else {
        Relay::wateringCloseValve("s1");
    }
}

void setup() {
    // initiate screen first to show loading state
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

    // initially off all the loads
//    Relay::wateringCloseValve();
//    Relay::wateringOff();

    // restore preferences
    AppStorage::setVariable(&otaHost, "otaHost");
    AppStorage::setVariable(&otaBin, "otaBin");
    AppStorage::setVariable(&wSoilMstrMin, "wSoilMstrMin");
    AppStorage::setVariable(&autoWatering, "autoWatering");
    AppStorage::setVariable(&lastWatering, "lastWatering");
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

    Relay::wateringCloseValve("s1");
    Relay::wateringCloseValve("s2");
    Relay::wateringCloseValve("s3");
    Relay::wateringCloseValve("s4");
    Relay::wateringCloseValve("s5");
    Relay::wateringCloseValve("s6");
    Relay::wateringCloseValve("s7");
    Relay::wateringCloseValve("s8");

//    Watering::setVariable(&autoWatering, "autoWatering");
//    Watering::setVariable(&wSoilMstrMin, "wSoilMstrMin");
//    Watering::setVariable(&lastWatering, "lastWatering");

    // register Blynk variables
//    AppBlynk::setVariable(&otaHost, "otaHost");
//    AppBlynk::setVariable(&otaBin, "otaBin");
//    AppBlynk::setVariable(&wSoilMstrMin, "wSoilMstrMin");
//    AppBlynk::setVariable(&autoWatering, "autoWatering");

    // start Blynk connection
//    AppBlynk::initiate();

//    timer.setInterval("watering", wateringInterval, Watering::check);
//    timer.setInterval("wateringProgress", wateringProgressCheckInterval, Watering::checkProgress);
    timer.setInterval("testWatering", wateringProgressCheckInterval, checkMoisture);
    timer.setInterval("screenRefresh", screenRefreshInterval, Screen::refresh);
//    timer.setInterval("ota", otaCheckUpdateInterval, otaUpdateHandler);
//    timer.setInterval("blynkCheckConnect", blynkCheckConnectInterval, AppBlynk::checkConnect);
//    timer.setInterval("blynkSync", blynkSyncInterval, AppBlynk::sync);
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

//    AppBlynk::run();

    timer.run();
}
