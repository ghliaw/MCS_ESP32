#include <WiFi.h>
#include <WiFiClient.h>
#include "MCS_ESP32.h"
#include "DHT.h"

// DHT Sensor related definition & declaration
#define DHTPIN 27         // Digital pin connected to the DHT sensor
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22     // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHT_PERIOD  5000  //ms
DHT dht(DHTPIN, DHTTYPE);
unsigned long dht_last_time = 0;

// Assign AP ssid / password here
#define _SSID "31N"
#define _KEY  "087389887"

// Assign device id / key of your test device
#define MCS_SERVER_URL  "140.127.196.40"
#define MCS_SERVER_PORT     3000
#define MCS_DEVICE_ID "S1_4gIZIa"
#define MCS_DEVICE_KEY  "e55f563c2de72c27ff2704ff4ea31e54b92244cd531c0c86f5a07eaace729440"
MCSLiteDevice mcs(MCS_DEVICE_ID, MCS_DEVICE_KEY, MCS_SERVER_URL, MCS_SERVER_PORT);

// Assign channel id 
MCSControllerOnOff led("led_sw");
MCSDisplayFloat    temp("temp");
MCSDisplayFloat    hum("hum");

#define LED_PIN LED_BUILTIN

void setup() {
  // setup Serial output at 115200
  Serial.begin(115200);

  // setup LED/Button pin
  pinMode(LED_PIN, OUTPUT);

  // setup Wifi connection
  WiFi.begin(_SSID, _KEY);
  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // setup MCS connection
  mcs.addChannel(led);
  mcs.addChannel(temp);
  mcs.addChannel(hum);
  while(!mcs.connected())
  {
    Serial.println("MCS.connect()...");
    mcs.connect();
  }
  Serial.println("MCS connected !!");

  // read LED value from MCS server
  while(!led.valid())
  {
    Serial.println("read LED value from MCS...");
    led.value();
  }
  Serial.print("done, LED value = ");
  Serial.println(led.value());
  digitalWrite(LED_PIN, led.value() ? HIGH : LOW);

  // Enable DHT sensor
  dht.begin();
}

void loop() {
  // call process() to allow background processing, add timeout to avoid high cpu usage
  //Serial.print("process(");
  //Serial.print(millis());
  //Serial.println(")");
  mcs.process(100);
  
  // updated flag will be cleared in process(), user must check it after process() call.
  if(led.updated())
  {
    Serial.print("LED updated, new value = ");
    Serial.println(led.value());
    digitalWrite(LED_PIN, led.value() ? HIGH : LOW);
  }

  // Repeatedly read and upoad temperature & humidity data
  unsigned long current_time;
  current_time = millis();
  if (current_time - dht_last_time >= DHT_PERIOD) {
    // read temperature & humidity data
    float h = dht.readHumidity();    
    float t = dht.readTemperature();
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
    }
    else {
      // Output to Serial for debugging
      Serial.print(F("Humidity: "));
      Serial.print(h);
      Serial.print(F("%  Temperature: "));
      Serial.print(t);
      Serial.println(F("°C "));
      // Upload data to MCS Lite
      if (hum.set(h)) Serial.println(F("Humidity upload successfully"));
      else Serial.println(F("Humidity upload failed"));
      if (temp.set(t)) Serial.println(F("Temperatiure upload successfully"));
      else Serial.println(F("Temperatiure upload failed"));
    }
    dht_last_time = current_time;
  }
  
  // check if need to re-connect
  while(!mcs.connected())
  {
    Serial.println("re-connect to MCS...");
    mcs.connect();
    if(mcs.connected())
      Serial.println("MCS connected !!");
  }
}
