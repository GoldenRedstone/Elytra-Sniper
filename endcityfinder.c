#include "finders.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

void findStructures(int structureType, int mc, int dim, uint64_t seed,
    int x0, int z0, int x1, int z1)
{
    printf("\nEnd Cities between x:%d-%d z:%d-%d\n", x0, x1, z0, z1);

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
            // int n = 0;

            // int hasShip = 0;

            printf("x: %d, z: %d\n", pos.x, pos.z);
            fprintf(fpt, "%d,%d,0,0\n", pos.x, pos.z);
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
    int x = 0, z = 0;
    printf("Seed: %" PRIu64 "\n", seed);

    findStructures(End_City, mc, dim, seed, 0, 0, r, r);

    return 0;
}