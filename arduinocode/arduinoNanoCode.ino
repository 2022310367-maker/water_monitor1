#include <OneWire.h>
#include <DallasTemperature.h>

float tempValue = 0;
float phValue = 0;
float turbidityValue = 0;

int tempSensorPin = A5;
int turbiditySensorPin = A1;
int phSensorPin = A0;

OneWire oneWire(tempSensorPin);
DallasTemperature tempSensor(&oneWire);

void setup() {
  Serial.begin(115200);
  tempSensor.begin();
}

void loop() {

  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "getData") {
      sendJSON();
    }
  }

  delay(50);
}

void sendJSON() {
  float t = getTemp();
  float pH = getPH();
  float turb = getTurbidity();

  Serial.print("{\"temperature\":");
  Serial.print(t);
  Serial.print(",\"ph\":");
  Serial.print(pH);
  Serial.print(",\"turbidity\":");
  Serial.print(turb);
  Serial.println("}");
}

float getTemp() {
  tempSensor.requestTemperatures();
  float temp = tempSensor.getTempCByIndex(0);

  if (temp != DEVICE_DISCONNECTED_C) {
    return temp;
  } else {
    return 0;
  }
}

float getPH() {
  float measure = analogRead(phSensorPin);
  float voltage = measure * (5.0 / 1024.0);
  float phVal = 2.85 * voltage;
  return phVal;
}

float getTurbidity() {
  float measure = analogRead(turbiditySensorPin);
  float turbVal = (measure * (5.0 / 1024.0));
  return turbVal;
}
