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
#include "raymath.h"
#include "screens.h"
#include <crtdbg.h>
#include <assert.h>

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

Camera3D _camera = { 0 };

struct Game {
    float dt;
};


Game _game;
// For hex functions see https://www.redblobgames.com/grids/hexagons/

struct Hex {
    int r;
    int q;
};
struct Layout {
    Vector2 origin;
    
};

Vector2 flat_hex_to_pixel(int q, int r, float size) 
{
    float x = size * (3. / 2 * q);
    float y = size * (sqrt(3) / 2 * q + sqrt(3) * r);
    return Vector2(x, y);
}


Vector2 pointy_hex_to_pixel(int q, int r, float size)
{
    float x = size * (sqrt(3) * q + sqrt(3) / 2 * r);
    float y = size * (3. / 2 * r);
    return Vector2(x, y);
}

void process_input(Camera3D* camera, float dt)
{
    float change = GetMouseWheelMove();
    camera->fovy += change;
    camera->fovy = Clamp(camera->fovy, 20, 80);
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

    _camera = Camera3D{ .position = Vector3 { 10, 10, 0}, .target = Vector3 {0,0,0}, .up = Vector3{0,1,0},
        .fovy = 30.0f, .projection = CAMERA_PERSPECTIVE };
}

// Gameplay Screen Update logic
void update_gameplay_screen(void)
{
    process_input(&_camera, GetFrameTime());

    // TODO: Update GAMEPLAY screen variables here!

    // Press enter or tap to change to ENDING screen
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    {
        finishScreen = 1;
        PlaySound(fxCoin);
    }
}

// Gameplay Screen Draw logic
void draw_gameplay_screen(void)
{
    static int counts[7] = { 4,5,6,7,6,5,4 };
    static int q_start[7] = { 0, -1, -2, -3, -3, -3, -3 };
    int r = -3;
    int tiles[7][7] = {};
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 7; ++j) {
            tiles[i][j] = GetRandomValue(0, 2);
        }
    }

    // TODO: Draw GAMEPLAY screen here!
    //DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), PURPLE);
    BeginMode3D(_camera);

    for (int row = 0; row < 7; ++row) {
        for (int i = 0; i < counts[row]; ++i)
        {
           int q = q_start[row] + i;
           Vector2 coords = pointy_hex_to_pixel(q , r, 1/sqrtf(3.0));
           DrawModel(g_models[tiles[q+3][r+3]], Vector3(coords.x, 0, coords.y), 1.0, WHITE);
        }
        r += 1;
    }

    DrawCubeWires(Vector3{ 0,0,0 }, 1, 1, 1, BLUE);
    EndMode3D();
}

// Gameplay Screen Unload logic
void unload_gameplay_screen(void)
{
    //unload_models(_models, MODEL_COUNT);
}

// Gameplay Screen should finish?
int finish_gameplay_screen(void)
{
    return finishScreen;
}