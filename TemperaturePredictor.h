// TemperaturePredictor.h
#pragma once
#include "Candlestick.h"
#include <vector>
#include <string>
#include <utility>

// TemperaturePredictor class to predict future temperatures based on historical data.
class TemperaturePredictor {
public:
  // Constructor that takes historical candlestick data.
    TemperaturePredictor(const std::vector<Candlestick>& historicalData);
    // Predicts temperatures for a given range of years.
    std::vector<Candlestick> predictTemperatures(int startYear, int endYear);
private:
    std::vector<Candlestick> data;
    //  Calculates the slope (m) and intercept (c) for linear regression.
    void calculateLinearRegression(double& m, double& c);
};