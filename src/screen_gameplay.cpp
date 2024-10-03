/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"
#include "raymath.hpp"
#include "screens.h"
#include "assets.hpp"
#include "world.hpp"

#include <crtdbg.h>
#include <assert.h>

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

static int _editor_tiles[ECONOMY_TILE_COUNT * 2] = {
    ECONOMY_TILE_FARM, MODEL_BUILDING_FARM
};


enum Actions {
    ACTION_CURSOR_LEFT = KEY_LEFT,
    ACTION_CURSOR_RIGHT = KEY_RIGHT,
    ACTION_CURSOR_UP = KEY_UP,
    ACTION_CURSOR_DOWN = KEY_DOWN,
    ACTION_ROTATE_LEFT = KEY_E,
    ACTION_ROTATE_RIGHT = KEY_Q,
    ACTION_PLACE = KEY_SPACE,
};

Camera3D _camera = { 0 };

struct Hex {
    int r;
    int q;
};

struct Cursor {
    Hex hex;
    Tile tile;
};


struct Game {
    float dt;
    Cursor cursor;
    World* world;
};

static constexpr int _board_size = 7;
static Tile _tiles[_board_size][_board_size] = {0};

static const float _size = 1.0f / sqrtf(3.0);
static Vector3 _origin;


Game _game;
// For hex functions see https://www.redblobgames.com/grids/hexagons/


struct Layout {
    Vector2 _origin;
    
};

Vector2 flat_hex_to_pixel(int q, int r, float _size) 
{
    float x = _size * (3. / 2 * q);
    float y = _size * (sqrt(3) / 2 * q + sqrt(3) * r);
    return Vector2(x, y);
}


constexpr Vector3 pointy_hex_to_pixel(int q, int r, float _size)
{
    constexpr float sqrt3 = 1.73205080757;
    float x = _size * (sqrt3 * q + sqrt3 / 2 * r);
    float y = _size * (3. / 2 * r);
    return Vector3(x, 0, y);
}

void draw_coords(Vector3 _origin, float _size = 1) {
    DrawLine3D(_origin, _origin + Vector3(_size, 0, 0), RED);
    DrawLine3D(_origin, _origin + Vector3(0, _size, 0), GREEN);
    DrawLine3D(_origin, _origin + Vector3(0, 0, _size), BLUE);
}

void process_input(Camera3D* camera, float dt)
{
    float change = GetMouseWheelMove();
    camera->fovy += change;
    camera->fovy = Clamp(camera->fovy, 20, 80);

    Cursor cursor = _game.cursor;

    int key = GetKeyPressed();
    switch (key) {
    case ACTION_CURSOR_LEFT:
        cursor.hex.q = (cursor.hex.q - 1 + _board_size) % _board_size;
        break;
    case ACTION_CURSOR_RIGHT:
        cursor.hex.q = (cursor.hex.q + 1) % _board_size;
        break;
    case ACTION_CURSOR_UP:
        cursor.hex.r = (cursor.hex.r - 1 + _board_size) % _board_size;
        break;
    case ACTION_CURSOR_DOWN:
        cursor.hex.r = (cursor.hex.r + 1) % _board_size;
        break;
    case ACTION_ROTATE_LEFT:
        cursor.tile.rotation = (cursor.tile.rotation + 1) % 6;
        break;
    case ACTION_ROTATE_RIGHT:
        cursor.tile.rotation = (cursor.tile.rotation - 1 + 6) % 6;
        break;
    case ACTION_PLACE:
        _tiles[cursor.hex.q][cursor.hex.r] = cursor.tile;
        world_add_tile(_game.world, cursor.tile, cursor.hex.q, cursor.hex.r);
    }
    if (_game.cursor.hex.q != cursor.hex.q || _game.cursor.hex.r != cursor.hex.r) {
        Vector3 coords = pointy_hex_to_pixel(cursor.hex.q, cursor.hex.r, 1.0f);
        TraceLog(LOG_INFO, "Cursor: %d/%d %f|%f|%f", cursor.hex.q, cursor.hex.r, coords.x, coords.y, coords.z);
        
    }
    
    _game.cursor = cursor;
}

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void init_gameplay_screen(void)
{

    // TODO: Initialize GAMEPLAY screen variables here!
    framesCounter = 0;
    finishScreen = 0;

    _camera = Camera3D{ .position = Vector3 { 0, 10, 10}, .target = Vector3 {0,0,0}, .up = Vector3{0,1,0},
        .fovy = 30.0f, .projection = CAMERA_PERSPECTIVE };

    _game.cursor.hex = Hex{ 0,0 };
    _game.cursor.tile.type = ECONOMY_TILE_FARM;
    _game.cursor.tile.rotation = 0;

    for (int i = 0; i < _board_size; ++i) {
        for (int j = 0; j < _board_size; ++j) {
            _tiles[i][j].type = -1;
            _tiles[i][j].rotation = 0;
        }
    }

    _game.world = world_create(_board_size, _board_size);
    _origin = pointy_hex_to_pixel(-3, -3, _size);
}

// Gameplay Screen Update logic
void update_gameplay_screen(void)
{
    process_input(&_camera, GetFrameTime());
    world_update(_game.world, GetFrameTime());

    if (_tiles[_game.cursor.hex.q][_game.cursor.hex.r].type != -1) {
        TraceLog(LOG_INFO, world_get_tile_info(_game.world,  
            _game.cursor.hex.q, _game.cursor.hex.r));
    }

    // TODO: Update GAMEPLAY screen variables here!

    // Press enter or tap to change to ENDING screen
    if (IsKeyPressed(KEY_ENTER))
    {
        finishScreen = 1;
        PlaySound(fxCoin);
    }

}

void draw_tile(int type, int rotation, int q, int r, Color color)
{
    if (type == -1) return;
    const Vector3 pos = pointy_hex_to_pixel(q, r, _size) + _origin;
    DrawModelEx(g_buildings[_editor_tiles[type*2 + 1]], 
        pos, Vector3{ 0,1,0 }, 60.0f * rotation, Vector3{ 1, 1, 1 }, WHITE);
}

// Gameplay Screen Draw logic
void draw_gameplay_screen(void)
{
    static int counts[7] = { 4,5,6,7,6,5,4 };
    static int q_start[7] = { 0, -1, -2, -3, -3, -3, -3 };
    int r = -3;


    // TODO: Draw GAMEPLAY screen here!
    //DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), PURPLE);
    BeginMode3D(_camera);
    draw_coords(Vector3{ 0,0,0 });
    for (int q = 0; q < _board_size; ++q) {
        for (int r = 0; r < _board_size; ++r)
        {
            Tile* t = &_game.world->tiles[q * _game.world->max_r + r];
            draw_tile(t->type, t->rotation, q, r, WHITE);
        }
    }

    const Vector3 pos  = pointy_hex_to_pixel(_game.cursor.hex.q, _game.cursor.hex.r, _size) + _origin;

    if (_tiles[_game.cursor.hex.q][_game.cursor.hex.r].type == -1) {
        draw_tile(_game.cursor.tile.type, _game.cursor.tile.rotation, 
            _game.cursor.hex.q, _game.cursor.hex.r, Color(255, 255, 255, 128));
    }
    else {
        DrawCubeWires(pos, 1, 1, 1, BLUE);
    }
    EndMode3D();
}

// Gameplay Screen Unload logic
void unload_gameplay_screen(void)
{
    world_destroy(_game.world);
}

// Gameplay Screen should finish?
int finish_gameplay_screen(void)
{
    return finishScreen;
}