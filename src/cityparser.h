#ifndef CITYPARSER_H
#define CITYPARSER_H

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

std::vector<CityLocation> parseCSVFile(std::string filename);

std::vector<CityLocation> readCitiesAround(uint64_t seed, int x, int z);

#endif /* CITYPARSER_H */