// PlotCandlesticks.cpp
#include "PlotCandlesticks.h"
#include "Utils.h" // For clamp and normalize
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <limits>

//Plots candlestick data in a text-based format with spacing aligned to x-axis labels.

void plotCandlesticks(const std::vector<Candlestick>& candlesticks, int scaleHeight) {
    if (candlesticks.empty()) {
        std::cerr << "No candlestick data to plot." << std::endl;
        return;
    }

    // Determine global min and max temperatures
    double globalMin = candlesticks[0].low;
    double globalMax = candlesticks[0].high;

    for (const auto& candle : candlesticks) {
        globalMin = std::min(globalMin, candle.low);
        globalMax = std::max(globalMax, candle.high);
    }

    // Adjust scaleHeight if necessary
    scaleHeight = std::max(scaleHeight, 10);

    // Pagination setup
    const int pageSize = 20;
    int totalCandlesticks = candlesticks.size();
    int totalPages = (totalCandlesticks + pageSize - 1) / pageSize;

    for (int currentPage = 1; currentPage <= totalPages; ++currentPage) {
        int startIdx = (currentPage - 1) * pageSize;
        int endIdx = std::min(startIdx + pageSize, totalCandlesticks);

        // Subset of candlesticks for the current page
        std::vector<Candlestick> subset(candlesticks.begin() + startIdx, candlesticks.begin() + endIdx);

        // Print the plot from top (max) to bottom (min)
        for (int row = scaleHeight - 1; row >= 0; --row) {
            double currentTemp = globalMin + (globalMax - globalMin) * row / (scaleHeight - 1);
            std::cout << std::setw(6) << std::fixed << std::setprecision(1) << currentTemp << " | ";

            for (const auto& candle : subset) {
                int highPos = normalize(candle.high, globalMin, globalMax, scaleHeight);
                int lowPos = normalize(candle.low, globalMin, globalMax, scaleHeight);
                int openPos = normalize(candle.open, globalMin, globalMax, scaleHeight);
                int closePos = normalize(candle.close, globalMin, globalMax, scaleHeight);

                if (row == highPos && row == lowPos) {
                    std::cout << "|    ";
                } else if (row == highPos) {
                    std::cout << "|    ";
                } else if (row == lowPos) {
                    std::cout << "|    ";
                } else if (row == openPos && row == closePos) {
                    std::cout << "=    ";
                } else if (row == openPos) {
                    std::cout << "+    ";
                } else if (row == closePos) {
                    std::cout << "-    ";
                } else if (row < highPos && row > lowPos) {
                    std::cout << "|    ";
                } else {
                    std::cout << "     ";  // Add appropriate spacing
                }
            }
            std::cout << std::endl;
        }

        // Print the x-axis separator
        std::cout << "       " << std::string(subset.size() * 5, '-') << std::endl;

        // Print the years below the plot with spacing aligned
        std::cout << "       ";
        for (const auto& candle : subset) {
            std::cout << std::setw(5) << candle.date.substr(0, 4) << " ";
        }
        std::cout << std::endl;

        // If there are more pages, prompt the user to continue
        if (currentPage < totalPages) {
            std::cout << "\nPress Enter to view the next page...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer
            std::cin.get();    // Wait for Enter
        }
    }
}
