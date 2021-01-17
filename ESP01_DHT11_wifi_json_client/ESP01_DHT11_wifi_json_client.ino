#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>

const uint8_t fingerprint[20] = {0x88, 0x38, 0x51, 0x41, 0x70, 0x08, 0x43, 0xf7, 0x5b, 0xa5, 0x52, 0xa2, 0xde, 0x5b, 0x66, 0xdc, 0xae, 0x7a, 0x00, 0xe6};
#define DHTTYPE    DHT11     // DHT 11
#define DHTPIN 2     // Digital pin connected to the DHT sensor 
uint32_t delayMS = 5* 1000;

String temperature = "";
String humidity = "";

DHT_Unified dht(DHTPIN, DHTTYPE);
ESP8266WiFiMulti WiFiMulti;
DynamicJsonDocument doc(1024);

void setup() {
  
  Serial.begin(115200);

  dht.begin();
    // Print temperature sensor details.
  sensor_t sensor;
  
  // Serial.setDebugOutput(true);
  Serial.println(F("DHTxx Unified Sensor Example"));

  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("perica", "normalnasifrazanet");
}

void readSensorData() {
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
    temperature = event.temperature;
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));

  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
    humidity = event.relative_humidity;
  }
  
}

void loop() {

  readSensorData();

  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setFingerprint(fingerprint);

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://iot.cinamontest.duckdns.org/")) {  // HTTPS

      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);

          https.addHeader("Content-Type", "application/json");
//          int httpResponseCode = https.POST("{\"api_key\":\"zGalhZZA4wKbFmFubAEomq0SHLVaW8na\",\"sensor\":\"DHT11\",\"value1\":\"24.25\",\"value2\":\"49.54\"}");
          doc["sensor"] = "DHT11";
          doc["api_key"]   = "zGalhZZA4wKbFmFubAEomq0SHLVaW8na";
          doc["value1"] = temperature;
          doc["value2"] = humidity;
          String output;
          serializeJson(doc, output);

//          int httpResponseCode = https.POST("{\"api_key\":\"f00uA7uC47RCmdB3DWOAh4UkLhsVhfn1\",\"sensor\":\"DHT11\",\"+"temperature"+\":\"24.25\",\"+"humidity"+\":\"49.54\"}");
          int httpResponseCode = https.POST(output);
          Serial.printf("[HTTPS] POST... code: %d\n", httpResponseCode);
          
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

  Serial.println("Wait  before next round...");
  delay(30*1000);
}

  
