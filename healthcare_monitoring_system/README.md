# Healthcare Monitoring System

A comprehensive healthcare monitoring system built for Arduino that continuously tracks vital health metrics and environmental conditions.

## Overview

This system collects real-time health data from multiple sensors to monitor patient vitals and surrounding environment. It provides continuous readings of heart rate, blood oxygen saturation, body temperature, room temperature, and humidity.

## Features

- **Heart Rate Monitoring**: Measures beats per minute (bpm) using optical pulse detection
- **Blood Oxygen Saturation (SpO2)**: Tracks oxygen saturation percentage in the blood
- **Body Temperature**: Measures core body temperature
- **Room Temperature & Humidity**: Monitors ambient environmental conditions
- **Robust Fallback System**: Maintains last known good readings to handle sensor inconsistencies
- **Serial Output**: Displays formatted readings every 2 seconds

## Hardware Requirements

### Sensors
- **MAX30102**: Pulse oximeter and heart rate sensor
- **DHT22**: Digital humidity and temperature sensor (for room conditions)
- **DS18B20**: Digital temperature sensor (for body temperature)

### Microcontroller
- Arduino or compatible board with:
  - I2C interface (SDA/SCL pins)
  - Digital I/O pins for DHT22 (default: D2) and DS18B20 (default: D3)
  - Serial communication (115200 baud)

## Wiring

```
MAX30102 (I2C):
  SDA → Arduino SDA
  SCL → Arduino SCL
  VCC → 3.3V
  GND → GND

DHT22:
  Signal → D2
  VCC → 3.3V/5V
  GND → GND

DS18B20 (One-Wire):
  Signal → D3
  VCC → 3.3V/5V
  GND → GND
  (Note: Connect 4.7kΩ pull-up resistor between signal and VCC)
```

## Libraries Required

- `Wire.h` - I2C communication
- `MAX30105.h` - MAX30102 sensor driver
- `heartRate.h` - Heart rate algorithm
- `spo2_algorithm.h` - SpO2 calculation algorithm
- `DHT.h` - DHT sensor library
- `OneWire.h` - One-Wire protocol
- `DallasTemperature.h` - DS18B20 temperature library

## Installation

1. Clone or download this repository
2. Open `code.ino` in the Arduino IDE
3. Install required libraries via Arduino Library Manager:
   - Search for and install: DHT, OneWire, DallasTemperature
   - For MAX30102, install the SparkFun MAX3010x Pulse and Proximity Sensor Library
4. Select your Arduino board and COM port
5. Upload the sketch

## Configuration

### Pin Definitions
```cpp
#define DHTPIN D2          // DHT22 signal pin
#define ONE_WIRE D3        // DS18B20 signal pin
```

### MAX30102 Settings
```cpp
byte bright = 60;          // LED brightness (0-255)
byte avg = 4;              // Number of averaged samples
byte ledMode = 2;          // LED mode (2 = Red + IR)
int sRate = 100;           // Sample rate (Hz)
int pWidth = 411;          // Pulse width (microseconds)
int adc = 4096;            // ADC resolution
```

Modify these values in the `setup()` function if needed.

## Operation

1. Power on the Arduino
2. Open Serial Monitor (115200 baud) to view readings
3. System initializes all sensors (~1 second startup)
4. Readings are updated and displayed every 2 seconds

### Sample Output
```
==== Current Readings ====
HR: 72 bpm
SpO2: 98%
Humidity = 45.5%
Room Temp = 22.3 C
Body Temp = 36.8 C
==========================
```

## How It Works

### Heart Rate & SpO2 Detection
- The MAX30102 collects 100 samples of infrared and red LED light reflections
- The `maxim_heart_rate_and_oxygen_saturation()` algorithm processes this data
- Results are only updated when sensor validation succeeds (`hr_ok` or `spo2_ok` flags)
- Falls back to last known good value if sensor returns invalid data

### Temperature Monitoring
- **Room Temperature**: DHT22 sensor provides ambient air temperature
- **Body Temperature**: DS18B20 provides precise temperature via One-Wire protocol

### Environmental Monitoring
- **Humidity**: DHT22 measures relative humidity percentage
- Includes error handling for DHT sensor failures

## Troubleshooting

### MAX30102 Not Responding
- Verify I2C wiring (SDA/SCL connections)
- Check that pull-up resistors are present on I2C lines
- Ensure 3.3V power supply (not 5V) for the sensor

### DHT22 Reading Fails
- System prints "DHT failed (again... ugh)"
- Check wiring and ensure proper power connection
- DHT sensors are notoriously unreliable; consider replacing if frequent failures occur

### Sensor Values Jumping Wildly
- This is normal behavior when sensor validation fails
- The fallback system prevents display of clearly invalid readings
- May indicate need for sensor replacement or recalibration

### No Serial Output
- Verify baud rate is set to 115200 in Serial Monitor
- Check USB connection and Arduino drivers

## Performance Notes

- Heart rate reading takes approximately 1 second per cycle (100 samples @ 100 Hz)
- DS18B20 temperature requests add ~750ms latency per reading
- Total cycle time: ~2 seconds (includes delays for sensor stabilization)
- Serial output updates every 2 seconds

## Safety Considerations

**This system is for monitoring purposes only and should not be used for medical diagnosis or treatment decisions. Always consult with healthcare professionals for medical concerns.**

## Future Enhancements

- SD card logging for long-term data storage
- Bluetooth/WiFi connectivity for remote monitoring
- Alerts for abnormal readings
- Data visualization dashboard
- Cloud integration for health tracking

## License

This project is open source and available for educational and personal use.

## References

- MAX30102 Datasheet
- DHT22 Sensor Documentation
- DS18B20 Temperature Sensor Guide
- Arduino Official Documentation
