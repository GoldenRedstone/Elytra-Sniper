#include <iostream>
#include <fstream>
#include <sstream>

#include "cityparser.hpp"


std::vector<CityLocation> parseCSVFile(std::string filename) {
    std::string path = PROJECT_DIR(filename);
    std::ifstream input(path.c_str());
    if (!input.is_open()) {
      std::cerr << "Couldn't read file: " << filename << "\n";
      return {};
    }

    std::vector<CityLocation> cities;
    std::vector<std::string> headRow;
    
    for (std::string line; std::getline(input, line);) {
        std::istringstream ss(std::move(line));
        std::vector<std::string> row;

        for (std::string value; std::getline(ss, value, ',');) {
            row.push_back(std::move(value));
        }

        if (headRow.empty()) {
            // Skip this row
            headRow = row;
            continue;
        }

        CityLocation newCity;
        newCity.x = std::stoi(row.at(0));
        newCity.z = std::stoi(row.at(1));
        newCity.hasShip = row.at(2) == "1";
        newCity.looted = row.at(3) == "1";

        cities.push_back(newCity);
    }
    input.close();
    return cities;
}

std::vector<CityLocation> readCitiesAround(uint64_t seed, int x, int z) {
    std::vector<CityLocation> mommy;
    std::vector<CityLocation> child;
    std::ostringstream filename;

    int r = 5000;
    int rx = x/r;
    if (x < 0) {
        rx--;
    }
    int rz = z/r;
    if (z < 0) {
        rz--;
    }

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            std::cout << "Loading chunk " << i << " " << j;
            filename << "searched/" << seed << "." << r*(rx+i) << "." << r*(rz+j) << ".csv";
            std::cout << " - " << filename.str() << "\n";
            child = parseCSVFile(filename.str());
            mommy.insert(mommy.end(), child.begin(), child.end());
            filename.str(std::string());
        }
    }
    
    return mommy;
}

std::vector<CityLocation> filterCities(std::vector<CityLocation> cities, bool mustHaveShip = false, bool mustBeUnexplored = false) {
    std::vector<CityLocation> filteredCities;
    for (const CityLocation& city : cities) {
        if (!city.hasShip && mustHaveShip) {
            continue;
        }
        if (city.looted && mustBeUnexplored) {
            continue;
        }
        filteredCities.push_back(city);
    }
    return filteredCities;
}


void markCityLooted(uint64_t seed, int64_t x, int64_t z) {
    std::ostringstream filename;
    std::vector<CityLocation> cities;
    
    int r = 5000;
    int rx = x/r;
    if (x < 0) {
        rx--;
    }
    int rz = z/r;
    if (z < 0) {
        rz--;
    }

    filename << "searched/" << seed << "." << r*(rx) << "." << r*(rz) << ".csv";
    std::cout << "Loading chunk - " << filename.str() << "\n";
    cities = parseCSVFile(filename.str());

    FILE *fpt;
    fpt = fopen(PROJECT_DIR(filename.str()).c_str(), "w+");
    fprintf(fpt, "x,z,ship,looted\n");

    for (CityLocation& city : cities) {
        if ((city.x == x && city.z == z) || city.looted) {
            fprintf(fpt, "%d,%d,%d,1\n", city.x, city.z, city.hasShip);
        } else {
            fprintf(fpt, "%d,%d,%d,0\n", city.x, city.z, city.hasShip);
        }
    }
    fclose(fpt);
}