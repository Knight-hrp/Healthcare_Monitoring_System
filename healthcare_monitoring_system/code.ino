#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "spo2_algorithm.h"

#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// --------- for MAX30102 (Pulse + SpO2) ---------
MAX30105 pulse;

// buffers for readings (100 feels like a safe number didn't test smaller much)
uint32_t irData[100];
uint32_t redData[100];

// results
int32_t spo2_out;
int8_t spo2_ok;
int32_t hr_out;
int8_t hr_ok;

// fallback values (sensor sometimes gives garbage values randomly)
int lastKnownHR = 0;
int lastKnownSpO2 = 0;

// --------- DHT22 ---------
#define DHTPIN D2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// --------- DS18B20 ---------
#define ONE_WIRE D3
OneWire ow(ONE_WIRE);
DallasTemperature ds18(&ow);


// --------- SETUP ---------
void setup() {
  Serial.begin(115200);

  delay(1000); // giving everything time to settle

  Serial.println("Initializing stuff...");

  // I once forgot these pins and wasted like 30 mins debugging
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!pulse.begin(Wire)) {
    Serial.println("MAX30102 not responding...");
    while (1);
  }

  Serial.println("Pulse sensor ready.");

  // config (copied from example initially, tweaked slightly)
  byte bright = 60;
  byte avg = 4;
  byte ledMode = 2;
  int sRate = 100;
  int pWidth = 411;
  int adc = 4096;

  pulse.setup(bright, avg, ledMode, sRate, pWidth, adc);

  pulse.setPulseAmplitudeRed(0x1F);
  pulse.setPulseAmplitudeIR(0x1F);

  // starting other sensors
  dht.begin();
  ds18.begin();
}


// --------- LOOP ---------
void loop() {

  // ---- read pulse sensor ----
  for (int i = 0; i < 100; i++) {

    // blocking wait
    while (!pulse.available()) {
      pulse.check();
    }

    redData[i] = pulse.getRed();
    irData[i] = pulse.getIR();

    pulse.nextSample();
  }

  // calculate values
  maxim_heart_rate_and_oxygen_saturation(
    irData, 100, redData,
    &spo2_out, &spo2_ok,
    &hr_out, &hr_ok
  );

  // only update if valid (otherwise values jump weirdly)
  if (hr_ok == 1) {
    lastKnownHR = hr_out;
  } else {
    // sometimes it fails for no reason, keeping old value
  }

  if (spo2_ok == 1) {
    lastKnownSpO2 = spo2_out;
  }

  // ---- For DHT22 ----
  float humidityVal = dht.readHumidity();
  float tempRoom = dht.readTemperature();

  // ---- For DS18B20 ----
  ds18.requestTemperatures();   // takes time...
  float tempBody = ds18.getTempCByIndex(0);

  // ---- printing ----
  Serial.println("\n==== Current Readings ====");

  Serial.print("HR: ");
  Serial.print(lastKnownHR);
  Serial.println(" bpm");

  Serial.print("SpO2: ");
  Serial.print(lastKnownSpO2);
  Serial.println("%");

  if (isnan(humidityVal) || isnan(tempRoom)) {
    Serial.println("DHT failed (again... ugh)");
  } else {
    Serial.print("Humidity = ");
    Serial.print(humidityVal);
    Serial.println("%");

    Serial.print("Room Temp = ");
    Serial.print(tempRoom);
    Serial.println(" C");
  }

  Serial.print("Body Temp = ");
  Serial.print(tempBody);
  Serial.println(" C");

  Serial.println("==========================");

  // random thought
  delay(2000);
}