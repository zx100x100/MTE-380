#ifndef DRIVE_H
#define DRIVE_H

#include <stdint.h>
#import <Arduino.h>


typedef struct {
  uint8_t left_power;
  uint8_t right_power;
} drive_command_t;

void drive_init(int left_drive_pin, int right_drive_pin);

void set_drive(drive_command_t drive_cmd);


#endif
