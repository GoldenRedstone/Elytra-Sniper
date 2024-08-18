#ifndef ENDCITY_FINDER_H
#define ENDCITY_FINDER_H

#include "finders.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#define PROJECT_DIR(x) (std::string(PROJECT_SOURCE_DIR) + x)

void findStructures(int structureType, int mc, int dim, uint64_t seed,
    int x0, int z0, int x1, int z1);

int findStructuresAround(uint64_t seed, int x, int z, int mc);

#endif // ENDCITY_FINDER_H
