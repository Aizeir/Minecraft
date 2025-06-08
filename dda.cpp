#include "olcPixelGameEngine.h"
// DDA Algorithm ==============================================
// https://lodev.org/cgtutor/raycasting.html


// Position/direction initiales
vec2 START = pos;
vec2 DIR = front;
vec2 SLOPE_FOR_DELTA = { sqrt(1 + (DIR.y / DIR.x) * (DIR.y / DIR.x)), sqrt(1 + (DIR.x / DIR.y) * (DIR.x / DIR.y)) };
glm::ivec2 STEP;
        
// Variables
glm::ivec2 tile = floor(START);
vec2 slope;

// Conditions initiales
if (dir.x < 0) {
    STEP.x = -1;
    slope.x = (START.x - float(tile.x)) * SLOPE_FOR_DELTA.x;
}
else {
    STEP.x = 1;
    slope.x = (float(tile.x + 1) - START.x) * SLOPE_FOR_DELTA.x;
}

if (dir.y < 0) {
    STEP.y = -1;
    slope.y = (START.y - float(tile.y)) * SLOPE_FOR_DELTA.y;
}
else {
    STEP.y = 1;
    slope.y = (float(tile.y + 1) - START.y) * SLOPE_FOR_DELTA.y;
}

// Perform "Walk" until collision or range check
float MAX_DISTANCE = 100.0f;
float distance = 0.0f;
bool tile_found = false;

while (!tile_found && distance < MAX_DISTANCE) {
    // Walk along shortest path
    if (slope.x < slope.y) {
        tile.x += STEP.x;
        distance = slope.x;
        slope.x += SLOPE_FOR_DELTA.x;
    }
    else {
        tile.y += STEP.y;
        distance = slope.y;
        slope.y += SLOPE_FOR_DELTA.y;
    }

    // Test tile at new test tile
    if (in_map(world, tile) && is_solid(world, tile)) {
        tile_found = true;
    }
}

// Calculate intersection location
vec2 point;
if (tile_found) {
    point = START + dir * distance;
}