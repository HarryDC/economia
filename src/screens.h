/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Screens Functions Declarations (Init, Update, Draw, Unload)
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

#pragma once

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum GameScreen { UNKNOWN = -1, LOGO = 0, TITLE, OPTIONS, GAMEPLAY, ENDING } GameScreen;

struct GameModels {
    Model* tiles;
    Model* roads;
    Model* rivers;
};

//----------------------------------------------------------------------------------
// Global Variables Declaration (shared by several modules)
//----------------------------------------------------------------------------------
extern GameScreen currentScreen;
extern Font font;
extern Music music;
extern Sound fxCoin;

extern Model *g_models;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Logo Screen Functions Declaration
//----------------------------------------------------------------------------------
void init_logo_screen(void);
void update_logo_screen(void);
void draw_logo_screen(void);
void unload_logo_screen(void);
int finish_logo_screen(void);

//----------------------------------------------------------------------------------
// Title Screen Functions Declaration
//----------------------------------------------------------------------------------
void init_title_screen(void);
void update_title_screen(void);
void draw_title_screen(void);
void unload_title_screen(void);
int finish_title_screen(void);

//----------------------------------------------------------------------------------
// Options Screen Functions Declaration
//----------------------------------------------------------------------------------
void init_options_screen(void);
void update_options_screen(void);
void draw_options_screen(void);
void unload_options_screen(void);
int finish_options_screen(void);

void process_input(Camera3D* camera, float dt);

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Declaration
//----------------------------------------------------------------------------------
void init_gameplay_screen(void);
void update_gameplay_screen(void);
void draw_gameplay_screen(void);
void unload_gameplay_screen(void);
int finish_gameplay_screen(void);

//----------------------------------------------------------------------------------
// Ending Screen Functions Declaration
//----------------------------------------------------------------------------------
void init_ending_screen(void);
void update_ending_screen(void);
void draw_ending_screen(void);
void unload_ending_screen(void);
int finish_ending_screen(void);

#ifdef __cplusplus
}
#endif