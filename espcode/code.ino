#include <time.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"
#include <LittleFS.h>
#include <pgmspace.h>
#include "index.h"
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <ArduinoJson.h>

String incomingData = "";

#define FORMAT_LITTLEFS_IF_FAILED true

const char ssidPath[] PROGMEM = "/SSID";
const char passwordPath[] PROGMEM = "/Password";
const char AP_PASSWORD[] PROGMEM = "12345678";

AsyncWebServer server(80);

const char *ntpServer = "time.google.com";
const long gmtOffset_sec = 28800;
const int daylightOffset_sec = 0;

const char USER_EMAIL[] PROGMEM = "waterMonitor@gmail.com";
const char USER_PASSWORD[] PROGMEM = "@adminroot";
const char API_KEY[] PROGMEM = "AIzaSyBEsFQwYbsJmHcnyfyDMk7euKD0S1-CWWA";
const char DATABASE_URL[] PROGMEM = "https://water-monitoring-system-aac9b-default-rtdb.asia-southeast1.firebasedatabase.app/";

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

struct Config {
  String ssid;
  String password;
} deviceConfig;

int hour, minute, second, day, month, year;

float tempValue = 0;
float phValue = 0;
float turbidityValue = 0;

void setup(void) {
  Serial.begin(115200);

  if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
    Serial.println(F("LittleFS Mount Failed"));
    return;
  }

  deviceConfig.ssid = readFile(LittleFS, ssidPath);
  deviceConfig.password = readFile(LittleFS, passwordPath);

  WiFi.mode(WIFI_STA);
  WiFi.begin(deviceConfig.ssid.c_str(), deviceConfig.password.c_str());

  if (!connectToWiFi()) {
    startAPMode();
  } else {
    Serial.println(F("Connected to Wi-Fi."));
    initializeFirebase();
  }

  setupWebServer();
  server.begin();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  delay(5000);
}

void loop() {
  Serial.println("getData");
  delay(2000);
  if (Serial.available()) {
    incomingData = Serial.readStringUntil('\n');
    incomingData.trim();

    if (incomingData.startsWith("{")) {
      StaticJsonDocument<200> doc;

      DeserializationError error = deserializeJson(doc, incomingData);

      if (error) {
        Serial.print("JSON parse failed: ");
        Serial.println(error.c_str());
        return;
      }
  
      float tempValue = doc["temperature"];
      float phValue = doc["ph"];
      float turbidityValue = doc["turbidity"];

      updateLocalTime();

      char timestamp[25];
      sprintf(timestamp, "%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);

      String path = "/readings/";
      path += String(year) + "-" + String(month) + "-" + String(day) + "_" + String(hour) + ":" + String(minute) + ":" + String(second);

      Serial.printf("Writing entry: %s\n", path.c_str());
      Serial.printf("Temp: %.2f, pH: %.2f, Turbidity: %.2f\n", tempValue, phValue, turbidityValue);

      if (Firebase.RTDB.setFloat(&fbdo, path + "/temp", tempValue)) {
        Serial.println("Temperature written.");
      } else {
        Serial.println("Temp write failed: " + fbdo.errorReason());
      }

      if (Firebase.RTDB.setFloat(&fbdo, path + "/ph", phValue)) {
        Serial.println("pH written.");
      } else {
        Serial.println("pH write failed: " + fbdo.errorReason());
      }

      if (Firebase.RTDB.setFloat(&fbdo, path + "/turbidity", turbidityValue)) {
        Serial.println("Turbidity written.");
      } else {
        Serial.println("Turbidity write failed: " + fbdo.errorReason());
      }

      if (Firebase.RTDB.setString(&fbdo, path + "/timestamp", timestamp)) {
        Serial.println("Timestamp written.");
      } else {
        Serial.println("Timestamp write failed: " + fbdo.errorReason());
      }
    }
  }
}

void updateLocalTime() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    hour = timeinfo.tm_hour;
    minute = timeinfo.tm_min;
    second = timeinfo.tm_sec;
    day = timeinfo.tm_mday;
    month = timeinfo.tm_mon + 1;
    year = timeinfo.tm_year + 1900;
  }
}

bool connectToWiFi() {
  for (byte attempts = 0; attempts < 20; attempts++) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(F("\nConnected to Wi-Fi!"));
      return true;
    }
    delay(1000);
    Serial.print(".");
  }
  Serial.println(F("\nFailed to connect to Wi-Fi"));
  return false;
}

void startAPMode() {
  Serial.println(F("Switching to AP mode"));
  WiFi.mode(WIFI_AP);
  WiFi.softAP("DeviceAP", AP_PASSWORD);
  Serial.println(WiFi.softAPIP());
}

void setupWebServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", configure);
  });

  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
      deviceConfig.ssid = request->getParam("ssid", true)->value();
      deviceConfig.password = request->getParam("password", true)->value();

      saveCredentials();

      request->send(200, "text/html", HTML_OK);
      delay(1000);
      ESP.restart();
    } else {
      request->send(400, "text/html", HTML_ERROR);
    }
  });
}

void saveCredentials() {
  writeFile(LittleFS, ssidPath, deviceConfig.ssid.c_str());
  writeFile(LittleFS, passwordPath, deviceConfig.password.c_str());
}

String readFile(fs::FS &fs, const char *path) {
  Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println(F("- failed to open file for reading"));
    return "";
  }
  String fileContent;
  while (file.available()) {
    fileContent += (char)file.read();
  }
  file.close();
  return fileContent;
}

void writeFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println(F("- failed to open file for writing"));
    return;
  }
  file.print(message);
  file.close();
}

void initializeFirebase() {
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);

  fbdo.setBSSLBufferSize(2048, 2048);
  fbdo.setResponseSize(1024);

  Firebase.RTDB.setwriteSizeLimit(&fbdo, "tiny");
  Firebase.RTDB.setMaxRetry(&fbdo, 1);

  Firebase.begin(&config, &auth);

  if (Firebase.ready()) {
    Serial.println("Connected to Firebase!");
  } else {
    Serial.println("Failed to connect to Firebase. Check configuration.");
    Serial.println("Error: " + String(config.signer.tokens.error.message.c_str()));
  }
}
