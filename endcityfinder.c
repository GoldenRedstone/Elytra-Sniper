#include "finders.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

void findStructures(int structureType, int mc, int dim, uint64_t seed,
    int x0, int z0, int x1, int z1)
{
    printf("\nEnd Cities between x:%d-%d z:%d-%d\n", x0, x1, z0, z1);

    char fname[80];
    sprintf(fname, "searched\\%" PRIu64 ".%d.%d.csv", seed, x0, z0);
    printf("%s\n", fname);

    if (access(fname, F_OK) == 0) {
        printf("File already exists\n");
        return;
    } else {
        printf("File does not exist\n");
        printf("Searching for cities\n");
    }

    FILE *fpt;
    fpt = fopen(fname, "w+");
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

    for (j = rz0; j <= rz1; j++)
    {
        for (i = rx0; i <= rx1; i++)
        {   // check the structure generation attempt in region (i, j)
            Pos pos;
            if (!getStructurePos(structureType, mc, seed, i, j, &pos))
                continue; // this region is not suitable
            if (pos.x < x0 || pos.x > x1 || pos.z < z0 || pos.z > z1)
                continue; // structure is outside the specified area
            if (!isViableStructurePos(structureType, &g, pos.x, pos.z, 0))
                continue; // biomes are not viable
            if (!isViableEndCityTerrain(&g, &sn, pos.x, pos.z))
                // end cities have a dedicated terrain checker
                continue;
            else if (mc >= MC_1_18)
            {   // some structures in 1.18+ depend on the terrain
                if (!isViableStructureTerrain(structureType, &g, pos.x, pos.z))
                    continue;
            }

            
            Piece* plist = (Piece*)calloc(200, sizeof(Piece));
            {
                Piece* lst = plist;
                for (int i = 0; i < 200; i++){
                    plist[i].next = &(plist[i+1]);
                }
            }

            int ret = getEndCityPieces(plist, seed, pos.x/16, pos.z/16);

            int hasShip = 0;

            Piece* cpiece = plist;
            for (int i = 0; i < ret; i++) {
                if (strcmp(plist[i].name, "ship") == 0) {
                    hasShip = 1;
                    break;
                }
            }

            printf("x: %d, z: %d, ship: %d\n", pos.x, pos.z, hasShip);
            fprintf(fpt, "%d,%d,%d,0\n", pos.x, pos.z, hasShip);
        }
    }
    fclose(fpt);
}

int main()
{   
    uint64_t seed = 2438515238773172647;
    int mc = MC_1_20;
    int dim = DIM_END;
    int r = 5000;
    int start_x = 0, start_z = 0;
    int x = (start_x / r) * r;
    int z = (start_z / r) * r;
    printf("Seed: %" PRIu64 "\n", seed);

    // Centre
    findStructures(End_City, mc, dim, seed, x, z, x+r, z+r);
    // N
    findStructures(End_City, mc, dim, seed, x, z-r, x+r, z);
    // NE
    findStructures(End_City, mc, dim, seed, x+r, z-r, x+2*r, z);
    // E
    findStructures(End_City, mc, dim, seed, x+r, z, x+2*r, z+r);
    // SE
    findStructures(End_City, mc, dim, seed, x+r, z+r, x+2*r, z+2*r);
    // S
    findStructures(End_City, mc, dim, seed, x, z+r, x+r, z+2*r);
    // SW
    findStructures(End_City, mc, dim, seed, x-r, z+r, x, z+2*r);
    // W
    findStructures(End_City, mc, dim, seed, x-r, z, x, z+r);
    // NW
    findStructures(End_City, mc, dim, seed, x-r, z-r, x, z);

    return 0;
}