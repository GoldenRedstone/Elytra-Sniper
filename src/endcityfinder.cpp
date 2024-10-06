#include "finders.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <unistd.h>

// Define a helper macro to generate file paths
#define PROJECT_DIR(x) (std::string(PROJECT_SOURCE_DIR) + x)

// Function to find structures within a specific coordinate range
void findStructures(int structureType, int mcVersion, int dimension, uint64_t seed, 
                    int x0, int z0, int x1, int z1) {
    
    // Construct the output filename
    std::ostringstream filename;
    filename << "searched/" << seed << "." << x0 << "." << z0 << ".csv";
    std::string fullPath = PROJECT_DIR(filename.str());
    
    // Check if the file already exists
    if (access(fullPath.c_str(), F_OK) == 0) {
        std::cout << fullPath << " - File already exists\n";
        return;
    } else {
        std::cout << fullPath << " - File does not exist\n";
        std::cout << "Searching for cities between x: " << x0 << " and " << x1 
                  << ", z: " << z0 << " and " << z1 << "\n";
    }

    // Open file for writing
    FILE* file = fopen(fullPath.c_str(), "w+");
    if (!file) {
        std::cerr << "Error opening file: " << fullPath << "\n";
        return;
    }
    fprintf(file, "x,z,ship,looted\n");

    // Setup biome generator and seed
    Generator generator;
    setupGenerator(&generator, mcVersion, 0);
    applySeed(&generator, dimension, seed);

    // Special handling for End City structure type
    SurfaceNoise surfaceNoise;
    if (structureType == End_City) {
        initSurfaceNoise(&surfaceNoise, dimension, seed);
    }

    // Get structure configuration for the given version and structure type
    StructureConfig structureConfig;
    if (!getStructureConfig(structureType, mcVersion, &structureConfig)) {
        std::cerr << "Invalid version or structure type\n";
        fclose(file);
        return;
    }

    // Define region boundaries based on structure region size
    double blocksPerRegion = structureConfig.regionSize * 16.0;
    int rx0 = static_cast<int>(std::floor(x0 / blocksPerRegion));
    int rz0 = static_cast<int>(std::floor(z0 / blocksPerRegion));
    int rx1 = static_cast<int>(std::ceil(x1 / blocksPerRegion));
    int rz1 = static_cast<int>(std::ceil(z1 / blocksPerRegion));

    // Iterate over structure regions
    for (int j = rz0; j <= rz1; j++) {
        for (int i = rx0; i <= rx1; i++) {

            // Get the structure position
            Pos pos;
            if (!getStructurePos(structureType, mcVersion, seed, i, j, &pos)) {
                continue;
            }
            // Check if the structure is within the specified area
            if (pos.x < x0 || pos.x > x1 || pos.z < z0 || pos.z > z1) {
                continue;
            }
            // Verify biome viability for the structure
            if (!isViableStructurePos(structureType, &generator, pos.x, pos.z, 0)) {
                continue;
            }
            // Additional terrain checks for End City and 1.18+ versions
            if (structureType == End_City && !isViableEndCityTerrain(&generator, &surfaceNoise, pos.x, pos.z)) {
                continue;
            }
            if (mcVersion >= MC_1_18 && !isViableStructureTerrain(structureType, &generator, pos.x, pos.z)) {
                continue;
            }

            // Get End City pieces (specific to End City structures)
            Piece* pieceList = (Piece*) calloc(END_CITY_PIECES_MAX, sizeof(Piece));
            if (!pieceList) {
                std::cerr << "Memory allocation failed\n";
                fclose(file);
                return;
            }

            // Initialize linked list for pieces
            for (int k = 0; k < END_CITY_PIECES_MAX - 1; k++) {
                pieceList[k].next = &pieceList[k + 1];
            }

            int chunkX = pos.x / 16 - (pos.x < 0);
            int chunkZ = pos.z / 16 - (pos.z < 0);
            int ret = getEndCityPieces(pieceList, seed, chunkX, chunkZ);

            // Check if a ship is present in the structure
            int hasShip = 0;
            for (int k = 0; k < ret; k++) {
                if (strcmp(pieceList[k].name, "ship") == 0) {
                    hasShip = 1;
                    break;
                }
            }

            // Output structure info
            std::cout << "x: " << pos.x << ", z: " << pos.z << ", ship: " << hasShip << "\n";
            fprintf(file, "%d,%d,%d,0\n", pos.x, pos.z, hasShip);

            // Free allocated memory
            free(pieceList);
        }
    }

    // Close file after writing
    fclose(file);
}

// Function to find structures around a given seed and coordinates
void findStructuresAround(uint64_t seed, int x, int z, int mcVersion) {
    const int radius = 5000;
    int rx = x / radius - (x < 0);
    int rz = z / radius - (z < 0);

    // Check regions around the coordinates
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            findStructures(End_City, mcVersion, DIM_END, seed, 
                           radius * (rx + i), radius * (rz + j), 
                           radius * (rx + i + 1), radius * (rz + j + 1));
        }
    }
}
