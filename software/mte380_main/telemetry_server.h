#ifndef TELEMETRY_SERVER_H
#define TELEMETRY_SERVER_H

#include <WiFi.h>

#include "nav_data.pb.h"
#include "cmd_data.pb.h"
#include "guidance_data.pb.h"
#include "sensors.h"
#include "hms.h"
#include "network_info.h"

class TelemetryServer{
  private:
    const char* ssid = WIFI_SSID;
    const char* password = WIFI_PASSWORD;
    WiFiServer server = WiFiServer(SERVER_PORT);
    WiFiClient client;
    bool alreadyConnected = false;

    IPAddress localIP = IPAddress(SERVER_BYTE0, SERVER_BYTE1, SERVER_BYTE2, SERVER_BYTE3); // static IP of esp
    IPAddress gateway = IPAddress(SERVER_BYTE0, SERVER_BYTE1, SERVER_BYTE2, 1); // gateway IP

    IPAddress subnet = IPAddress(255, 255, 0, 0);
    IPAddress primaryDNS = IPAddress(8, 8, 8, 8);   // optional
    IPAddress secondaryDNS = IPAddress(8, 8, 4, 4); // optional

    Sensors& sensors;
    NavData& navData;
    GuidanceData& guidanceData;
    CmdData& cmdData;
    Hms* hms;

    unsigned long lastCommandTime; 

    void serializeData(pb_ostream_t& stream);

  public:
    TelemetryServer(Sensors& sensors,
              NavData& navData,
              GuidanceData& guidanceData,
              CmdData& cmdData,
              Hms* hms);
    void init();
    bool update();
    unsigned long beforeReceiveT; 
};

#endif
