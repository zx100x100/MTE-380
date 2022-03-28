#include "math_utils.h"
#include "Arduino.h"

float rad2deg(float rad){
  return rad * 180 / PI;
}

float deg2rad(float deg){
  return deg / 180 * PI;
}

float cosd(float deg){
  return cos(deg2rad(deg));
}

float sind(float deg){
  return sin(deg2rad(deg));
}


bool float_eq(float num1, float num2) {
      float diff = num1 - num2;
      return (diff < EPSILON) && ((diff*(-1)) < EPSILON);
}

bool float_le(float num1, float num2) {
      float diff = num1 - num2;
      return (abs(diff) > EPSILON) && (diff < 0);
}

bool float_ge(float num1, float num2) {
      float diff = num1 - num2;
      return (abs(diff) > EPSILON) && (diff > 0);
}

// constrainVal value to within + or - maximum
float constrainVal(float val, float maximum){
  if (val > 0){
    if (val > maximum){
      return maximum;
    }
    return val;
  }
  if (val < -maximum){
    return -maximum;
  }
  return val;
}
