#pragma once

#include "raymath.hpp"

/*
This is responsible for managing and trackign the world state

*/

enum Good {
    GOOD_NONE = -1,
    GOOD_WOOD,
    GOOD_WHEAT,
    GOOD_COUNT
};

enum TileType {
    ECONOMY_TILE_NONE = -1,
    ECONOMY_TILE_GRASS,
    ECONOMY_TILE_FARM,
    ECONOMY_TILE_HOUSE,
    ECONOMY_TILE_FOREST,
    ECONOMY_TILE_COUNT
};

struct Tile {
    int type;
    int model_type;
    int rotation;
    float production[GOOD_COUNT] = { 0 }; // amount produced per sec WHEN demand is fullfilled from storage 
    float demand[GOOD_COUNT] = { 0 };  // amount used to do work per sec
    float supply[GOOD_COUNT] = { 0 }; // Total amount available 
    float supplyMax[GOOD_COUNT] = { 0 };
};

struct Person {
    int model_type;
    // Int activity
    int q;
    int r;
    Vector3 tile_pos;
};

#define PEOPLE_MAX 100

struct World {
    int max_q;
    int max_r;
    int tile_count;
    Tile *tiles;
    int people_count;
    Person people[PEOPLE_MAX] = { 0 };
};

World* world_create(int board_max_q, int board_max_r);
void world_destroy(World* world);
Tile* world_get_tile(World* w, int q, int r);
const char* world_get_tile_info(World* e, int q, int r);
void world_update(World* world, float dt);
void world_add_tile(World* world, Tile tile, int q, int r);
Person* world_get_person(World* w, int q, int r);

void world_add_person(World* world, int type, int q, int r);
