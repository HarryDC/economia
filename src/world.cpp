#include "world.hpp"
#include "assets.hpp"

#include "raylib.h"
#include "raymath.h"

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
    }
    }
}

void tile_clear(Tile* tile) {
    // Clear all data from a tile, e.g. when it's deleted 
}

