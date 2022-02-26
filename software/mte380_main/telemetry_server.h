#ifndef TELEMETRY_SERVER_H
#define TELEMETRY_SERVER_H

#include <WiFi.h>

#include "nav_data.pb.h"
#include "cmd_data.pb.h"
#include "guidance_data.pb.h"
#include "sensors.h"
#include "hms.h"

class TelemetryServer{
  private:
    const char* ssid = "EMU";
    /* const char* ssid = "Heatspeet"; */
    /* const char* password = "alloneword"; */
    const char* password = "12341234";
    WiFiServer server = WiFiServer(23);
    WiFiClient client;
    bool alreadyConnected = false;

    IPAddress localIP = IPAddress(192, 168, 183, 111); // static IP of esp
    IPAddress gateway = IPAddress(192, 168, 183, 1); // gateway IP

    IPAddress subnet = IPAddress(255, 255, 0, 0);
    IPAddress primaryDNS = IPAddress(8, 8, 8, 8);   // optional
    IPAddress secondaryDNS = IPAddress(8, 8, 4, 4); // optional

    Sensors& sensors;
    NavData& navData;
    GuidanceData& guidanceData;
    CmdData& cmdData;
    Hms* hms;

    void serializeData(pb_ostream_t& stream);

  public:
    TelemetryServer(Sensors& sensors,
              NavData& navData,
              GuidanceData& guidanceData,
              CmdData& cmdData,
              Hms* hms);
    void init();
    void update();
};

#endif
