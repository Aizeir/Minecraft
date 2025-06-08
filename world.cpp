#pragma once
#include "util.cpp"
#include <FastNoiseLite.h>
#include <vector>


class World {public:
    int map[MAPW][MAPD][MAPH];

    World() {
        FastNoiseLite noise;
        noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

        // Gather noise data
        for (uint x=0;x<MAPW;x++) {
        for (uint z=0;z<MAPD;z++) {
        float value = (noise.GetNoise((float)x, (float)z) + 1.0f)/2.0f;
        int ground = (int)(value * MAPD); // entre 0 et MAPD-1

        for (uint y=0;y<MAPH;y++) {
            if      (y >= ground) map[x][z][y] = -1;
            else if (y >= ground-1) map[x][z][y] = 0;
            else if (y >= ground-3) map[x][z][y] = 1;
            else if (y >=  0) map[x][z][y] = 2;
        }}}
    }

    bool is_solid(uint x, uint y, uint z) {
        return 0<=x && x<MAPW && 0<=y && y<MAPH && 0<=z && z<MAPD && (map[x][z][y] != -1);
    }
    bool is_solid(ivec3 block) {
        return is_solid(block.x, block.y, block.z);
    }

    void break_block(glm::ivec3 pos) {
        map[pos.x][pos.z][pos.y] = -1;
    }
};