#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "cityparser.hpp"


std::vector<CityLocation> parseCSVFile(const std::string filename) {
    std::string path = PROJECT_DIR(filename);
    std::ifstream inputFile(path);

    if (!inputFile.is_open()) {
        std::cerr << "Couldn't read file: " << filename << "\n";
        return {};
    }

    std::vector<CityLocation> cities;
    std::string line;

    // Reserve space to avoid reallocations, assuming a maximum of 100 entries
    cities.reserve(100);

    // Process each line in the CSV file
    bool isHeader = true; // Flag to skip header row
    while (std::getline(inputFile, line)) {
        if (isHeader) { // Skip header
            isHeader = false;
            continue;
        }

        std::istringstream lineStream(line);
        CityLocation newCity;

        // Use std::getline and push_back directly to newCity fields
        std::string value;
        if (std::getline(lineStream, value, ',')) newCity.x = std::stoi(value);
        if (std::getline(lineStream, value, ',')) newCity.z = std::stoi(value);
        if (std::getline(lineStream, value, ',')) newCity.hasShip = value == "1";
        if (std::getline(lineStream, value, ',')) newCity.looted = value == "1";

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
    mommy.reserve(400); // Reserve space for 400 entries (9 regions)

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            filename.str(std::string()); // Reset filename
            filename << "searched/" << seed << "." << regionSize * (rx + i) 
                     << "." << regionSize * (rz + j) << ".csv";

            std::cout << "Loading chunk " << i << ", " << j << " - " << filename.str() << "\n";
            auto child = parseCSVFile(filename.str());
            mommy.insert(mommy.end(), child.begin(), child.end());
        }
    }

    return mommy;
}

// Filter cities based on whether they have a ship and/or are unexplored
std::vector<CityLocation> filterCities(const std::vector<CityLocation> cities, 
                                       bool mustHaveShip = false, 
                                       bool mustBeUnexplored = false) {
    std::vector<CityLocation> filteredCities;

    filteredCities.reserve(cities.size()); // Reserve space for filtering

    for (const CityLocation& city : cities) {
        if ((mustHaveShip && !city.hasShip) || (mustBeUnexplored && city.looted)) {
            continue; // Skip if conditions are not met
        }
        filteredCities.push_back(city);
    }

    return filteredCities;
}


// Mark a city as looted by updating its status in the corresponding CSV file
void markCityLooted(uint64_t seed, int64_t x, int64_t z) {
    std::ostringstream filename;
    const int regionSize = 5000;

    int rx = (x / regionSize) - (x < 0);
    int rz = (z / regionSize) - (z < 0);

    filename << "searched/" << seed << "." << regionSize * rx << "." 
             << regionSize * rz << ".csv";

    std::cout << "Loading chunk - " << filename.str() << "\n";

    auto cities = parseCSVFile(filename.str());
    std::ofstream outputFile(PROJECT_DIR(filename.str()));

    if (!outputFile.is_open()) {
        std::cerr << "Couldn't write to file: " << filename.str() << "\n";
        return;
    }

    outputFile << "x,z,ship,looted\n";

    // Update the looted status of the city
    for (const CityLocation& city : cities) {
        outputFile << city.x << ',' << city.z << ',' << city.hasShip << ',' 
                   << ((city.x == x && city.z == z) || city.looted ? 1 : 0) << '\n';
    }

    outputFile.close();
}