#include "drive.h"

int left_drive, right_drive;

void drive_init(int left_drive_pin, int right_drive_pin){
  left_drive = left_drive_pin;
  right_drive = right_drive_pin;

  pinMode(left_drive, OUTPUT);
  pinMode(right_drive, OUTPUT);
  
}
void set_drive(drive_command_t drive_cmd){
  analogWrite(left_drive, drive_cmd.left_power);
  analogWrite(right_drive, drive_cmd.right_power);
}
