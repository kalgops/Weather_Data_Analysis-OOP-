// DataFilter.h
#pragma once
#include "Candlestick.h"
#include <vector>
#include <string>

// Filtering by the range of the year from 1980 to 2019
std::vector<Candlestick> filterByYearRange(const std::vector<Candlestick>& candlesticks, int startYear, int endYear);

// Filtering by the opening and closing temperature
std::vector<Candlestick> filterByClosingTemperatureRange(const std::vector<Candlestick>& candlesticks, double minTemp, double maxTemp);