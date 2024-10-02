#pragma once

#include <raylib.h>

enum ModelType {
    MODEL_BUILDING_NONE = -1,
    MODEL_BUILDING_FARM,
    MODEL_BUILDING_HOUSE,
    MODEL_BUILDING_MARKET,
    MODEL_BUILDING_COUNT
};

Model* models_load_buildings();

Model* models_load(const char** names, const int count);

void models_unload(Model* models, const int count);



