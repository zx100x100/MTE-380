/* #include <chrono> */
/* #include <thread> */
/* #include <iostream> */
/* #include <WiFi.h> */
/* #include <Arduino.h> */

/* const char* ssid = "K"; */
/* const char* password = "12341234"; */

/* void connect_server(){ */
  /* int status = WL_IDLE_STATUS; */
  /* IPAddress server(192,168,86,243); */
  /* // Initialize the client library */
  /* WiFiClient client; */
  /* Serial.println("Starting connection..."); */
  /* if (client.connect(server, 80)) { */
    /* Serial.println("connected"); */
    /* // Make a HTTP request: */
    /* client.println("GET /search?q=arduino HTTP/1.0"); */
    /* client.println(); */
  /* } */
  /* else{ */
    /* Serial.println("not connected"); */
  /* } */
/* } */

/* void setup_wifi(){ */
  /* WiFi.mode(WIFI_STA); */
  /* WiFi.begin(ssid, password); */
  /* Serial.println("Connecting to WiFi .."); */
  /* while (WiFi.status() != WL_CONNECTED) { */
    /* Serial.print("."); */
    /* [> std::this_thread::sleep_for(std::chrono::seconds(1)); <] */
    /* delay(1000); */
  /* } */
  /* Serial.println(WiFi.localIP()); */
  /* connect_server(); */
/* } */
