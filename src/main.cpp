#include <raylib.h>
#include <raymath.h>

#include "game.h"

// void AppInitialize();
// void UpdateDraw();
// void AppDeInitialize();

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 600;



    InitWindow(screenWidth, screenHeight, "my personal hell");
    SetTargetFPS(60);

    GameInitialize();

    while (!WindowShouldClose())
    {
        //Update
        GameUpdate();

        //Draw
        BeginDrawing();
        
        GameDraw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

// void GameInitialize(){
//     InitAudioDevice();
// }

// void GameUpdate(){

// }
// void GameDraw(){

// }

// void GameShutdown(){
//     CloseAudioDevice();
// }