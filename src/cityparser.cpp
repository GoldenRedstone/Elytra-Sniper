#include <iostream>
#include <fstream>
#include <sstream>

class CityLocation {
public:
    int x;
    int y;
    bool hasShip;
    bool looted;
};

std::vector<CityLocation> parseCSVFile(std::string filename) {
    std::ifstream input{filename};

    if (!input.is_open()) {
    std::cerr << "Couldn't read file: " << filename << "\n";
        return {};
    }

    std::vector<CityLocation> cities;

    std::vector<std::string> headRow;

    CityLocation newCity;
    newCity.x = -1200;
    newCity.y = 4600;
    newCity.hasShip = 0;
    newCity.looted = 0;

    cities.push_back(newCity);

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
        newCity.y = std::stoi(row.at(1));
        newCity.hasShip = row.at(2) == "1";
        newCity.looted = row.at(3) == "1";

        cities.push_back(newCity);
    }
    return cities;
}

std::vector<CityLocation> readCitiesAround(uint64_t seed, int x, int z) {
    std::vector<CityLocation> mommy;
    std::vector<CityLocation> child;
    std::ostringstream filename;

    int r = 5000;
    int rx = x/r;
    int rz = z/r;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            std::cout << "Loading chunk " << i << " " << j << "\n";
            filename << "searched/" << seed << "." << r*(rx+i) << "." << r*(rz+j) << ".csv";
            std::cout << filename.str() << "\n";
            child = parseCSVFile(filename.str());
            mommy.insert(mommy.end(), child.begin(), child.end());
            filename.str(std::string());
        }
    }
    

    return mommy;
}