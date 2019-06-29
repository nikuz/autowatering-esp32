#include <Arduino.h>

#include "def.h"
#include "Sensor.h"
#include "Tools.h"

int currentTemperature = 0;
int currentHumidity = 0;
unsigned int soilMoisture[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static SoilMoistureSensor soilMoistureSensors[] = {
    {SOIL_SENSOR_1, SOIL_SENSOR_1_MIN, SOIL_SENSOR_1_MAX},
    {SOIL_SENSOR_2, SOIL_SENSOR_2_MIN, SOIL_SENSOR_2_MAX},
    {SOIL_SENSOR_3, SOIL_SENSOR_3_MIN, SOIL_SENSOR_3_MAX},
    {SOIL_SENSOR_4, SOIL_SENSOR_4_MIN, SOIL_SENSOR_4_MAX},
    {SOIL_SENSOR_5, SOIL_SENSOR_5_MIN, SOIL_SENSOR_5_MAX},
    {SOIL_SENSOR_6, SOIL_SENSOR_6_MIN, SOIL_SENSOR_6_MAX},
    {SOIL_SENSOR_7, SOIL_SENSOR_7_MIN, SOIL_SENSOR_7_MAX},
    {SOIL_SENSOR_8, SOIL_SENSOR_8_MIN, SOIL_SENSOR_8_MAX},
};
int lightIntensity = 0;

Sensor::Sensor() {}

Sensor::~Sensor() {}

void Sensor::parseSerialCommand(const char *command, const char *param) {
    int value = atoi(param);
    if (value < 0) {
        value = 0;
    }

    if (strcmp(command, "sm1") == 0) {
        soilMoisture[0] = value;
    }
    if (strcmp(command, "sm2") == 0) {
        soilMoisture[1] = value;
    }
    if (strcmp(command, "sm3") == 0) {
        soilMoisture[2] = value;
    }
    if (strcmp(command, "sm4") == 0) {
        soilMoisture[3] = value;
    }
    if (strcmp(command, "sm5") == 0) {
        soilMoisture[4] = value;
    }
    if (strcmp(command, "sm6") == 0) {
        soilMoisture[5] = value;
    }
    if (strcmp(command, "sm7") == 0) {
        soilMoisture[6] = value;
    }
    if (strcmp(command, "sm8") == 0) {
        soilMoisture[7] = value;
    }
}

// soil

int Sensor::getSoilMoisture(int sensorId) {
    int value = soilMoisture[sensorId - 1];
    if (value) {
        const int varsLen = *(&soilMoistureSensors + 1) - soilMoistureSensors;
        for (int i = 0; i < varsLen; i++) {
            if (soilMoistureSensors[i].sensorId == sensorId) {
                value = map(value, soilMoistureSensors[i].min, soilMoistureSensors[i].max, 0, 100);
                if (value < 0) {
                    value = 0;
                } else if (value > 100) {
                    value = 100;
                }
            }
        }
    }

    return value;
}
