#pragma once

/* 
Assets, this is responsible for managing the assets of the games, these are all models, textures, fonts etc 
 
*/

struct Model;

enum ModelType {
    MODEL_BUILDING_NONE = -1,
    MODEL_BUILDING_FARM,
    MODEL_BUILDING_HOUSE,
    MODEL_BUILDING_FOREST,
    MODEL_BUILDING_COUNT
};

Model* models_load_buildings();

Model* models_load(const char** names, const int count);

void models_unload(Model* models, const int count);



