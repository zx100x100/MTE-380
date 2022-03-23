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
