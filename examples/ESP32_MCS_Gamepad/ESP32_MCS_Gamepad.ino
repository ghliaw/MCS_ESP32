#include <WiFi.h>
#include <WiFiClient.h>
#include "MCS_ESP32.h"

// Assign AP ssid / password here
#define _SSID "your_wifi_ap_ssid"
#define _KEY  "your_wifi_password"

// Assign device id / key of your test device
#define MCS_SERVER_ADDR  "your server address"
#define MCS_SERVER_PORT     3000	// default MCS lite HTTP port
#define MCS_DEVICE_ID "your device id"
#define MCS_DEVICE_KEY  "your device key"
MCSLiteDevice mcs(MCS_DEVICE_ID, MCS_DEVICE_KEY, MCS_SERVER_ADDR, MCS_SERVER_PORT);

// Assign channel id 
// The test device should have a channel id "control_gamepad".
// the first channel should be "Controller" - "GamePad"
MCSControllerGamePad gamepad("control_gamepad");

void setup() {
  // setup Serial output at 9600
  Serial.begin(9600);

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
  mcs.addChannel(gamepad);
    
  while(!mcs.connected())
  {
    Serial.println("MCS.connect()...");
    mcs.connect();
  }
  Serial.println("MCS connected !!");

  while(!gamepad.valid())
  {
    Serial.println("initialize gamepad default value...");
    gamepad.value();    
    // Note: At this moment we can "read" the values
    // of the gamepad - but the value is meaningless.
    // 
    // The MCS server returns that "last button pressed" 
    // in this cause - even if the user is not pressing any button
    // at this moment.
    // 
    // We read the values here simply to make the following
    // process() -> if(gamepad.updated()) check working.
  }

}

void loop() {
  // Note that each process consumes 1 command from MCS server.
  // The 100 millisecond timeout assumes that the server
  // won't send command rapidly.
  mcs.process(100);

  // updated flag will be cleared in process(), user must check it after process() call.
  if(gamepad.updated())
  {
    Serial.print("Gamepad event arrived, new value = ");
    Serial.println(gamepad.value());
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
