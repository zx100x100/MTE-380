#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#define EPSILON 0.0000001

bool float_eq(float,float);
bool float_le(float,float);
bool float_ge(float,float);
float constrainVal(float val, float maximum);
template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}
float deg2rad(float deg);
float rad2deg(float rad);
float cosd(float deg);
float sind(float deg);

#endif
