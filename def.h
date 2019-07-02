#define PRODUCTION 1    // production
//#define PRODUCTION 0    // development
//#define DEBUG 1

#define VERSION_ID "4"

#if PRODUCTION
#define VERSION_MARKER "P"
#else
#define VERSION_MARKER "D"
#endif

#define VERSION VERSION_MARKER VERSION_ID

#ifdef DEBUG
#define DEBUG_PRINT(x)    Serial.print(x)
#define DEBUG_PRINTLN(x)  Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

// sensors
#define SOIL_SENSOR_1 1
#define SOIL_SENSOR_1_MIN 572
#define SOIL_SENSOR_1_MAX 377

#define SOIL_SENSOR_2 2
#define SOIL_SENSOR_2_MIN 573
#define SOIL_SENSOR_2_MAX 380

#define SOIL_SENSOR_3 3
#define SOIL_SENSOR_3_MIN 573
#define SOIL_SENSOR_3_MAX 390

#define SOIL_SENSOR_4 4
#define SOIL_SENSOR_4_MIN 600
#define SOIL_SENSOR_4_MAX 272

#define SOIL_SENSOR_5 5
#define SOIL_SENSOR_5_MIN 560
#define SOIL_SENSOR_5_MAX 350

#define SOIL_SENSOR_6 6
#define SOIL_SENSOR_6_MIN 590
#define SOIL_SENSOR_6_MAX 262

#define SOIL_SENSOR_7 7
#define SOIL_SENSOR_7_MIN 570
#define SOIL_SENSOR_7_MAX 370

#define SOIL_SENSOR_8 8
#define SOIL_SENSOR_8_MIN 560
#define SOIL_SENSOR_8_MAX 414

#define MEGA_RESET_PIN 15
