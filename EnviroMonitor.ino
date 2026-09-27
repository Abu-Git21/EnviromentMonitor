#include "EnviroMonitor.h"

// Global objects
Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);
SCD30 airSensor;

// Global state
const char* menuItems[MENU_COUNT] = {
  "Environment", "Connectivity", "System Status"
};

Page currentPage = ENVIRONMENT;
int menuIndex = 0;
bool inPage = false;

float temperatureC = NAN;
float humidity = NAN;
float co2 = NAN;
int lightRaw = 0;
int lightPercent = 0;
bool scd30Ready = false;

Button upButton = {BUTTON_UP, HIGH, HIGH, 0};
Button downButton = {BUTTON_DOWN, HIGH, HIGH, 0};
Button selectButton = {BUTTON_SELECT, HIGH, HIGH, 0};

// Setup
void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);

  analogReadResolution(12);
  analogSetPinAttenuation(LDR_PIN, ADC_11db);

  tft.begin();
  tft.setRotation(1);
  tft.setTextWrap(false);
  tft.fillScreen(BG);

  Wire.begin(I2C_SDA, I2C_SCL);
  scd30Ready = airSensor.begin();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  Serial.println(scd30Ready ? "SCD30 detected." : "SCD30 not detected.");
  drawHome();
}

// Main loop
void loop() {
  readLightSensor();

  if (scd30Ready && airSensor.dataAvailable()) {
    float temp = airSensor.getTemperature();
    float hum = airSensor.getHumidity();
    float carbonDioxide = airSensor.getCO2();

    if (!isnan(temp) && !isnan(hum) &&
        !isnan(carbonDioxide) && carbonDioxide > 0) {
      temperatureC = temp;
      humidity = hum;
      co2 = carbonDioxide;

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

  static unsigned long lastDisplayUpdate = 0;
  if (inPage && currentPage == ENVIRONMENT &&
      millis() - lastDisplayUpdate >= 1000) {
    drawEnvironment();
    lastDisplayUpdate = millis();
  }

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
  } else if (pressed(selectButton)) {
    inPage = false;
    drawHome();
  }
}

// Button debounce
bool pressed(Button &button) {
  bool reading = digitalRead(button.pin);
  unsigned long now = millis();

  if (reading != button.lastReading) {
    button.lastChange = now;
    button.lastReading = reading;
  }

  if (now - button.lastChange >= 35 &&
      reading != button.stableState) {
    button.stableState = reading;
    return button.stableState == LOW;
  }
  return false;
}

// LDR reading
void readLightSensor() {
  static unsigned long lastRead = 0;
  if (millis() - lastRead < 250) return;
  lastRead = millis();

  lightRaw = analogRead(LDR_PIN);

  if (LDR_BRIGHT_RAW <= LDR_DARK_RAW) {
    lightPercent = 0;
  } else {
    lightPercent = map(lightRaw, LDR_DARK_RAW,
                       LDR_BRIGHT_RAW, 0, 100);
    lightPercent = constrain(lightPercent, 0, 100);
  }
}

// Home menu
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
  tft.print("UP/DOWN: Navigate  SELECT: Enter");
}

void drawCurrentPage() {
  switch (currentPage) {
    case ENVIRONMENT: drawEnvironment(); break;
    case CONNECTIVITY: drawConnectivity(); break;
    case SYSTEM_STATUS: drawSystemStatus(); break;
  }
}

// Bar graph
void drawBar(int x, int y, int w, int h,
             float value, float maxValue, uint16_t color) {
  tft.fillRoundRect(x, y, w, h, 5, DARK_GRAY);
  if (isnan(value) || maxValue <= 0 || value < 0) return;

  float fraction = constrain(value / maxValue, 0.0f, 1.0f);
  int fillWidth = (int)(fraction * w);
  if (fillWidth > 0) tft.fillRoundRect(x, y, fillWidth, h, 5, color);
}

// Environment screen
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

  // Temperature
  tft.setTextColor(WHITE);
  tft.setCursor(15, 50);
  tft.print("TEMPERATURE");
  float tempF = NAN;
  tft.setTextSize(2);
  tft.setCursor(215, 46);
  if (!isnan(temperatureC)) {
    tempF = temperatureC * 9.0 / 5.0 + 32.0;
    tft.setTextColor(ORANGE);
    tft.print(tempF, 1);
    tft.print(" F");
  } else {
    tft.setTextColor(RED);
    tft.print("--.- F");
  }
  drawBar(15, 70, 285, 12, tempF, 100, ORANGE);

  // Humidity
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
    tft.print("----");
  } else {
    tft.setTextColor(GREEN);
    tft.print((int)round(co2));
  }
  tft.setTextSize(1);
  tft.setCursor(270, 141);
  tft.print("ppm");
  drawBar(15, 158, 285, 12, co2, 2000, GREEN);

  // Light level
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

  tft.setTextSize(1);
  tft.setTextColor(scd30Ready ? GREEN : RED);
  tft.setCursor(15, 225);
  tft.print(scd30Ready ? "SCD30: CONNECTED" : "SCD30: NOT FOUND");
  tft.setTextColor(LIGHT_GRAY);
  tft.setCursor(207, 225);
  tft.print("SELECT: BACK");
}

// Connectivity screen
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
  tft.setTextColor(scd30Ready ? GREEN : RED);
  tft.print(scd30Ready ? "READY" : "ERROR");

  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(90, 220);
  tft.print("SELECT = RETURN");
}

// System status screen
void drawSystemStatus() {
  tft.fillScreen(BG);

  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(75, 15);
  tft.print("SYSTEM STATUS");

  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(25, 65);
  tft.print("ESP32");
  tft.setTextSize(2);
  tft.setTextColor(GREEN);
  tft.setCursor(190, 60);
  tft.print("ONLINE");

  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(25, 105);
  tft.print("SCD30");
  tft.setTextSize(2);
  tft.setCursor(190, 100);
  tft.setTextColor(scd30Ready ? GREEN : RED);
  tft.print(scd30Ready ? "READY" : "ERROR");

  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(25, 145);
  tft.print("LDR");
  tft.setTextSize(2);
  tft.setTextColor(GREEN);
  tft.setCursor(190, 140);
  tft.print("ACTIVE");

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
