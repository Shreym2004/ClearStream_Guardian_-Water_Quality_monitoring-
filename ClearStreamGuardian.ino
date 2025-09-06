#include <WiFi.h>
//#define USE_BLYNK

#ifdef USE_BLYNK
  #include <BlynkSimpleEsp32.h>
  char auth[] = "YOUR_BLYNK_TOKEN";
  const char* ssid = "YOUR_SSID";
  const char* pass = "YOUR_PASS";
#endif

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ONE_WIRE_BUS 4
#define PIN_PH 34
#define PIN_TDS 35
#define PIN_TURB 32

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

const float ADC_MAX = 4095.0;
const float VREF = 3.3;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float PH_VOLTAGE_OFFSET = 0.0;
float PH_SLOPE = -5.7;
const float TDS_K = 0.5;
float TURB_CLEAR_V = 2.6;
float TURB_DIRTY_V = 0.4;

void setup() {
  Serial.begin(115200);
  delay(100);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
  }
  sensors.begin();
  Serial.println("Clear Stream Guardian - ESP32");
#ifdef USE_BLYNK
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Blynk.begin(auth, ssid, pass);
#endif
}

float readAnalogVoltage(int pin) {
  const int samples = 10;
  long acc = 0;
  for (int i = 0; i < samples; ++i) {
    acc += analogRead(pin);
    delay(5);
  }
  float avg = (float)acc / samples;
  return (avg / ADC_MAX) * VREF;
}

float readTemperatureC() {
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

float convertVoltageToPH(float v) {
  float pH_ref = 7.0;
  float v_ref = 2.5 + PH_VOLTAGE_OFFSET;
  return pH_ref + (v_ref - v) / PH_SLOPE;
}

float convertVoltageToTDS(float v, float temperatureC) {
  float ec = (133.42 * v * v * v) - (255.86 * v * v) + (857.39 * v);
  float compensationCoefficient = 1.0 + 0.02 * (temperatureC - 25.0);
  ec = ec / compensationCoefficient;
  return ec * TDS_K;
}

float convertVoltageToTurbidityNTU(float v) {
  if (v >= TURB_CLEAR_V) return 0.0;
  else if (v <= TURB_DIRTY_V) return 500.0;
  return (TURB_CLEAR_V - v) / (TURB_CLEAR_V - TURB_DIRTY_V) * 500.0;
}

unsigned long lastMillis = 0;
const unsigned long INTERVAL = 5000;

void loop() {
#ifdef USE_BLYNK
  Blynk.run();
#endif
  if (millis() - lastMillis < INTERVAL) return;
  lastMillis = millis();

  float tempC = readTemperatureC();
  float v_ph = readAnalogVoltage(PIN_PH);
  float v_tds = readAnalogVoltage(PIN_TDS);
  float v_turb = readAnalogVoltage(PIN_TURB);

  float phValue = convertVoltageToPH(v_ph);
  float tdsValue = convertVoltageToTDS(v_tds, (isnan(tempC) ? 25.0 : tempC));
  float turbNTU = convertVoltageToTurbidityNTU(v_turb);

  Serial.println("---- Readings ----");
  Serial.printf("Temp: %.2f C\n", tempC);
  Serial.printf("pH: %.2f\n", phValue);
  Serial.printf("TDS: %.0f ppm\n", tdsValue);
  Serial.printf("Turb: %.0f NTU\n", turbNTU);

  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.printf("Temp: %.1f C\n", tempC);
  display.printf("pH: %.2f\n", phValue);
  display.printf("TDS: %.0f ppm\n", tdsValue);
  display.printf("Turb: %.0f NTU\n", turbNTU);
  display.display();
#ifdef USE_BLYNK
  Blynk.virtualWrite(V1, tempC);
  Blynk.virtualWrite(V2, phValue);
  Blynk.virtualWrite(V3, tdsValue);
  Blynk.virtualWrite(V4, turbNTU);
#endif
}
