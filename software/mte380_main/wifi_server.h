#ifndef WIFI_SERVER_H
#define WIFI_SERVER_H

#include "position.pb.h"
#include "command.pb.h"

void wifi_init();
void host_server();
void server_tick(Command& command, Position& position);
#endif
