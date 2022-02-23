#include <WiFi.h>
#include <Arduino.h>
#include <proto/pb_common.h>
#include <proto/pb.h>
#include <proto/pb_encode.h>

#include "wifi_server.h"

const char* ssid = "K";
const char* password = "12341234";
WiFiServer server(23);
bool alreadyConnected = false;

// Set your Static IP address
IPAddress local_IP(192, 168, 86, 111);
// Set your Gateway IP address
IPAddress gateway(192, 168, 86, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

#define POSITION_BUF_SIZE 60

void serialize_position(pb_ostream_t& stream, Position& position){
  /* if (!pb_encode_submessage(&stream, Location_fields, &position.loc)){ */
    /* Serial.println("failed to encode loc"); */
    /* Serial.println(PB_GET_ERROR(&stream)); */
    /* return; */
  /* } */
  /* if (!pb_encode_submessage(&stream, Velocity_fields, &position.vel)){ */
    /* Serial.println("failed to encode vel"); */
    /* Serial.println(PB_GET_ERROR(&stream)); */
    /* return; */
  /* } */
  /* if (!pb_encode_submessage(&stream, Accel_fields, &position.acc)){ */
    /* Serial.println("failed to encode accel"); */
    /* Serial.println(PB_GET_ERROR(&stream)); */
    /* return; */
  /* } */
  /* if (!pb_encode_submessage(&stream, Rotation_fields, &position.rot)){ */
    /* Serial.println("failed to encode rot"); */
    /* Serial.println(PB_GET_ERROR(&stream)); */
    /* return; */
  /* } */
  if (!pb_encode(&stream, Position_fields, &position)){
    Serial.println("failed to encode position");
    Serial.println(PB_GET_ERROR(&stream));
    return;
  }

  /* Serial.println("Data:"); */
  /* for(int i = 0; i<stream.bytes_written; i++){ */
    /* Serial.printf("%02X",buffer[i]); */
  /* } */
}

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
      /* client.println("Hello, client!"); */
      alreadyConnected = true;
    }
    else{
      client.flush();
      pb_ostream_t stream;

      uint8_t buffer[POSITION_BUF_SIZE];
      stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
      serialize_position(stream, position);
      
      client.write(buffer, stream.bytes_written);
      Serial.print("bytes: "); Serial.println(stream.bytes_written);
      Serial.println("bb");
    }

    if (client.available() > 0) {
      // read the bytes incoming from the client:
      char thisChar = client.read();
      // echo the bytes back to the client:
      /* server.write(thisChar); */
      // echo the bytes to the server as well:
      /* Serial.write(thisChar); */
    }
  }
}
