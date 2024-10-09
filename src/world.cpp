#include "raylib.h"

#include "world.hpp"
#include "assets.hpp"

#include <stdlib.h>

World* world_create(int board_max_q, int board_max_r) 
{
    World *w = (World*)calloc(1, sizeof(World));
    if (w == nullptr) return w;
    w->max_q = board_max_q;
    w->max_r = board_max_r;
    w->tile_count = board_max_q * board_max_r;
    w->tiles = (Tile*)calloc(board_max_q * board_max_r, sizeof(Tile));
    for (int i = 0; i < w->tile_count; ++i) {
        w->tiles[i].type = ECONOMY_TILE_NONE;
    }
    return w;
}

void world_destroy(World* world) {
    free(world->tiles);
    free(world);
}

Tile* world_get_tile(World* w, int q, int r) {
    if (q < 0 || q >= w->max_q || r < 0 || r >= w->max_r) {
        TraceLog(LOG_ERROR, "Invalid tile access %d/%d", q, r);
        return nullptr;
    }
    else return &w->tiles[q * w->max_r + r];
}

// To think about:
// various types of production
// Continous: draw resources from storage and make the product
// With phased things like growing wheat the phases can be done on the
// supply side, only when the growing cycle is done does the good get added
// Work Power ... if more people are on the tile production should be per person

static void world_update_production(World* world, float dt) {
    for (int i = 0; i < world->tile_count; ++i) {
        bool doWork = true;
        Tile* t = &world->tiles[i];
        if (t->type == ECONOMY_TILE_NONE) continue;

        for (int g = 0; g < GOOD_COUNT; ++g) {
            if (t->supply[g] < t->demand[g] * dt) {
                doWork = false;
            }
        }

        if (doWork) {
            // In general a place is not going to use the same resource
            // as it produces
            for (int g = 0; g < GOOD_COUNT; ++g) {
                t->supply[g] += t->production[g] * dt - t->demand[g] * dt;
                t->supply[g] = Clamp(t->supply[g], 0, t->supplyMax[g]);
            }
        }
    }
}

const char* world_get_tile_info(World* e,int q, int r) {
    static char buffer[1024] = { 0 };
    Tile* t = world_get_tile(e, q, r);
    if (t == nullptr) return "INVALID TILE";
    else {
        int end = 0;
        TextAppend(buffer, TextFormat("Tile: %d/%d\n", q, r), &end);
        for (int i = 0; i < GOOD_COUNT; ++i) {
            TextAppend(buffer, TextFormat("%f,", t->supply[i]), &end);
        }
    }
    return buffer;
}

void world_update(World* world, float dt)
{
    bool doWork = true;
    world_update_production(world, dt);
}

void world_add_tile(World* world, Tile tile, int q, int r)
{
    int index = q * world->max_r + r; 
    if (index < 0 || index >= world->tile_count) {
        TraceLog(LOG_WARNING, "Invalid hex coordinates %d/%d", q, r);
        return;
    }
    
    Tile* t = world_get_tile(world, q, r);
    if (t == nullptr) return;

    t->type = tile.type;
    t->rotation = tile.rotation;

    switch (t->type) {
    case (ECONOMY_TILE_FARM): {
        t->model_type = MODEL_BUILDING_FARM;
        t->production[GOOD_WHEAT] = 1.0;
        t->supplyMax[GOOD_WHEAT] = 100.0;
        break;
    }
    case (ECONOMY_TILE_FOREST): {
        t->model_type = MODEL_BUILDING_FOREST;
        t->production[GOOD_WOOD] = 1.0;
        t->supplyMax[GOOD_WOOD] = 100.0;
        break;
    }
    case (ECONOMY_TILE_HOUSE): {
        t->model_type = MODEL_BUILDING_HOUSE;
        // Increase Space for people
        break;
    }
    case (ECONOMY_TILE_GRASS): {
        t->model_type = MODEL_BUILDING_GRASS;
        break;
    }
    }
}

void world_add_person(World* world, int type, int q, int r)
{
    Tile* t = world_get_tile(world, q, r);
    if (t->type == ECONOMY_TILE_NONE) {
        TraceLog(LOG_WARNING, "Trying to add person on empty spot %d/%d", q, r);
        return;
    }
    if (world->people_count <= PEOPLE_MAX) {
        Person* p = &world->people[world->people_count];
        *p = Person{ .model_type = type, .q = q, .r = r, .tile_pos = Vector3{0,.25f,0} };
        ++world->people_count;
    }
    else {
        TraceLog(LOG_WARNING, "Exceeded maximum number of people (%d)", PEOPLE_MAX);
    }
    
}

void tile_clear(Tile* tile) {
    // Clear all data from a tile, e.g. when it's deleted 
}


