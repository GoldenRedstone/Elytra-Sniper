#include "finders.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>

#define PROJECT_DIR(x) (std::string(PROJECT_SOURCE_DIR) + x)

void findStructures(int structureType, int mc, int dim, uint64_t seed,
    int x0, int z0, int x1, int z1) {

    std::ostringstream filename;
    filename << "searched/" << seed << "." << x0 << "." << z0 << ".csv";
    std::cout << filename.str();

    if (access(PROJECT_DIR(filename.str()).c_str(), F_OK) == 0) {
        std::cout << " - File already exists\n";
        return;
    } else {
        std::cout << " - File does not exist\n";
        std::cout << "Searching for cities between x: " << x0 << "-" << x1 << ", " << z0 << "-" << z1 << "\n";
    }

    FILE* fpt;
    fpt = fopen(PROJECT_DIR(filename.str()).c_str(), "w+");
    fprintf(fpt, "x,z,ship,looted\n");

    // set up a biome generator
    Generator g;
    setupGenerator(&g, mc, 0);
    applySeed(&g, dim, seed);

    // ignore this if you are not looking for end cities
    SurfaceNoise sn;
    if (structureType == End_City)
        initSurfaceNoise(&sn, dim, seed);

    StructureConfig sconf;
    if (!getStructureConfig(structureType, mc, &sconf))
        return; // bad version or structure

    // segment area into structure regions
    double blocksPerRegion = sconf.regionSize * 16.0;
    int rx0 = (int) floor(x0 / blocksPerRegion);
    int rz0 = (int) floor(z0 / blocksPerRegion);
    int rx1 = (int) ceil(x1 / blocksPerRegion);
    int rz1 = (int) ceil(z1 / blocksPerRegion);
    int i, j;

    for (j = rz0; j <= rz1; j++) {
        for (i = rx0; i <= rx1; i++) { // check the structure generation attempt in region (i, j)
            Pos pos;
            if (!getStructurePos(structureType, mc, seed, i, j, &pos))
                continue; // this region is not suitable
            if (pos.x < x0 || pos.x > x1 || pos.z < z0 || pos.z > z1)
                continue; // structure is outside the specified area
            if (!isViableStructurePos(structureType, &g, pos.x, pos.z, 0))
                continue; // biomes are not viable
            if ((structureType == End_City) && !isViableEndCityTerrain(&g, &sn, pos.x, pos.z))
                // end cities have a dedicated terrain checker
                continue;
            else if (mc >= MC_1_18) { // some structures in 1.18+ depend on the terrain
                if (!isViableStructureTerrain(structureType, &g, pos.x, pos.z))
                    continue;
            }

            Piece* plist = (Piece*) calloc(END_CITY_PIECES_MAX, sizeof(Piece));
            {
                Piece* lst = plist;
                for (int i = 0; i < END_CITY_PIECES_MAX; i++) {
                    plist[i].next = &(plist[i + 1]);
                }
            }

            int chunkX = pos.x / 16 - (pos.x < 0);
            int chunkZ = pos.z / 16 - (pos.z < 0);
            int ret = getEndCityPieces(plist, seed, chunkX, chunkZ);

            int hasShip = 0;

            Piece* cpiece = plist;
            for (int i = 0; i < ret; i++) {
                if (strcmp(plist[i].name, "ship") == 0) {
                    hasShip = 1;
                    break;
                }
            }

            std::cout << "x: " << pos.x << ", z: " << pos.z << ", ship: " << hasShip << "\n";
            fprintf(fpt, "%d,%d,%d,0\n", pos.x, pos.z, hasShip);
        }
    }
    fclose(fpt);
}

void findStructuresAround(uint64_t seed, int x, int z, int mc) {
    int r = 5000;

    int rx = x / r - (x < 0);
    int rz = z / r - (z < 0);

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            findStructures(End_City, mc, DIM_END, seed, r * (rx + i), r * (rz + j), r * (rx + i + 1), r * (rz + j + 1));
        }
    }
}