#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "cityparser.hpp"


std::vector<CityLocation> parseCSVFile(std::string filename) {
    std::string path = PROJECT_DIR(filename);
    std::ifstream inputFile(path);
    
    if (!inputFile.is_open()) {
        std::cerr << "Couldn't read file: " << filename << "\n";
        return {};
    }

    std::vector<CityLocation> cities;
    std::string line;
    std::vector<std::string> headerRow;

    // Process each line in the CSV file
    while (std::getline(inputFile, line)) {
        std::istringstream lineStream(line);
        std::vector<std::string> row;

        std::string value;
        while (std::getline(lineStream, value, ',')) {
            row.push_back(value);
        }

        // Skip the header row
        if (headerRow.empty()) {
            headerRow = row;
            continue;
        }

        // Populate CityLocation struct from the row
        CityLocation newCity;
        newCity.x = std::stoi(row.at(0));
        newCity.z = std::stoi(row.at(1));
        newCity.hasShip = row.at(2) == "1";
        newCity.looted = row.at(3) == "1";

        cities.push_back(newCity);
    }

    inputFile.close();
    return cities;
}

// Load city locations around a given seed and coordinates
std::vector<CityLocation> readCitiesAround(uint64_t seed, int x, int z) {
    std::vector<CityLocation> mommy;
    std::ostringstream filename;
    const int regionSize = 5000;

    int rx = (x / regionSize) - (x < 0);
    int rz = (z / regionSize) - (z < 0);

    // Check adjacent regions (3x3 grid)
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            filename << "searched/" << seed << "." << regionSize * (rx + i) 
                     << "." << regionSize * (rz + j) << ".csv";

            std::cout << "Loading chunk " << i << ", " << j << " - " << filename.str() << "\n";
            auto child = parseCSVFile(filename.str());

            mommy.insert(mommy.end(), child.begin(), child.end());

            // Reset filename for the next chunk
            filename.str(std::string());
        }
    }

    return mommy;
}

// Filter cities based on whether they have a ship and/or are unexplored
std::vector<CityLocation> filterCities(std::vector<CityLocation> cities, 
                                       bool mustHaveShip = false, 
                                       bool mustBeUnexplored = false) {
    std::vector<CityLocation> filteredCities;

    for (const CityLocation& city : cities) {
        if (mustHaveShip && !city.hasShip) {
            continue;
        }
        if (mustBeUnexplored && city.looted) {
            continue;
        }
        filteredCities.push_back(city);
    }

    return filteredCities;
}


void markCityLooted(uint64_t seed, int64_t x, int64_t z) {
    std::ostringstream filename;
    const int regionSize = 5000;

    int rx = (x / regionSize) - (x < 0);
    int rz = (z / regionSize) - (z < 0);

    filename << "searched/" << seed << "." << regionSize * rx << "." 
             << regionSize * rz << ".csv";
    
    std::cout << "Loading chunk - " << filename.str() << "\n";
    
    auto cities = parseCSVFile(filename.str());
    std::ofstream outputFile(PROJECT_DIR(filename.str()).c_str());

    if (!outputFile.is_open()) {
        std::cerr << "Couldn't write to file: " << filename.str() << "\n";
        return;
    }

    outputFile << "x,z,ship,looted\n";

    // Update the looted status of the city
    for (CityLocation& city : cities) {
        bool isCurrentCity = (city.x == x && city.z == z);
        outputFile << city.x << ',' << city.z << ',' << city.hasShip << ',' 
                   << (isCurrentCity || city.looted ? 1 : 0) << '\n';
    }

    outputFile.close();
}
