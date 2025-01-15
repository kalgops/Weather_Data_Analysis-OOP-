    // main.cpp
    #include "Candlestick.h"
    #include "CSVReader.h"
    #include "ComputeCandlesticks.h"
    #include "DataFilter.h"
    #include "PlotCandlesticks.h"
    #include "TemperaturePredictor.h"
    #include <iostream>
    #include <vector>
    #include <string>
    #include <fstream>
    #include <sstream>
    #include <map>
    #include <iomanip>
    #include <limits>

   
    std::vector<std::string> getColumnNames(const std::string& filename) {
        std::ifstream file(filename);
        std::vector<std::string> columnNames;

        if (file.is_open()) {
            std::string headerLine;
            std::getline(file, headerLine); // Read the first line
            std::stringstream ss(headerLine);
            std::string column;

            while (std::getline(ss, column, ',')) {
                columnNames.push_back(column);
            }
        } else {
            throw std::runtime_error("Unable to open file: " + filename);
        }

        return columnNames;
    }


    //  Dynamically extracts country columns from the dataset.
    //  Vector of column names from the dataset.
    //   Map of menu index to column name (e.g., "AT_temperature").
     
    std::map<int, std::string> extractCountryColumns(const std::vector<std::string>& columnNames) {
        std::map<int, std::string> countryMenu;
        int index = 1;

        for (const auto& column : columnNames) {
            // Only include columns ending with "_temperature"
            if (column.find("_temperature") != std::string::npos) {
                countryMenu[index++] = column;
            }
        }

        if (countryMenu.empty()) {
            throw std::runtime_error("No country temperature columns found in the CSV file.");
        }

        return countryMenu;
    }

    // Converts a column name into a user-friendly country name.
    //  Column name from the dataset (e.g., "AT_temperature").
    //  Formatted country name (e.g., "Austria Temperature").
     
    std::string formatCountryName(const std::string& columnName) {
        std::string countryCode = columnName.substr(0, columnName.find("_"));
        std::string countryName = countryCode; // Default to code if not mapped

        // Example map of country codes to full names (add more if needed)
        static std::map<std::string, std::string> countryNameMap = {
            {"AT", "Austria"},
            {"BE", "Belgium"},
            {"BG", "Bulgaria"},
            {"CH", "Switzerland"},
            {"CZ", "Czech Republic"},
            {"DE", "Germany"},
            {"DK", "Denmark"},
            {"EE", "Estonia"},
            {"ES", "Spain"},
            {"FI", "Finland"},
            {"FR", "France"},
            {"GB", "United Kingdom"},
            {"GR", "Greece"},
            {"HR", "Croatia"},
            {"HU", "Hungary"},
            {"IE", "Ireland"},
            {"IT", "Italy"},
            {"LT", "Lithuania"},
            {"LU", "Luxembourg"},
            {"LV", "Latvia"},
            {"NL", "Netherlands"},
            {"NO", "Norway"},
            {"PL", "Poland"},
            {"PT", "Portugal"},
            {"RO", "Romania"},
            {"SE", "Sweden"},
            {"SI", "Slovenia"},
            {"SK", "Slovakia"}
        };

        if (countryNameMap.find(countryCode) != countryNameMap.end()) {
            countryName = countryNameMap[countryCode];
        }

        return countryName + " Temperature";
    }

    std::string selectCountry(const std::map<int, std::string>& countryMenu) {
        while (true) {
            std::cout << "\nAvailable Countries:\n";

            for (const auto& it : countryMenu) {
                std::cout << it.first << ". " << formatCountryName(it.second) << std::endl;
            }

            std::cout << "Enter the number of the country: ";
            int choice;
            std::cin >> choice;

            if (std::cin.fail()) {
                std::cin.clear(); // Clear the error flags
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                std::cout << "Invalid input. Please enter a valid number.\n";
                continue;
            }

            if (countryMenu.find(choice) != countryMenu.end()) {
                return countryMenu.at(choice);
            } else {
                std::cout << "Invalid selection. Please try again.\n";
            }
        }
    }

    // Displays candlestick data in a tabular format.

    void displayCandlesticksAsTable(const std::vector<Candlestick>& candlesticks) {
        // Print header
        std::cout << std::setw(15) << "Year"
                << std::setw(10) << "Open"
                << std::setw(10) << "High"
                << std::setw(10) << "Low"
                << std::setw(10) << "Close" << std::endl;

        // Print separator line
        std::cout << std::string(55, '-') << std::endl;

        // Print each candlestick row
        for (const auto& candle : candlesticks) {
            std::cout << std::setw(15) << candle.date
                    << std::setw(10) << std::fixed << std::setprecision(3) << candle.open
                    << std::setw(10) << candle.high
                    << std::setw(10) << candle.low
                    << std::setw(10) << candle.close << std::endl;
        }
    }

    // Prompts the user for filter criteria and filters the candlestick data accordingly.
    
    std::vector<Candlestick> filterCandlesticks(const std::vector<Candlestick>& candlesticks) {
        std::vector<Candlestick> filtered;
        
        bool filterDate = false, filterTemp = false;
        int startYear = 1980, endYear = 2019;
        double minTemp = -1000.0, maxTemp = 1000.0;
        
        // Ask user if they want to filter by date range
        std::cout << "Do you want to filter by date range? (y/n): ";
        char choice;
        std::cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            filterDate = true;
            std::cout << "Enter start year (1980-2019): ";
            std::cin >> startYear;
            std::cout << "Enter end year (1980-2019): ";
            std::cin >> endYear;
            
            // Validate years
            if (startYear < 1980 || startYear > 2019 || endYear < 1980 || endYear > 2019 || startYear > endYear) {
                std::cerr << "Invalid year range. Please ensure 1980 <= start year <= end year <= 2019.\n";
                // Reset to default
                startYear = 1980;
                endYear = 2019;
                filterDate = false;
            }
        }
        
        // Ask user if they want to filter by closing temperature range
        std::cout << "Do you want to filter by closing temperature range? (y/n): ";
        std::cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            filterTemp = true;
            std::cout << "Enter minimum closing temperature: ";
            std::cin >> minTemp;
            std::cout << "Enter maximum closing temperature: ";
            std::cin >> maxTemp;
            
            // Validate temperatures
            if (minTemp > maxTemp) {
                std::cerr << "Invalid temperature range. Minimum temperature cannot be greater than maximum temperature.\n";
                // Reset to default
                minTemp = -1000.0;
                maxTemp = 1000.0;
                filterTemp = false;
            }
        }
        
        // Apply filters using DataFilter functions
        std::vector<Candlestick> tempFiltered = candlesticks;
        
        if (filterDate) {
            tempFiltered = filterByYearRange(tempFiltered, startYear, endYear);
        }
        
        if (filterTemp) {
            tempFiltered = filterByClosingTemperatureRange(tempFiltered, minTemp, maxTemp);
        }
        
        filtered = tempFiltered;
        
        // Debug output
        std::cout << "Debug: Filtered " << filtered.size() << " candlesticks after applying filters.\n";
        return filtered;
    }

    // Handles the filtering and plotting functionality.
    
    void filterAndPlot(const std::map<int, std::string>& countryMenu, const std::string& filename) {
        try {
            // Select Country
            std::string selectedColumn = selectCountry(countryMenu);
            std::cout << "You selected: " << formatCountryName(selectedColumn) << std::endl;

            // Read CSV data for the selected country
            std::vector<std::pair<std::string, double>> data = CSVReader::readCSV(filename, selectedColumn);

            // Compute Candlesticks
            std::vector<Candlestick> candlesticks = computeCandlesticks(data);

            // Apply Filters
            std::vector<Candlestick> filteredCandlesticks = filterCandlesticks(candlesticks);

            if (filteredCandlesticks.empty()) {
                std::cout << "No data matches the specified filters.\n";
                return;
            }

            // Display filtered candlesticks
            std::cout << "\nFiltered Candlestick Data:\n";
            displayCandlesticksAsTable(filteredCandlesticks);

            // Plot filtered candlesticks
            std::cout << "\nPlotting Filtered Candlestick Data:\n";
            plotCandlesticks(filteredCandlesticks, 20);  // Adjust scaleHeight as needed

        } catch (const std::exception& e) {
            std::cerr << "Error during filtering and plotting: " << e.what() << std::endl;
        }
    }

    int main() {
        const std::string filename = "weather_data.csv";
        std::vector<std::string> columnNames;
        std::map<int, std::string> countryMenu;
        std::vector<std::pair<std::string, double>> data;
        std::vector<Candlestick> candlesticks;
        std::string selectedColumn;

        try {
            // Extract column names from the CSV file
            columnNames = getColumnNames(filename);

            // Dynamically extract country columns
            countryMenu = extractCountryColumns(columnNames);

            // Menu Loop
            while (true) {
                // Display Menu
                std::cout << "\nWeather Analysis Menu\n";
                std::cout << "1. Select Country and Display Table\n";
                std::cout << "2. Plot Candlestick Data\n";
                std::cout << "3. Filter Plot Data\n";
                std::cout << "4. Predict Temperature Changes\n"; // New option
                std::cout << "5. Exit\n"; // Updated Exit option number
                std::cout << "Enter your choice: ";

                int choice;
                std::cin >> choice;

                // Clear cin fail state if any non-integer is entered
                if (std::cin.fail()) {
                    std::cin.clear(); // clear the error flags
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard invalid input
                    std::cout << "Invalid input. Please enter a number between 1 and 5.\n";
                    continue;
                }

                switch (choice) {
                    case 1:
                        try {
                            selectedColumn = selectCountry(countryMenu);
                            std::cout << "You selected: " << formatCountryName(selectedColumn) << std::endl;

                            // Read and compute candlesticks for the selected country
                            data = CSVReader::readCSV(filename, selectedColumn);
                            candlesticks = computeCandlesticks(data);

                            // Display computed candlesticks immediately (Tabular Format Only)
                            std::cout << "\nCandlestick Data:\n";
                            displayCandlesticksAsTable(candlesticks);
                        } catch (const std::exception& e) {
                            std::cerr << "Error: " << e.what() << std::endl;
                        }
                        break;

                    case 2:
                        if (candlesticks.empty()) {
                            std::cout << "No candlestick data available. Please select a country first.\n";
                        } else {
                            std::cout << "\nPlotting Candlestick Data:\n";
                            plotCandlesticks(candlesticks, 20);  // Adjust scaleHeight as needed
                        }
                        break;

                    case 3:
                        if (candlesticks.empty()) {
                            std::cout << "No candlestick data available. Please select a country first.\n";
                        } else {
                            filterAndPlot(countryMenu, filename);
                        }
                        break;

                    case 4:
                        if (candlesticks.empty()) {
                            std::cout << "No candlestick data available. Please select a country first.\n";
                        } else {
                            try {
                                // Prompt user for prediction range
                                int predStartYear, predEndYear;
                                std::cout << "Enter the start year for prediction (e.g., 2020): ";
                                std::cin >> predStartYear;
                                std::cout << "Enter the end year for prediction (e.g., 2025): ";
                                std::cin >> predEndYear;
                                
                                if (predStartYear > predEndYear) {
                                    std::cerr << "Invalid range. Start year must be less than or equal to end year.\n";
                                    break;
                                }
                                
                                // Initialize TemperaturePredictor with historical data
                                TemperaturePredictor predictor(candlesticks);
                                
                                // Predict temperatures
                                std::vector<Candlestick> predictions = predictor.predictTemperatures(predStartYear, predEndYear);
                                
                                // Display predictions
                                std::cout << "\nPredicted Candlestick Data:\n";
                                displayCandlesticksAsTable(predictions);
                                
                                // Plot predictions
                                std::cout << "\nPlotting Predicted Candlestick Data:\n";
                                plotCandlesticks(predictions, 20);
                                
                            } catch (const std::exception& e) {
                                std::cerr << "Error during prediction: " << e.what() << std::endl;
                            }
                        }
                        break;

                    case 5:
                        std::cout << "Exiting Weather Analysis...\n";
                        return 0;

                    default:
                        std::cout << "Invalid choice. Please try again.\n";
                        break;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        return 0;
    }
