#include "util.cpp"
#include <array>

struct Block {
    int id;
    array<int, 6> faces; // left right up down front back

    Block(int id, const array<int, 6>& faces) : id(id), faces(faces) {}
};

enum Blocks {
    AIR = -1,
    GRASS = 0,
    DIRT = 1,
    STONE = 2,
    IRON = 3,
    COPPER = 4,
    GOLD = 5,
};

Block BlockData[] = {
    Block(GRASS, {2,2,0,1,2,2}),
    Block(DIRT,  {1,1,1,1,1,1}),
    Block(STONE, {3,3,3,3,3,3}),
    Block(IRON,  {4,4,7,7,4,4}),
    Block(COPPER,{5,5,7,7,5,5}),
    Block(GOLD,  {6,6,7,7,6,6}),
};
