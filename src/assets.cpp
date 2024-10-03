#include "assets.hpp"

#include "raylib.h"

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif


#include <stdlib.h>

static const char* _building_names[MODEL_BUILDING_COUNT] = {
"building-farm.glb",
"building-market.glb",
"building-house.glb",
};

static const char* resource_dir = "resources/";

static const Vector3 Vector3Zero{ 0,0,0 };

#define MAX_LIGHTS 4

static Shader _lit_shader = { 0 };
static Light _lights[MAX_LIGHTS] = { 0 };

static void assets_init_shaders() {
    if (_lit_shader.id != 0) return;

    _lit_shader = LoadShader(TextFormat("resources/shaders/lighting.vs", GLSL_VERSION),
        TextFormat("resources/shaders/lighting.fs", GLSL_VERSION));
    // Get some required shader locations
    _lit_shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(_lit_shader, "viewPos");
    // NOTE: "matModel" location name is automatically assigned on shader loading, 
    // no need to get the location again if using that uniform name
    //shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");

    // Ambient light level (some basic lighting)
    int ambientLoc = GetShaderLocation(_lit_shader, "ambient");
    float values[4] = {0.2f, 0.2f, 0.2f, 0.2f };
    SetShaderValue(_lit_shader, ambientLoc, values, SHADER_UNIFORM_VEC4);

    // Create lights
    _lights[0] = CreateLight(LIGHT_DIRECTIONAL, Vector3 { 1, 1, 1 }, Vector3Zero, WHITE, _lit_shader);
    //lights[1] = CreateLight(LIGHT_POINT, Vector3 { 2, 1, 2 }, Vector3Zero, RED, _lit_shader);
    //lights[2] = CreateLight(LIGHT_POINT, Vector3 { -2, 1, 2 }, Vector3Zero, GREEN, _lit_shader);
    //lights[3] = CreateLight(LIGHT_POINT, Vector3 { 2, 1, -2 }, Vector3Zero, BLUE, _lit_shader);
}

Model* models_load(const char** names, const int count) {
    assets_init_shaders();
    Model* result = (Model*)calloc(count, sizeof(Model));
    if (result == nullptr) return nullptr;
    char buffer[1024];
    
    for (int i = 0; i < count; ++i) {
        const char* filename = TextFormat("resources/hex/%s", names[i]);
        result[i] = LoadModel(filename);
        for (int mat = 0; mat < result[i].materialCount; ++mat) {
            result[i].materials[mat].shader = _lit_shader;
        }
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


