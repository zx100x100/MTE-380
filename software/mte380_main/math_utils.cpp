#include "math_utils.h"
#include "Arduino.h"

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

