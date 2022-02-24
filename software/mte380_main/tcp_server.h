#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <WiFi.h>

#include "nav_data.pb.h"
#include "cmd_data.pb.h"
#include "guidance_data.pb.h"
#include "sensors.h"

class TcpServer{
  private:
    const char* ssid = "K";
    const char* password = "12341234";
    WiFiServer server = WiFiServer(23);
    WiFiClient client;
    bool alreadyConnected = false;

    IPAddress localIP = IPAddress(192, 168, 86, 111); // static IP of esp
    IPAddress gateway = IPAddress(192, 168, 86, 1); // gateway IP

    IPAddress subnet = IPAddress(255, 255, 0, 0);
    IPAddress primaryDNS = IPAddress(8, 8, 8, 8);   // optional
    IPAddress secondaryDNS = IPAddress(8, 8, 4, 4); // optional

    Sensors& sensors;
    NavData& navData;
    GuidanceData& guidanceData;
    CmdData& cmdData;

    void serializeData(pb_ostream_t& stream);

  public:
    TcpServer();
    TcpServer(Sensors& sensors,
              NavData& navData,
              GuidanceData& guidanceData,
              CmdData& cmdData);
    void update();
};

#endif
