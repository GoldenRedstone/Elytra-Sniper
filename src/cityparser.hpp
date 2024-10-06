#ifndef CITYPARSER_H
#define CITYPARSER_H

#include <string>
#include <vector>
#include <stdint.h>

#define PROJECT_DIR(x) (std::string(PROJECT_SOURCE_DIR) + x)

/*
 * Dataclass representing one end city.
 * x, z (uint64_t): The coordinates of the city.
 * hasShip (bool): Whether the city includes a ship and elytra.
 * looted (bool): Whether the city has previously been looted.
 */
class CityLocation {
public:
    int x;
    int z;
    bool hasShip;
    bool looted;
};

/*
 * Parses a single CSV file and returns a vector of the cities stored in it.
 */
std::vector<CityLocation> parseCSVFile(std::string filename);

/*
 * Determines the best CSVs to read and parses them.
 Combines results into a vector which is returned.
 */
std::vector<CityLocation> readCitiesAround(uint64_t seed, int x, int z);

/*
 * Determines the best CSVs to read and parses them.
 Combines results into a vector which is returned.
 */
std::vector<CityLocation> filterCities(std::vector<CityLocation> cities,
    bool mustHaveShip, bool mustBeUnexplored);
/*
 * Filters cities on either the existance of a ship, not yet visited, or both.
 */

void markCityLooted(uint64_t seed, int64_t x, int64_t z);
/*
 * Marks a single city at the provided coordinates as looted.
 */

#endif /* CITYPARSER_H */
