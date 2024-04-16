#include "Arduino.h"
#include <WifiManager.h>
#include <HTTPClient.h>
#include <WiFi.h>

#include "esp32cam.h"

esp32cam::Resolution initialResolution;

WiFiManager wm;

//TODO: this probably isn't the right URL but idk
const char* serverUrl = "http://alex-is-cool:5000/upload";

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("serial initialized");

  {
    using namespace esp32cam;

    initialResolution = Resolution::find(1024, 768);

    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(initialResolution);
    cfg.setJpeg(80);

    bool ok = Camera.begin(cfg);
    if (!ok) {
      Serial.println("camera initialize failure");
      delay(5000);
      ESP.restart();
    }
    Serial.println("camera initialize success");
  }

  bool res = wm.autoConnect("EspSketchCam", "urMomLol");
  if (!res) {
    Serial.println("Failed to connect to WiFi");
    ESP.restart();
  } else {
    Serial.println("Connected to WiFi");
  }

  Serial.println("tcp server started");
}

void loop() {
  delay(1000);
  Serial.println("Capturing image... Say cheese!");

  // capture image to send
  using namespace esp32cam;
  auto frame = Camera.capture();
  if (frame == nullptr) {
    Serial.println("Failed to capture image");
    return;
  }

  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    HTTPClient http;

    http.begin(serverUrl); //Specify the URL
    http.addHeader("Content-Type", "application/octet-stream"); //Specify content-type header 

    int httpResponseCode = http.POST((uint8_t*)frame->data(), frame->size());   //Send the actual POST request

    if (httpResponseCode>0) {
      String response = http.getString();   //Get the response to the request
      Serial.println(httpResponseCode);   //Print return code
      Serial.println(response);           //Print request answer
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end(); //Free resources
  }

  // print ip
  Serial.println(WiFi.localIP());
}
