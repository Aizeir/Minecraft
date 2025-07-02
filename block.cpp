#pragma once
#include "util.cpp"
#include <array>


enum Items {
    AIR = -1,
    GRASS = 0,
    DIRT = 1,
    STONE = 2,
    IRON = 3,
    COPPER = 4,
    GOLD = 5,
    SAND = 6,
    WOOD = 7,
    WOOD_STUMP = 8,
    WOOD_TOP = 9,
    LEAVES = 10,
    PLANKS = 11,
    BEDROCK = 12,
    WATER = 13,
};


struct Block {
    int id;
    string name;
    array<int, 6> faces; // left right up down front back

    Block(int id, string name, const array<int, 6>& faces) : id(id), name(name), faces(faces) {}
};

Block BlockData[] = {
    Block(GRASS, "grass", {2,2,0,1,2,2}),
    Block(DIRT, "dirt",  {1,1,1,1,1,1}),
    Block(STONE, "stone", {3,3,3,3,3,3}),
    Block(IRON, "iron",  {4,4,7,7,4,4}),
    Block(COPPER, "copper",{5,5,7,7,5,5}),
    Block(GOLD, "gold",  {6,6,7,7,6,6}),
    Block(SAND, "sand",  {8,8,8,8,8,8}),
    Block(WOOD, "wood",  {9,9,11,11,9,9}),
    Block(WOOD_STUMP, "wood_stump",  {10,10,11,16,10,10}),
    Block(WOOD_TOP, "wood_top",  {15,15,16,11,15,15}),
    Block(LEAVES, "leaves",  {12,12,12,12,12,12}),
    Block(PLANKS, "planks",  {13,13,13,13,13,13}),
    Block(BEDROCK, "bedrock",  {14,14,14,14,14,14}),
    Block(WATER, "water",  {17,17,17,17,17,17}),
};

struct Item {
    string name;
    int block = -1;

    Item(string name, int block=-1) : name(name), block(block) {}
};

Item ItemData[] = {
    Item("grass", GRASS),
    Item("dirt", DIRT),
    Item("stone", STONE),
    Item("iron", IRON),
    Item("copper", COPPER),
    Item("gold", GOLD),
    Item("sand", SAND),
    Item("wood", WOOD),
    Item("wood_stump", WOOD_STUMP),
    Item("wood_top", WOOD_TOP),
    Item("leaves", LEAVES),
    Item("planks", PLANKS),
    Item("bedrock", BEDROCK),
    Item("water", WATER),
};


const std::array<std::array<int, 4>, 11> tree_structure = {{
    {{0,0,0,WOOD_STUMP}},
    {{0,1,0,WOOD}},
    {{0,2,0,WOOD}},
    {{0,3,0,WOOD_TOP}}, // trunk

    // leaves layer 1 (top)
    {{0,4,0,LEAVES}},
    {{1,3,0,LEAVES}},
    {{-1,3,0,LEAVES}},
    {{0,3,1,LEAVES}},
    {{0,3,-1,LEAVES}},

    // leaves layer 2 (corners)
    {{1,4,0,LEAVES}},
    {{-1,4,0,LEAVES}}
}};

static unordered_set<int> non_solid = {
    AIR, WATER
};
