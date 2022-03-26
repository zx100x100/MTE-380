#include <Arduino.h>
#include <proto/pb_common.h>
#include <proto/pb.h>
#include <proto/pb_encode.h>
#include <proto/pb_decode.h>

#include "imu_data.pb.h"
#include "tof_data.pb.h"
#include "telemetry_server.h"
#include "hms_and_cmd_data.pb.h"

#define DEAD_MAN_TIMEOUT_MS 2000
#define CMD_BUF_SIZE 600
#define OUTPUT_BUF_SIZE 1200
#define INFREQUENT_TELEMETRY_INTERVAL 100 // every 100 ticks, send back telem

const uint8_t delimit[3] = {uint8_t(':'),uint8_t(':'),uint8_t(':')};
const uint8_t delimitEnd[3] = {uint8_t(';'),uint8_t(';'),uint8_t(';')};
const uint8_t delimitStart[3] = {uint8_t('('),uint8_t('('),uint8_t('(')};

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
    lastCommandTime = micros();
    beforeReceiveT = micros();
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
  hms->greenLedState = LED_SLOW_FLASH;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
    if (wifiConnectionTicks++ >= maxWifiConnectionTicksBeforeReboot){
      ESP.restart();
    }
    hms->updateLEDs();
  }
  Serial.println(WiFi.localIP());
  server.begin();
  hms->greenLedState = LED_FAST_FLASH;
}

void delimitData(pb_ostream_t& stream){
  if (!pb_write(&stream, delimit, 3)){
    Serial.printf("write fail: %s\n", PB_GET_ERROR(&stream));
    return;
  }
}

void delimitMessageEnd(pb_ostream_t& stream){
  if (!pb_write(&stream, delimitEnd, 3)){
    Serial.printf("write fail: %s\n", PB_GET_ERROR(&stream));
    return;
  }
}

void delimitMessageStart(pb_ostream_t& stream){
  if (!pb_write(&stream, delimitStart, 3)){
    Serial.printf("write fail: %s\n", PB_GET_ERROR(&stream));
    return;
  }
}

void TelemetryServer::serializeData(pb_ostream_t& stream){
  delimitMessageStart(stream);
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
  delimitMessageEnd(stream);
}

bool TelemetryServer::update(){
  if (hms->data.mainLogLevel >= 2) Serial.println("TelemetryServer::update()");
  // if (millis() - lastCommandTime/1000 > DEAD_MAN_TIMEOUT_MS){
    /* Serial.println("TIMEOUT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"); */
    // cmdData.runState = CmdData_RunState_E_STOP;
    // cmdData.leftPower = 0;
    // cmdData.rightPower = 0;
    // cmdData.propPower = 0;
  // }
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
      hms->greenLedState = LED_ON;
      return false;
    }
    else{
      hms->greenLedState = LED_ON;
      // TODO CHANGE THIS BEHAVIOR OR AT LEAST MAKE SURE DOING IT THIS WAY ISNT MAKING TICKRATES LESS CONSISTENT
      int receiveBytes = client.available();
      if (receiveBytes > 0){ // input available, update cmd, send back telemetry
        if (hms->data.mainLogLevel >= 2){Serial.println("Received cmd data");}
        beforeReceiveT = micros();
        // RECEIVE DATA -----------------------------------
        uint8_t inputBuffer[CMD_BUF_SIZE];
        int seps = 0;
        bool encounteredMessageProblem = false;
        while (true){
          if (!client.available()){
            encounteredMessageProblem = true;
            break;
          }
          uint8_t thisChar = uint8_t(client.read());
          if (thisChar == delimit[0]){
            seps++;
          }
          else{
            Serial.print("x");
          }
          if (seps == 3){
            Serial.println("found valid message start");
            break;
          }
        }
        int i = 0;
        seps = 0;
        if (!encounteredMessageProblem){
          encounteredMessageProblem = true;
          while (client.available() > 0){
            // read the bytes incoming from the client:
            uint8_t thisChar = uint8_t(client.read());
            if (thisChar == delimit[0]){
              seps++;
            }
            else{
              seps = 0;
            }
            if (seps == 3){
              Serial.println("found valid message end");
              i -= 2;
              encounteredMessageProblem = false;
              break;
            }
            inputBuffer[i++] = thisChar;
            if (i >= CMD_BUF_SIZE){
              Serial.println("Received too much data!!!!!");
              client.flush();
              return false;
            }
          }
          if (!encounteredMessageProblem){
            // client.flush();
            pb_istream_t instream = pb_istream_from_buffer(inputBuffer, i);
            bool decodeStatus = pb_decode(&instream, CmdData_fields, &cmdData);
            if (!decodeStatus){
              Serial.printf("Decoding Cmd fail: %s\n", PB_GET_ERROR(&instream));

              for (int i=0; i<CMD_BUF_SIZE; i++){
                Serial.print(char(inputBuffer[i]));
              }
              Serial.println();
              // delay(2000);
              // TODO
              //
              // ADD THIS TO HSM ERROR LOGGER!!!!!!!!!!!
              //
              //
              return false;
            }
            hms->data.mainLogLevel = HmsData_LogLevel(cmdData.mainLogLevel);
            hms->data.guidanceLogLevel = HmsData_LogLevel(cmdData.guidanceLogLevel);
            hms->data.navLogLevel = HmsData_LogLevel(cmdData.navLogLevel);
            hms->data.sensorsLogLevel = HmsData_LogLevel(cmdData.sensorsLogLevel);
            guidanceData.kP_vel = cmdData.kP_vel;
            guidanceData.kD_vel = cmdData.kD_vel;
            // if(hms->data.guidanceLogLevel >= 2){ Serial.print("guidanceData.kD_vel: "); Serial.println(guidanceData.kD_vel); }
            guidanceData.kI_vel = cmdData.kI_vel;
            guidanceData.kP_drift = cmdData.kP_drift;
            guidanceData.kD_drift = cmdData.kD_drift;
            guidanceData.kI_drift = cmdData.kI_drift;
            // client.flush();
          }
        }
      }
      if (!cmdData.disableTelemetry){
        if(hms->data.mainLogLevel >= 2){ Serial.println("Sending telemetry"); }
          // (cmdData.telemetryMode == CmdData_TelemetryMode_INFREQUENT && hms->data.nTicks % INFREQUENT_TELEMETRY_INTERVAL == 0)){
        // SEND DATA --------------------------------------
        pb_ostream_t stream;
        // pb_ostream_t sizeStream;
        uint8_t buffer[OUTPUT_BUF_SIZE];
        // uint8_t sizeBuffer[4];
        stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        // sizeStream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        serializeData(stream);
        unsigned long beforeSendT = micros();
        client.write(buffer, stream.bytes_written); // takes 0-2 ms
        // client.flush();
      }
      unsigned long newTimestamp = micros();
      lastCommandTime = newTimestamp;
      return true;
    }
  }
  else{
    if (hms->data.mainLogLevel >= 1){
      Serial.println("No TCP client connected.");
    }
    hms->greenLedState = LED_SLOW_FLASH;
    // DEAD MANS SWITCH!
    return false;
  }
}
