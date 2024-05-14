#include <raylib.h>
#include <raymath.h>

#include "game.h"

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

void AppInit();

int main()
{

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "my personal hell");
    SetTargetFPS(60);

    RenderTexture2D target = LoadRenderTexture(VSCREEN_WIDTH, VSCREEN_WIDTH);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    AppInit();
    GameInitialize();

    while (!WindowShouldClose())
    {
        float scale = MIN((float)GetScreenWidth()/VSCREEN_WIDTH, (float)GetScreenHeight()/VSCREEN_HEIGHT);
        //Update
        GameUpdate();

        BeginTextureMode(target);

        GameDraw();

        EndTextureMode();
        //Draw
        BeginDrawing();
        ClearBackground(BLACK);     // Clear screen background
            // Draw render texture to screen, properly scaled
            DrawTexturePro(target.texture, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
                           (Rectangle){ (GetScreenWidth() - ((float)VSCREEN_WIDTH*scale))*0.5f, (GetScreenHeight() - ((float)VSCREEN_HEIGHT*scale))*0.5f,
                           (float)VSCREEN_WIDTH*scale, (float)VSCREEN_WIDTH*scale }, (Vector2){ 0, 0 }, 0.0f, WHITE);
        DrawFPS(20, 20);
        EndDrawing();

    }

    CloseWindow();
    return 0;
}

void AppInit(){
      // Texture scale filter to use
}