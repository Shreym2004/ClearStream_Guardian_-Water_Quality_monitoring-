# Clear Stream Guardian 🌊

ESP32-based water quality monitoring system.  
Reads **Temperature (DS18B20)**, **pH**, **TDS**, and **Turbidity**, then displays on an OLED and (optionally) uploads to Blynk.

## 🚀 Features
- DS18B20 temperature sensor
- Analog pH probe
- Analog TDS probe
- Analog Turbidity probe
- 0.96" OLED display (SSD1306, I2C)
- Optional Blynk cloud integration
- Calibration sketches for pH, TDS, and Turbidity included

## 📦 Hardware Connections

| Sensor / Module  | ESP32 Pin |
|------------------|-----------|
| DS18B20 (Data)   | GPIO 4    |
| pH (Analog)      | GPIO 34   |
| TDS (Analog)     | GPIO 35   |
| Turbidity (Analog)| GPIO 32  |
| OLED SDA         | GPIO 21   |
| OLED SCL         | GPIO 22   |

⚠️ Some modules require **5V power** but must output ≤ 3.3V to ESP32 ADC. Use level shifting or check module specs.

## 📚 Libraries Required
- OneWire
- DallasTemperature
- Adafruit SSD1306
- Adafruit GFX
- Blynk (optional)

## ⚙️ Calibration
- Use calibration sketches in `/calibration` with standard solutions.
- Update constants in `ClearStreamGuardian.ino` after calibration.

## 📡 Blynk Integration
Uncomment `#define USE_BLYNK` and add your credentials.

## 📝 License
MIT License.
