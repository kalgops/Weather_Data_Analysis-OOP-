// Utils.h
#pragma once
#include <cmath>
#include <algorithm>

//Custom implementation of clamp for C++11.
inline int clamp(int value, int minValue, int maxValue) {
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

//Normalize a value to fit within the scale height.
inline int normalize(double value, double minValue, double maxValue, int scaleHeight) {
    if (maxValue == minValue) return 0; // Prevent division by zero
    double normalized = (value - minValue) / (maxValue - minValue) * (scaleHeight - 1);
    return clamp(static_cast<int>(std::round(normalized)), 0, scaleHeight - 1);
}