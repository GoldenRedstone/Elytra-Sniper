#ifndef CITYPARSER_H
#define CITYPARSER_H

#include <string>
#include <vector>
#include <stdint.h>

#define PROJECT_DIR(x) (std::string(PROJECT_SOURCE_DIR) + x)

class CityLocation {
public:
    int x;
    int z;
    bool hasShip;
    bool looted;
};

std::vector<CityLocation> parseCSVFile(std::string filename);
/*
 * Parses a single CSV file and returns a vector of the cities stored in it.
 */

std::vector<CityLocation> readCitiesAround(uint64_t seed, int x, int z);
/*
 * 
 */

std::vector<CityLocation> filterCities(std::vector<CityLocation> cities, bool mustHaveShip, bool mustBeUnexplored);
/*
 * Filters cities on either the existance of a ship, not yet visited, or both.
 */

void markCityLooted(uint64_t seed, int64_t x, int64_t z);
/*
 * Marks a single city at the provided coordinates as looted.
 */

#endif /* CITYPARSER_H */
