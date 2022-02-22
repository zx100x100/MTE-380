#include <chrono>
#include <thread>
#include <iostream>
#include <WiFi.h>
#include <Arduino.h>

#include "wifi_server.h"

const char* ssid = "K";
const char* password = "12341234";
WiFiServer server(23);
bool alreadyConnected = false;

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 111);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

void wifi_init(){
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA (static IP) Failed to configure");
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    /* std::this_thread::sleep_for(std::chrono::seconds(1)); */
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  host_server();
}

void host_server(){
  server.begin();
}

void server_tick(){
  // wait for a new client:
  WiFiClient client = server.available();


  // when the client sends the first byte, say hello:
  if (client) {
    if (!alreadyConnected) {
      // clead out the input buffer:
      client.flush();
      Serial.println("We have a new client");
      client.println("Hello, client!");
      alreadyConnected = true;
    }

    if (client.available() > 0) {
      // read the bytes incoming from the client:
      char thisChar = client.read();
      // echo the bytes back to the client:
      server.write(thisChar);
      // echo the bytes to the server as well:
      Serial.write(thisChar);
    }
  }
}
