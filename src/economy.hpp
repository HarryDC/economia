

enum Good {
    GOOD_NONE = -1,
    GOOD_WOOD,
    GOOD_WHEAT,
    GOOD_COUNT
};

enum EconomicTileType {
    ECONOMY_TILE_NONE = -1,
    ECONOMY_TILE_FARM,
    ECONOMY_TILE_COUNT
};

struct EconomicTile {
    int type;
    int model_type;
    float production[GOOD_COUNT] = { 0 }; // amount produced per sec WHEN demand is fullfilled from storage 
    float demand[GOOD_COUNT] = { 0 };  // amount used to do work per sec
    float supply[GOOD_COUNT] = { 0 }; // Total amount available 
    float supplyMax[GOOD_COUNT] = { 0 };
};

struct Economy {
    int max_q;
    int max_r;
    int tile_count;
    EconomicTile *tiles;
};

Economy* economy_create(int board_max_q, int board_max_r);
void economy_destroy(Economy* economy);
const char* economy_get_tile_info(Economy* e, int q, int r);
void economy_update(Economy* world, float dt);
void economy_add_tile(Economy* world, int q, int r, int type);