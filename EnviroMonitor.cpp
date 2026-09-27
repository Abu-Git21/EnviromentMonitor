
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SparkFun_SCD30_Arduino_Library.h>

// ==================================================
// PIN CONFIGURATION
// ==================================================

// TFT
#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4

// Buttons (connect each button to GND)
#define BUTTON_UP      32
#define BUTTON_DOWN    33
#define BUTTON_SELECT  25

// Sensors
#define I2C_SDA 21
#define I2C_SCL 22
#define LDR_PIN 34

// ==================================================
// LDR CALIBRATION
// Adjust these after testing your actual LDR.
//
// With the LDR connected to 3.3V and a 10k
// resistor connected to GND, brighter light
// normally produces a higher ADC reading.
// ==================================================

#define LDR_DARK_RAW   300
#define LDR_BRIGHT_RAW 3500

// ==================================================
// OBJECTS
// ==================================================

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);
SCD30 airSensor;

// ==================================================
// COLORS
// ==================================================

#define BG          0x1082
#define DARK_GRAY   0x4208
#define GRAY        0x8410
#define LIGHT_GRAY  0xC618
#define WHITE       ILI9341_WHITE
#define CYAN        ILI9341_CYAN
#define GREEN       ILI9341_GREEN
#define ORANGE      0xFD20
#define YELLOW      ILI9341_YELLOW
#define RED         ILI9341_RED

// ==================================================
// SENSOR DATA
// ==================================================

float temperatureC = NAN;
float humidity = NAN;
float co2 = NAN;

int lightRaw = 0;
int lightPercent = 0;

bool scd30Ready = false;
bool newReading = false;

// ==================================================
// MENU
// ==================================================

enum Page {
  ENVIRONMENT,
  CONNECTIVITY,
  SYSTEM_STATUS
};

Page currentPage = ENVIRONMENT;

const char* menuItems[] = {
  "Environment",
  "Connectivity",
  "System Status"
};

const int MENU_COUNT = 3;
int menuIndex = 0;
bool inPage = false;

// ==================================================
// BUTTON DEBOUNCING
// ==================================================

struct Button {
  int pin;
  bool lastReading;
  bool stableState;
  unsigned long lastChange;
};

Button upButton     = {BUTTON_UP, HIGH, HIGH, 0};
Button downButton   = {BUTTON_DOWN, HIGH, HIGH, 0};
Button selectButton = {BUTTON_SELECT, HIGH, HIGH, 0};

bool pressed(Button &button) {
  bool reading = digitalRead(button.pin);
  unsigned long now = millis();

  if (reading != button.lastReading) {
    button.lastChange = now;
    button.lastReading = reading;
  }

  if (now - button.lastChange >= 35) {
    if (reading != button.stableState) {
      button.stableState = reading;

      // INPUT_PULLUP: LOW means pressed.
      if (button.stableState == LOW) {
        return true;
      }
    }
  }

  return false;
}

// ==================================================
// SETUP
// ==================================================

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);

  analogReadResolution(12);
  analogSetPinAttenuation(LDR_PIN, ADC_11db);

  // Initialize display
  tft.begin();
  tft.setRotation(1);
  tft.setTextWrap(false);
  tft.fillScreen(BG);

  // Initialize I2C and SCD30
  Wire.begin(I2C_SDA, I2C_SCL);
  scd30Ready = airSensor.begin();

  // WiFi is not connected unless configured.
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (scd30Ready) {
    Serial.println("SCD30 detected.");
  } else {
    Serial.println("SCD30 not detected.");
  }

  drawHome();
}

// ==================================================
// MAIN LOOP
// ==================================================

void loop() {
  readLightSensor();

  // Read SCD30 whenever new data is available.
  if (scd30Ready && airSensor.dataAvailable()) {
    float newTemp = airSensor.getTemperature();
    float newHumidity = airSensor.getHumidity();
    float newCO2 = airSensor.getCO2();

    if (!isnan(newTemp) && !isnan(newHumidity) &&
        !isnan(newCO2) && newCO2 > 0) {
      temperatureC = newTemp;
      humidity = newHumidity;
      co2 = newCO2;
      newReading = true;

      Serial.print("Temperature: ");
      Serial.print(temperatureC);
      Serial.println(" C");

      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.println(" %");

      Serial.print("CO2: ");
      Serial.print(co2);
      Serial.println(" ppm");
    }
  }

  // Update the environment screen with live readings.
  static unsigned long lastDisplayUpdate = 0;
  unsigned long now = millis();

  if (currentPage == ENVIRONMENT &&
      inPage &&
      now - lastDisplayUpdate >= 1000) {
    drawEnvironment();
    lastDisplayUpdate = now;
  }

  // Button navigation
  if (!inPage) {
    if (pressed(upButton)) {
      menuIndex = (menuIndex - 1 + MENU_COUNT) % MENU_COUNT;
      drawHome();
    }

    if (pressed(downButton)) {
      menuIndex = (menuIndex + 1) % MENU_COUNT;
      drawHome();
    }

    if (pressed(selectButton)) {
      inPage = true;
      currentPage = (Page)menuIndex;
      drawCurrentPage();
    }
  } else {
    if (pressed(selectButton)) {
      inPage = false;
      drawHome();
    }
  }
}

// ==================================================
// LIGHT SENSOR
// ==================================================

void readLightSensor() {
  static unsigned long lastLightRead = 0;

  if (millis() - lastLightRead < 250) {
    return;
  }

  lastLightRead = millis();

  lightRaw = analogRead(LDR_PIN);

  int low = LDR_DARK_RAW;
  int high = LDR_BRIGHT_RAW;

  if (high <= low) {
    lightPercent = 0;
  } else {
    lightPercent = map(lightRaw, low, high, 0, 100);
    lightPercent = constrain(lightPercent, 0, 100);
  }
}

// ==================================================
// HOME MENU
// ==================================================

void drawHome() {
  tft.fillScreen(BG);

  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(40, 18);
  tft.print("EnviroMonitor");

  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(120, 45);
  tft.print("MAIN MENU");

  for (int i = 0; i < MENU_COUNT; i++) {
    int y = 80 + i * 48;

    if (i == menuIndex) {
      tft.fillRoundRect(25, y, 270, 38, 8, CYAN);
      tft.setTextColor(BG);
    } else {
      tft.drawRoundRect(25, y, 270, 38, 8, GRAY);
      tft.setTextColor(WHITE);
    }

    tft.setTextSize(2);
    tft.setCursor(45, y + 10);
    tft.print(menuItems[i]);
  }

  tft.setTextSize(1);
  tft.setTextColor(LIGHT_GRAY);
  tft.setCursor(58, 230);
  tft.print("UP/DOWN: Navigate   SELECT: Enter");
}

// ==================================================
// DRAW CURRENT PAGE
// ==================================================

void drawCurrentPage() {
  switch (currentPage) {
    case ENVIRONMENT:
      drawEnvironment();
      break;

    case CONNECTIVITY:
      drawConnectivity();
      break;

    case SYSTEM_STATUS:
      drawSystemStatus();
      break;
  }
}

// ==================================================
// BAR GRAPH
// ==================================================

void drawBar(int x, int y, int w, int h,
             float value, float maxValue, uint16_t color) {
  tft.fillRoundRect(x, y, w, h, 5, DARK_GRAY);

  if (isnan(value) || maxValue <= 0 || value < 0) {
    return;
  }

  float fraction = value / maxValue;
  fraction = constrain(fraction, 0.0f, 1.0f);

  int fillWidth = (int)(fraction * w);

  if (fillWidth > 0) {
    tft.fillRoundRect(x, y, fillWidth, h, 5, color);
  }
}

// ==================================================
// ENVIRONMENT SCREEN
// ==================================================

void drawEnvironment() {
  tft.fillScreen(BG);

  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(83, 8);
  tft.print("ENVIRONMENT");

  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(111, 30);
  tft.print("LIVE SENSOR DATA");

  // TEMPERATURE
  tft.setTextColor(WHITE);
  tft.setCursor(15, 50);
  tft.print("TEMPERATURE");

  tft.setTextSize(2);
  tft.setCursor(215, 46);

  if (isnan(temperatureC)) {
    tft.setTextColor(RED);
    tft.print("--.- F");
  } else {
    float temperatureF = temperatureC * 9.0 / 5.0 + 32.0;
    tft.setTextColor(ORANGE);
    tft.print(temperatureF, 1);
    tft.print(" F");
  }

  float tempF = isnan(temperatureC) ? -1 :
                temperatureC * 9.0 / 5.0 + 32.0;

  drawBar(15, 70, 285, 12, tempF, 100, ORANGE);

  // HUMIDITY
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(15, 94);
  tft.print("HUMIDITY");

  tft.setTextSize(2);
  tft.setCursor(230, 90);

  if (isnan(humidity)) {
    tft.setTextColor(RED);
    tft.print("-- %");
  } else {
    tft.setTextColor(CYAN);
    tft.print(humidity, 1);
    tft.print("%");
  }

  drawBar(15, 114, 285, 12, humidity, 100, CYAN);

  // CO2
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(15, 138);
  tft.print("CO2 LEVEL");

  tft.setTextSize(2);
  tft.setCursor(190, 134);

  if (isnan(co2)) {
    tft.setTextColor(RED);
    tft.print("---- ppm");
  } else {
    tft.setTextColor(GREEN);
    tft.print((int)round(co2));
    tft.setTextSize(1);
    tft.setCursor(275, 141);
    tft.print("ppm");
  }

  drawBar(15, 158, 285, 12, co2, 2000, GREEN);

  // LIGHT LEVEL
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(15, 182);
  tft.print("LIGHT LEVEL");

  tft.setTextSize(2);
  tft.setTextColor(YELLOW);
  tft.setCursor(230, 178);
  tft.print(lightPercent);
  tft.print("%");

  drawBar(15, 202, 285, 12, lightPercent, 100, YELLOW);

  // SENSOR STATUS
  tft.setTextSize(1);
  tft.setTextColor(scd30Ready ? GREEN : RED);
  tft.setCursor(15, 225);
  tft.print(scd30Ready ? "SCD30: CONNECTED" : "SCD30: NOT FOUND");

  tft.setTextColor(LIGHT_GRAY);
  tft.setCursor(207, 225);
  tft.print("SELECT: BACK");
}

// ==================================================
// CONNECTIVITY SCREEN
// ==================================================

void drawConnectivity() {
  tft.fillScreen(BG);

  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(75, 15);
  tft.print("CONNECTIVITY");

  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(25, 70);
  tft.print("ESP32");

  tft.setTextSize(2);
  tft.setTextColor(GREEN);
  tft.setCursor(190, 65);
  tft.print("ONLINE");

  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(25, 110);
  tft.print("WiFi");

  tft.setTextSize(2);
  tft.setCursor(190, 105);

  if (WiFi.status() == WL_CONNECTED) {
    tft.setTextColor(GREEN);
    tft.print("CONNECTED");
  } else {
    tft.setTextColor(GRAY);
    tft.print("OFFLINE");
  }

  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(25, 150);
  tft.print("I2C SENSOR");

  tft.setTextSize(2);
  tft.setCursor(190, 145);

  if (scd30Ready) {
    tft.setTextColor(GREEN);
    tft.print("READY");
  } else {
    tft.setTextColor(RED);
    tft.print("ERROR");
  }

  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(90, 220);
  tft.print("SELECT = RETURN");
}

// ==================================================
// SYSTEM STATUS SCREEN
// ==================================================

void drawSystemStatus() {
  tft.fillScreen(BG);

  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(75, 15);
  tft.print("SYSTEM STATUS");

  // ESP32
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(25, 65);
  tft.print("ESP32");

  tft.setTextSize(2);
  tft.setTextColor(GREEN);
  tft.setCursor(190, 60);
  tft.print("ONLINE");

  // SCD30
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(25, 105);
  tft.print("SCD30");

  tft.setTextSize(2);
  tft.setCursor(190, 100);
  tft.setTextColor(scd30Ready ? GREEN : RED);
  tft.print(scd30Ready ? "READY" : "ERROR");

  // LDR
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(25, 145);
  tft.print("LDR");

  tft.setTextSize(2);
  tft.setTextColor(GREEN);
  tft.setCursor(190, 140);
  tft.print("ACTIVE");

  // TFT
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(25, 185);
  tft.print("DISPLAY");

  tft.setTextSize(2);
  tft.setTextColor(GREEN);
  tft.setCursor(190, 180);
  tft.print("ACTIVE");

  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(90, 220);
  tft.print("SELECT = RETURN");
}
