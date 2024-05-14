#include <raylib.h>
#include <raymath.h>
#include <iostream>

#include "game.h"

typedef struct Player {
    Vector2 position;
    int speed;
    Texture2D sprite;
} Player;

//Go to point and home shoots
typedef struct Peashooter {
    Vector2 position;
    Vector2 targetPos;  
    Rectangle rec;
    int health;
    bool active;
} Peashooter;

//Move while shooting
typedef struct Runner {
    Vector2 spawnPoint;
    Vector2 pointA;
    Vector2 pointB;  
    Texture2D sprite;
    int health;
    bool active;
} Runner;

//Takes three hits
typedef struct Tank {
    Vector2 spawnPoint;
    Vector2 targetPos;  
    Texture2D sprite;
    int health;
    bool active;
} Tank;

static void Spawner();

static int activeEnemies = 10;
static Player player = { 0 };
static Peashooter peashooter[MAX_NUM_PEASHOOTER] = {0};
static double currentTime = 0;
static int enemyIndex = 0;
static int timeBetweenSpawn = 2;

void GameInitialize(){
    //player variables
    player.position = (Vector2){ 100, VSCREEN_HEIGHT/2};
    player.speed = 250;
    player.sprite = LoadTexture("src/graphics/spaceship.png");

     //initialize peashooter
    for(int i = 0; i < MAX_NUM_PEASHOOTER; i++){
        peashooter[i].position.x = GetRandomValue(VSCREEN_WIDTH, VSCREEN_WIDTH + 100);
        peashooter[i].position.y = GetRandomValue(0, VSCREEN_HEIGHT);
        peashooter[i].targetPos.x = -50;
        peashooter[i].targetPos.y = peashooter[i].position.y;
        peashooter[i].rec.height = 10;
        peashooter[i].rec.width = 10;
        peashooter[i].rec.x = peashooter[i].position.x;
        peashooter[i].rec.y = peashooter[i].position.y;
        peashooter[i].health = 1;
        peashooter[i].active = false;
    }
}

void GameUpdate(){
    // Enemy spawner timer
    currentTime += GetFrameTime();
    Spawner();
    std::cout << currentTime << std::endl;

    //Player Input
    if (IsKeyDown(KEY_LEFT)){player.position.x -= player.speed * GetFrameTime();}
    if (IsKeyDown(KEY_RIGHT)){player.position.x += player.speed * GetFrameTime();}
    if (IsKeyDown(KEY_UP)){player.position.y -= player.speed * GetFrameTime();}
    if (IsKeyDown(KEY_DOWN)){player.position.y += player.speed * GetFrameTime();}


    //Enemy Movement
    for (int i = 0; i < activeEnemies; i++){
        if (peashooter[i].active){
            //std::cout << "Im moving master!\n";
            peashooter[i].position = Vector2MoveTowards(peashooter[i].position, peashooter[i].targetPos, 500.0 * GetFrameTime());
        }
    }
}

void GameDraw(){

    ClearBackground(BLACK);
    DrawTexture(player.sprite, player.position.x, player.position.y, WHITE);
     for (int i = 0; i < activeEnemies; i++)
            {
                if (peashooter[i].active) DrawRectangle(peashooter[i].position.x, peashooter[i].position.y, 16, 16, WHITE);
            }
}

static void Spawner(){
    if(currentTime >= timeBetweenSpawn){
        currentTime = 0;
        peashooter[enemyIndex].active = true;
        enemyIndex++;
    }
}