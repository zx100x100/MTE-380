#include <WiFi.h>
#include <Arduino.h>
#include <proto/pb_common.h>
#include <proto/pb.h>
#include <proto/pb_encode.h>
#include <proto/pb_decode.h>

#include "wifi_server.h"

#define POSITION_BUF_SIZE 60
#define COMMAND_BUF_SIZE 30


const char* ssid = "K";
const char* password = "12341234";
WiFiServer server(23);
WiFiClient client;
bool alreadyConnected = false;

// Set your Static IP address
IPAddress local_IP(192, 168, 86, 111);
// Set your Gateway IP address
IPAddress gateway(192, 168, 86, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

void serialize_position(pb_ostream_t& stream, Position& position){
  if (!pb_encode(&stream, Position_fields, &position)){
    Serial.println("failed to encode position");
    Serial.println(PB_GET_ERROR(&stream));
    return;
  }
}
void serialize_command_temp(pb_ostream_t& stream, Command& position){
  if (!pb_encode(&stream, Command_fields, &position)){
    Serial.println("failed to encode position");
    Serial.println(PB_GET_ERROR(&stream));
    return;
  }
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
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  host_server();
}

void host_server(){
  server.begin();
}

void server_tick(Command& command, Position& position){
  Serial.print("t.");
  if (!client){
    client = server.available();
  }
  if (client) {
    // Serial.print("a");
    if (!alreadyConnected) {
      client.flush(); // clear out the input buffer
      Serial.println("New client");
      alreadyConnected = true;
      return;
    }
    else{
      int receive_bytes = client.available();
      Serial.print("receive_bytes:");
      Serial.print(receive_bytes);
      // if (receive_bytes > 0){ // input available, update cmd
        // uint8_t input_buffer[POSITION_BUF_SIZE];
        // int i = 0;
        // while (client.available() > 0) {
          // // read the bytes incoming from the client:
          // uint8_t thisChar = uint8_t(client.read());
          // input_buffer[i++] = thisChar;
          // Serial.write(thisChar);
        // }
        // pb_istream_t instream = pb_istream_from_buffer(input_buffer, COMMAND_BUF_SIZE);
        // Command command = Command_init_zero;
        // bool decode_status = pb_decode(&instream, Command_fields, &command);
        // if (!decode_status){
          // Serial.println("Failed to decode Command!");
          // return;
        // }
        // // Serial.print("RunState: "); Serial.println(command.robot_state.run_state);
        // Serial.print("command.test: "); Serial.println(command.test);
      // }

      // output
      Serial.print("send.");
      pb_ostream_t stream;

      uint8_t buffer[POSITION_BUF_SIZE];
      stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
      // serialize_position(stream, position);
      Command command = Command_init_zero;
      command.test = 1.0;
      serialize_command_temp(stream, command);
      
      client.write(buffer, stream.bytes_written);
      Serial.print("written:"); Serial.println(stream.bytes_written);
      client.flush();


      // TEMP DECODE THE OUTPUT BUFFER
      pb_istream_t instream = pb_istream_from_buffer(buffer, stream.bytes_written);
      Command command2 = Command_init_zero;
      bool decode_status = pb_decode(&instream, Command_fields, &command2);
      if (!decode_status){
        Serial.printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
        return;
      }
      // Serial.print("RunState: "); Serial.println(command.robot_state.run_state);
      Serial.print("command.test: "); Serial.println(command2.test);
    }
  }
  else{
    Serial.println("unavailable");
  }
}
