#include "economy.hpp"
#include "assets.hpp"

#include "raylib.h"
#include "raymath.h"

#include <stdlib.h>

static EconomicTile* economy_get_tile(Economy* e, int q, int r) {
    if (q < 0 || q >= e->max_q || r < 0 || r >= e->max_r) {
        TraceLog(LOG_ERROR, "Invalid tile access %d/%d", q, r);
        return nullptr;
    }
    else return &e->tiles[q * e->max_r + r];
}

Economy* economy_create(int board_max_q, int board_max_r) 
{
    Economy *e = (Economy*)calloc(1, sizeof(Economy));
    if (e == nullptr) return e;
    e->max_q = board_max_q;
    e->max_r = board_max_r;
    e->tile_count = board_max_q * board_max_r;
    e->tiles = (EconomicTile*)calloc(board_max_q * board_max_r, sizeof(EconomicTile));
    for (int i = 0; i < e->tile_count; ++i) {
        e->tiles[i].type = ECONOMY_TILE_NONE;
    }
    return e;
}

void economy_destroy(Economy* economy) {
    free(economy->tiles);
    free(economy);
}

static void update_production(Economy* economy, float dt) {
    for (int i = 0; i < economy->tile_count; ++i) {
        bool doWork = true;
        EconomicTile* t = &economy->tiles[i];
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

const char* economy_get_tile_info(Economy* e,int q, int r) {
    static char buffer[1024] = { 0 };
    EconomicTile* t = economy_get_tile(e, q, r);
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

void economy_update(Economy* economy, float dt)
{
    bool doWork = true;
    update_production(economy, dt);
}

void economy_add_tile(Economy* world, int q, int r, int type)
{
    int index = q * world->max_r + r; 
    if (index < 0 || index >= world->tile_count) {
        TraceLog(LOG_WARNING, "Invalid hex coordinates %d/%d", q, r);
        return;
    }
    
    EconomicTile* t = economy_get_tile(world, q, r);
    if (t == nullptr) return;

    t->type = type;

    switch (type) {
    case (ECONOMY_TILE_FARM): {
        t->model_type = MODEL_BUILDING_FARM;
        t->production[GOOD_WHEAT] = 1.0;
        t->supplyMax[GOOD_WHEAT] = 100.0;
    }
    }
}

void economictile_clear(EconomicTile* tile) {
    // Clear all data from a tile, e.g. when it's deleted 
}


