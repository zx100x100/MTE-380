#include <Arduino.h>
#include <proto/pb_common.h>
#include <proto/pb.h>
#include <proto/pb_encode.h>
#include <proto/pb_decode.h>

#include "imu_data.pb.h"
#include "tof_data.pb.h"
#include "telemetry_server.h"
#include "cmd_data.pb.h"

#define CMD_BUF_SIZE 30
#define OUTPUT_BUF_SIZE 300
const uint8_t delimit[3] = {uint8_t(':'),uint8_t(':'),uint8_t(':')};

TelemetryServer::TelemetryServer(Sensors& sensors,
                          NavData& navData,
                          GuidanceData& guidanceData,
                          CmdData& cmdData,
                          Hms* hms):
  sensors(sensors),
  navData(navData),
  guidanceData(guidanceData),
  cmdData(cmdData),
  hms(hms){
}

void TelemetryServer::init(){
  WiFi.disconnect();
  if (!WiFi.config(localIP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA (static IP) Failed to configure");
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi ..");
  int wifiConnectionTicks = 0;
  const int maxWifiConnectionTicksBeforeReboot = 40;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
    if (wifiConnectionTicks++ >= maxWifiConnectionTicksBeforeReboot){
      ESP.restart();
    }
  }
  Serial.println(WiFi.localIP());
  server.begin();
}

void delimitData(pb_ostream_t& stream){
  if (!pb_write(&stream, delimit, 3)){
    Serial.printf("write fail: %s\n", PB_GET_ERROR(&stream));
    return;
  }
}

void TelemetryServer::serializeData(pb_ostream_t& stream){
  if (!pb_encode(&stream, NavData_fields, &navData)){
    Serial.printf("encode fail: %s\n", PB_GET_ERROR(&stream));
    return;
  }
  delimitData(stream);
  /* delimitData(stream); */

  if (!pb_encode(&stream, GuidanceData_fields, &guidanceData)){
    Serial.printf("encode fail: %s\n", PB_GET_ERROR(&stream));
    return;
  }
  delimitData(stream);
  
  if (!pb_encode(&stream, HmsData_fields, &hms->data)){
    Serial.printf("encode fail: %s\n", PB_GET_ERROR(&stream));
    return;
  }
  delimitData(stream);
  
  if (!pb_encode(&stream, ImuData_fields, &sensors.imu.getData())){
    Serial.printf("encode fail: %s\n", PB_GET_ERROR(&stream));
    return;
  }
  delimitData(stream);
  
  for (int i=0; i<4; i++){
    if (!pb_encode(&stream, TofData_fields, &sensors.tof[i].getData())){
      Serial.printf("encode fail: %s\n", PB_GET_ERROR(&stream));
      return;
    }
    if (i<3){
      delimitData(stream);
    }
  }
}

void TelemetryServer::update(){
  if (!client){
    client = server.available();
  }
  if (client) {
    if (!alreadyConnected) {
      // commented out because idk why we needed it, and
      // our logic is that we only send data after receiving data,
      // ... and since the dashboard is gonna wait for new data from us
      // after sending its own first command, having a flush here makes
      // the dashboard freeze entirely.
      // client.flush(); // clear out the input buffer
      Serial.println("New client for tcp telemetry server");
      alreadyConnected = true;
      return;
    }
    else{
      // TODO CHANGE THIS BEHAVIOR OR AT LEAST MAKE SURE DOING IT THIS WAY ISNT MAKING TICKRATES LESS CONSISTENT
      int receiveBytes = client.available();
      if (receiveBytes > 0){ // input available, update cmd, send back telemetry
        // RECEIVE DATA -----------------------------------
        uint8_t inputBuffer[CMD_BUF_SIZE];
        int i = 0;
        while (client.available() > 0) {
          // read the bytes incoming from the client:
          uint8_t thisChar = uint8_t(client.read());
          inputBuffer[i++] = thisChar;
        }
        pb_istream_t instream = pb_istream_from_buffer(inputBuffer, i);
        bool decodeStatus = pb_decode(&instream, CmdData_fields, &cmdData);
        if (!decodeStatus){
          Serial.printf("Decoding Cmd fail: %s\n", PB_GET_ERROR(&instream));
          return;
        }

        // SEND DATA --------------------------------------
        pb_ostream_t stream;
        uint8_t buffer[OUTPUT_BUF_SIZE];
        stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        serializeData(stream);
        client.flush();
        client.write(buffer, stream.bytes_written); // takes 0-2 ms
      }
    }
  }
  else{
    Serial.println("No TCP client connected.");
  }
}
