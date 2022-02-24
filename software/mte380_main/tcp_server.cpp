#include <Arduino.h>
#include <proto/pb_common.h>
#include <proto/pb.h>
#include <proto/pb_encode.h>
#include <proto/pb_decode.h>

#include "imu_data.pb.h"
#include "tof_data.pb.h"
#include "tcp_server.h"

#define CMD_BUF_SIZE 30
#define OUTPUT_BUF_SIZE = 300
const uint8_t delimit[3] = {uint8_t("|"),uint8_t("|"),uint8_t("|")};

void TcpServer::TcpServer(SensorData& sensorData,
                          NavData& navData,
                          GuidanceData& guidanceData,
                          CmdData& cmdData)
  : sensorData(sensorData)
  , navData(navData)
  , guidanceData(guidanceData)
  , cmdData(cmdData){
  if (!WiFi.config(localIP, gateway, subnet, primaryDNS, secondaryDNS)) {
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
  server.begin();
}

void delimitData(pb_ostream_t& stream){
  if (!pb_write(&stream, delimit, 3)){
    Serial.printf("write fail: %s\n", PB_GET_ERROR(&stream));
    return;
  }
}

void TcpServer::serializeData(pb_ostream_t& stream){
  if (!pb_encode(&stream, NavData_fields, &navData)){
    Serial.printf("encode fail: %s\n", PB_GET_ERROR(&stream));
    return;
  }
  delimitData(stream);

  if (!pb_encode(&stream, GuidanceData_fields, &guidanceData)){
    Serial.printf("encode fail: %s\n", PB_GET_ERROR(&stream));
    return;
  }
  delimitData(stream);
  
  if (!pb_encode(&stream, CmdData_fields, &cmdData)){
    Serial.printf("encode fail: %s\n", PB_GET_ERROR(&stream));
    return;
  }
  delimitData(stream);
  
  if (!pb_encode(&stream, ImuData_fields, &sensorData.imuData)){
    Serial.printf("encode fail: %s\n", PB_GET_ERROR(&stream));
    return;
  }
  delimitData(stream);
  
  for (int i=0; i<4; i++){
    if (!pb_encode(&stream, TofData_fields, &sensorData.tofData[i])){
      Serial.printf("encode fail: %s\n", PB_GET_ERROR(&stream));
      return;
    }
    if (i<3){
      delimitData(stream);
    }
  }
}

void TcpServer::update(){
  if (!client){
    client = server.available();
  }
  if (client) {
    if (!alreadyConnected) {
      client.flush(); // clear out the input buffer
      Serial.println("New client");
      alreadyConnected = true;
      return;
    }
    else{
      int receiveBytes = client.available();
      // Serial.print("receiveBytes:");
      // Serial.print(receiveBytes);
      if (receiveBytes > 0){ // input available, update cmd
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
        // Serial.print("RunState: "); Serial.println(cmd.run_state);
      }

      // output
      // Serial.print("send.");
      pb_ostream_t stream;
      uint8_t buffer[OUTPUT_BUF_SIZE];
      stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
      serializeData(stream);
      client.write(buffer, stream.bytes_written);
      Serial.print("written:"); Serial.println(stream.bytes_written);
      client.flush();
    }
  }
  else{
    Serial.println("unavailable");
  }
}
