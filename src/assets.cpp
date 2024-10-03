#include "assets.hpp"

#include "raylib.h"

#include <stdlib.h>

static const char* _building_names[MODEL_BUILDING_COUNT] = {
"building-farm.glb",
"building-market.glb",
"building-house.glb",
};

static const char* resource_dir = "resources/";

Model* models_load(const char** names, const int count) {
    Model* result = (Model*)calloc(count, sizeof(Model));
    if (result == nullptr) return nullptr;
    char buffer[1024];
    
    for (int i = 0; i < count; ++i) {
        const char* filename = TextFormat("resources/hex/%s", names[i]);
        result[i] = LoadModel(filename);
    }
    return result;
}

void models_unload(Model* models, const int count) {
    for (int i = 0; i < count; ++i) {
        UnloadModel(models[i]);
    }
}

Model* models_load_buildings()
{
    return models_load(_building_names, MODEL_BUILDING_COUNT);
}


