#include "Arduino.h"
#include <WifiManager.h>
#include <WifiServer.h>
#include <WiFi.h>

#include "esp32cam.h"

esp32cam::Resolution initialResolution;

WiFiServer tcp_server(8080); // Set the desired port number
WiFiManager wm;

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

  tcp_server.begin();
  Serial.println("tcp server started");
}

void loop() {
  delay(1000);
  Serial.println("camera starting");
  WiFiClient tcp_client = tcp_server.available();
    if (tcp_client) {
      using namespace esp32cam;
      auto frame = Camera.capture();
      if (frame == nullptr) {
      Serial.println("Failed to capture image");
      return;
    }

    // Send the image size first
    uint32_t imageSize = frame->size();
    tcp_client.write((uint8_t*)&imageSize, sizeof(imageSize));

    // Then send the image data
    tcp_client.write(frame->data(), frame->size());

    tcp_client.stop();
  }

  // print ip
  Serial.println(WiFi.localIP());
}
