// TemperaturePredictor.cpp
#include "TemperaturePredictor.h"
#include <numeric>
#include <cmath>
#include <iostream>

//Constructor that initializes the historical data.
 
TemperaturePredictor::TemperaturePredictor(const std::vector<Candlestick>& historicalData) : data(historicalData) {}

//Calculates the slope (m) and intercept (c) for linear regression.

void TemperaturePredictor::calculateLinearRegression(double& m, double& c) {
    int n = data.size();
    if (n == 0) {
        throw std::runtime_error("No data available for prediction.");
    }
    
    std::vector<double> years;
    std::vector<double> temperatures;
    
    for (const auto& candle : data) {
        years.push_back(std::stod(candle.date));
        temperatures.push_back(candle.close);
    }
    
    double sum_x = std::accumulate(years.begin(), years.end(), 0.0);
    double sum_y = std::accumulate(temperatures.begin(), temperatures.end(), 0.0);
    double sum_xy = 0.0;
    double sum_x2 = 0.0;
    
    for (int i = 0; i < n; ++i) {
        sum_xy += years[i] * temperatures[i];
        sum_x2 += years[i] * years[i];
    }
    
    double denominator = n * sum_x2 - sum_x * sum_x;
    if (denominator == 0) {
        throw std::runtime_error("Denominator in linear regression calculation is zero.");
    }
    
    m = (n * sum_xy - sum_x * sum_y) / denominator;
    c = (sum_y * sum_x2 - sum_x * sum_xy) / denominator;
}

// Predicts temperatures for a given range of years using linear regression.

std::vector<Candlestick> TemperaturePredictor::predictTemperatures(int startYear, int endYear) {
    std::vector<Candlestick> predictions;
    
    double m, c;
    calculateLinearRegression(m, c);
    
    for (int year = startYear; year <= endYear; ++year) {
        double predictedClose = m * year + c;
        // For simplicity, set open = close = predictedClose, high = predictedClose + 1, low = predictedClose - 1
        double predictedOpen = predictedClose;
        double predictedHigh = predictedClose + 1.0;
        double predictedLow = predictedClose - 1.0;
        
        predictions.emplace_back(std::to_string(year), predictedOpen, predictedClose, predictedHigh, predictedLow);
    }
    
    return predictions;
}
