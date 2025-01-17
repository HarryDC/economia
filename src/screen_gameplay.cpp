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
#include "raygui.h"

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

// Duplicated
static int _editor_tiles[ECONOMY_TILE_COUNT * 2] = {
    ECONOMY_TILE_GRASS, MODEL_BUILDING_GRASS,
    ECONOMY_TILE_FARM, MODEL_BUILDING_FARM,
    ECONOMY_TILE_HOUSE, MODEL_BUILDING_HOUSE,
    ECONOMY_TILE_FOREST, MODEL_BUILDING_FOREST,
};


enum Actions {
    ACTION_CURSOR_LEFT = KEY_LEFT,
    ACTION_CURSOR_RIGHT = KEY_RIGHT,
    ACTION_CURSOR_UP = KEY_UP,
    ACTION_CURSOR_DOWN = KEY_DOWN,
    ACTION_ROTATE_LEFT = KEY_I,
    ACTION_NEXT_TILE = KEY_O,
    ACTION_ROTATE_RIGHT = KEY_P,
    ACTION_CAMERA_LEFT = KEY_A,
    ACTION_CAMERA_RIGHT = KEY_D,
    ACTION_CAMERA_UP = KEY_W,
    ACTION_CAMERA_DOWN = KEY_S,
    ACTION_CAMERA_ORBIT_CW = KEY_E,
    ACTION_CAMERA_OBRIT_CCW = KEY_Q,
    ACTION_PLACE_TILE = KEY_SPACE,
    ACTION_PERSON_PLACE = KEY_Y,
    ACTION_PERSON_SELECT = KEY_H,
    ACTION_PERSON_MOVE = KEY_M,
};

Camera3D _camera3D = { 0 };
Camera2D _camera2D = { 0 };

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
    World* world = nullptr;
    Person* selected_person = nullptr;
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
    camera->fovy = Clamp(camera->fovy, 5, 40);

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
    case ACTION_NEXT_TILE:
        cursor.tile.type = (cursor.tile.type + 1) % ECONOMY_TILE_COUNT;
        break;
    case ACTION_PLACE_TILE:
        _tiles[cursor.hex.q][cursor.hex.r] = cursor.tile;
        world_add_tile(_game.world, cursor.tile, cursor.hex.q, cursor.hex.r);
        break;
    case ACTION_PERSON_PLACE:
    {
        TraceLog(LOG_INFO, "Person dropped");
        Tile* t = world_get_tile(_game.world, cursor.hex.q, cursor.hex.r);
        if (t->type == ECONOMY_TILE_GRASS) {
            world_add_person(_game.world, MODEL_CHARACTER_FEMALE, cursor.hex.q, cursor.hex.r);
        }
        break;
    }
    case ACTION_PERSON_SELECT:
    {
        TraceLog(LOG_INFO, "Person Selected");
        Person* p = world_get_person(_game.world, cursor.hex.q, cursor.hex.r);
        if (p != nullptr) {
            _game.selected_person = p;
        }
        break;
    }
    case ACTION_PERSON_MOVE:
    {
        // TODO Check if there is room 
        // TODO Path planning to actually animate moving 
        if (_game.selected_person != nullptr) {
            _game.selected_person->q = cursor.hex.q;
            _game.selected_person->r = cursor.hex.r;
        }
        break;
    }
    }

    if (IsKeyDown(ACTION_CAMERA_LEFT))
    {
        Vector3 mov = Vector3(10, 0, 0) * GetFrameTime();
        _camera3D.position += mov;
        _camera3D.target += mov;
    }
    if (IsKeyDown(ACTION_CAMERA_RIGHT)) 
    {
        Vector3 mov = Vector3(-10, 0, 0) * GetFrameTime();
        _camera3D.position += mov;
        _camera3D.target += mov;
    }

    if (_game.cursor.hex.q != cursor.hex.q || _game.cursor.hex.r != cursor.hex.r) {
        Vector3 coords = pointy_hex_to_pixel(cursor.hex.q, cursor.hex.r, 1.0f);
        TraceLog(LOG_DEBUG, "Cursor: %d/%d %f|%f|%f", cursor.hex.q, cursor.hex.r, coords.x, coords.y, coords.z);
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

    _camera3D = Camera3D{ .position = Vector3 { 0, 10, 10}, .target = Vector3 {0,0,0}, .up = Vector3{0,1,0},
        .fovy = 15.0f, .projection = CAMERA_PERSPECTIVE };

    _camera2D = { 0 };
    _camera2D.zoom = 1.0;

    _game.cursor.hex = Hex{ 3,3 };
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
    process_input(&_camera3D, GetFrameTime());
    world_update(_game.world, GetFrameTime());

    // Update Model animations
    //int anim = 2;

    //frame_count = (frame_count + 1) % animations[anim].frameCount;

    //if (_tiles[_game.cursor.hex.q][_game.cursor.hex.r].type != -1) {
    //    TraceLog(LOG_INFO, world_get_tile_info(_game.world,  
    //        _game.cursor.hex.q, _game.cursor.hex.r));
    //}

    // TODO: Update GAMEPLAY screen variables here!

    // Press enter or tap to change to ENDING screen
    if (IsKeyPressed(KEY_ENTER))
    {
        finishScreen = 1;
        PlaySound(fxCoin);
    }

}

// Draw the information panel for a tile
void draw_hud_tile_info(Vector3 pos, int q, int r) {
    char buffer[1024] = { 0 };
    Tile* t = world_get_tile(_game.world, q, r);
    Vector2 pos2DTest = GetWorldToScreen(Vector3{ 0,0,0 }, _camera3D);
    Vector2 pos2D = GetWorldToScreen(pos, _camera3D);
    const float width = 200;
    const float height = 60;
    Rectangle rect = { .x = pos2D.x - width / 2, .y = pos2D.y - height * 1.3f,
                       .width = width, .height = height };

    GuiPanel(rect,"Information");
    int end = 0;
    TextAppend(buffer, "Goods:\n", &end);
    TextAppend(buffer, TextFormat("%d", (int)t->supply[0]), &end);
    for (int i = 1; i < GOOD_COUNT; ++i) {
        TextAppend(buffer, TextFormat("/%d", (int)t->supply[i]), &end);
    }
    rect.y += 15; // Panel bar
    GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_TOP);   // WARNING: Word-wrap does not work as expected in case of no-top alignment
    GuiSetStyle(TEXTBOX, TEXT_WRAP_MODE, TEXT_WRAP_WORD);            // WARNING: If wrap mode enabled, text editing is not supported
    GuiSetStyle(TEXTBOX, BORDER_WIDTH, 0);
    GuiTextBox(rect, buffer, 12, false);
}

void draw_tile(int type, int rotation, int q, int r, Color color)
{
    if (type == -1) return;
    const Vector3 pos = pointy_hex_to_pixel(q, r, _size) + _origin;
    DrawModelEx(g_models[_editor_tiles[type*2 + 1]], 
        pos, Vector3{ 0,1,0 }, 60.0f * rotation, Vector3{ 1, 1, 1 }, WHITE);
}

void gameplay_screen_draw_hud() {
    BeginMode2D(_camera2D);

    Vector3 gui_pos{ 20,20 };
    const float width = 10.0f;
    const float height = 10.0f;

    GuiPanel(Rectangle{ .x = gui_pos.x, .y = gui_pos.y, .width = 200, .height = 200 }, "Settings");

    static bool show_info = false;
    GuiCheckBox(Rectangle{ .x = 30, .y = 50, .width = width, .height = height }, "Show Info", &show_info);
    
    const Vector3 pos = pointy_hex_to_pixel(_game.cursor.hex.q, _game.cursor.hex.r, _size) + _origin;

    if (show_info && _tiles[_game.cursor.hex.q][_game.cursor.hex.r].type != -1) {
        draw_hud_tile_info(pos, _game.cursor.hex.q, _game.cursor.hex.r);
    }

    EndMode2D();
}

// Gameplay Screen Draw logic
void draw_gameplay_screen(void)
{
    static int counts[7] = { 4,5,6,7,6,5,4 };
    static int q_start[7] = { 0, -1, -2, -3, -3, -3, -3 };
    int r = -3;

    // TODO: Draw GAMEPLAY screen here!
    //DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), PURPLE);
    BeginMode3D(_camera3D);
    draw_coords(Vector3{ 0,0,0 });
    for (int q = 0; q < _board_size; ++q) {
        for (int r = 0; r < _board_size; ++r)
        {
            Tile* t = &_game.world->tiles[q * _game.world->max_r + r];
            draw_tile(t->type, t->rotation, q, r, WHITE);
        }
    }

    for (int i = 0; i < _game.world->people_count; ++i) {
        Person* p = &_game.world->people[i];
        Vector3 pos = pointy_hex_to_pixel(p->q, p->r, _size);
        DrawModel(g_models[p->model_type], _origin + pos + p->tile_pos , .3, WHITE);
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
    // Draw the HUB on top of the game screen
    gameplay_screen_draw_hud();
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