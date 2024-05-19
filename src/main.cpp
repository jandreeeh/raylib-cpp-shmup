#include <raylib.h>
#include <raymath.h>

#include "game.h"

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

void AppInit();

//----------------------------------
//      Local Variables
//----------------------------------
static float scale;
static RenderTexture2D target;
static Rectangle virtualSource;
static Rectangle virtualDest;
static Vector2 virtualOrigin;



int main()
{

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Shoot Em' Up: A Space Game");
    SetTargetFPS(60);

    AppInit();
    GameInitialize();

    while (!WindowShouldClose())
    {

        GameUpdate();

        //Renders game at 480x270
        BeginTextureMode(target);

        GameDraw();

        EndTextureMode();

        BeginDrawing();

        ClearBackground(BLACK);

        // Draw render texture to screen, properly scaled
        DrawTexturePro(target.texture, virtualSource, virtualDest, virtualOrigin, 0.0f, WHITE);
        DrawFPS(20, 20);
        EndDrawing();

    }

    GameDeInitialize();

    CloseWindow();
    return 0;
}

void AppInit() {
    // Initialization for virtual screen. This allows the program to render the game at 480x270 and display at 1280x720
    scale = MIN((float)GetScreenWidth() / VSCREEN_WIDTH, (float)GetScreenHeight() / VSCREEN_HEIGHT);
    target = LoadRenderTexture(VSCREEN_WIDTH, VSCREEN_WIDTH);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
    virtualSource = { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height };
    virtualDest = { (GetScreenWidth() - ((float)VSCREEN_WIDTH * scale)) * 0.5f, (GetScreenHeight() - ((float)VSCREEN_HEIGHT * scale)) * 0.5f, (float)VSCREEN_WIDTH * scale, (float)VSCREEN_WIDTH * scale };
    virtualOrigin = { 0, 0 };
}