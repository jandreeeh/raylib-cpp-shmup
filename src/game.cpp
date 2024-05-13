#include <raylib.h>
#include "game.h"

typedef struct Player {
    Vector2 position;
    int speed;
    Texture2D sprite;
} Player;

typedef struct Enemy {
    Vector2 position;
    Vector2 targetPos;  
    Texture2D sprite;
} Enemy;


static Player player = { 0 };


void GameInitialize(){
    //player variables
    player.position = (Vector2){ (float)GetScreenWidth() / (int)2, 100};
    player.speed = 10;
    player.sprite = LoadTexture("src/graphics/spaceship.png");


}

void GameUpdate(){
    if (IsKeyDown(KEY_LEFT)){player.position.x -= player.speed;}
    if (IsKeyDown(KEY_RIGHT)){player.position.x += player.speed;}
    if (IsKeyDown(KEY_UP)){player.position.y -= player.speed;}
    if (IsKeyDown(KEY_DOWN)){player.position.y += player.speed;}
}

void GameDraw(){

    ClearBackground(BLACK);
    DrawTexture(player.sprite, player.position.x, player.position.y, WHITE);
}