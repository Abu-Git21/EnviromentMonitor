#ifndef ENVIRO_MONITOR_H
#define ENVIRO_MONITOR_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SparkFun_SCD30_Arduino_Library.h>

#define TFT_CS 5
#define TFT_DC 2
#define TFT_RST 4

#define BUTTON_UP 32
#define BUTTON_DOWN 33
#define BUTTON_SELECT 25

#define I2C_SDA 21
#define I2C_SCL 22
#define LDR_PIN 34

#define LDR_DARK_RAW 300
#define LDR_BRIGHT_RAW 3500

#define BG 0x1082
#define DARK_GRAY 0x4208
#define GRAY 0x8410
#define LIGHT_GRAY 0xC618
#define WHITE ILI9341_WHITE
#define CYAN ILI9341_CYAN
#define GREEN ILI9341_GREEN
#define ORANGE 0xFD20
#define YELLOW ILI9341_YELLOW
#define RED ILI9341_RED

enum Page { ENVIRONMENT, CONNECTIVITY, SYSTEM_STATUS };
#define MENU_COUNT 3

extern const char* menuItems[MENU_COUNT];
extern Page currentPage;
extern int menuIndex;
extern bool inPage;

extern Adafruit_ILI9341 tft;
extern SCD30 airSensor;

extern float temperatureC;
extern float humidity;
extern float co2;
extern int lightRaw;
extern int lightPercent;
extern bool scd30Ready;

struct Button {
  int pin;
  bool lastReading;
  bool stableState;
  unsigned long lastChange;
};

extern Button upButton;
extern Button downButton;
extern Button selectButton;

bool pressed(Button &button);
void readLightSensor();
void drawHome();
void drawCurrentPage();
void drawEnvironment();
void drawConnectivity();
void drawSystemStatus();
void drawBar(int x, int y, int w, int h,
             float value, float maxValue, uint16_t color);

#endif
