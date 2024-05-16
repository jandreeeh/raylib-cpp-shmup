#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <cmath>

#include "game.h"

typedef struct Player {
    Vector2 position;
    Texture2D sprite;
    int speed;
} Player;

//Kamekaze at player
typedef struct Peashooter {
    Vector2 position;
    Vector2 targetPos;  
    Rectangle hitbox;
    int health;
    double speed;
    bool active;
} Peashooter;

//Moves at a line
typedef struct Cadet {
    Vector2 position;
    Rectangle hitbox;
    Texture2D sprite;
    double speed;
    int health;
    int spawnLocation;
    double time;
    bool active;
} Cadet;

//Shoot at player
typedef struct Turret {
    Vector2 position;
    Vector2 targetPos;  
    Texture2D sprite;
    int health;
    double speed;
    double time;
    bool startShoot;
    bool active;
} Turret;

typedef struct Bullet{
    Vector2 position;
    Vector2 target;
    bool active;
} Bullet;

static void Spawner();

//Entity initialization
static Player player = { 0 };
static Peashooter peashooter[MAX_NUM_PEASHOOTER] = {0};
static Cadet upCadet[MAX_NUM_UP_CADET] = {0};
static Cadet downCadet[MAX_NUM_DOWN_CADET] = {0};
static Turret turret[MAX_NUM_TURRET] = {0};

//Peashooter spawner variables
static double peashooterTime = 0;
static int peashooterIndex = 0;
static double peashooterSpawnTime = 1;

//Cadet spawner variables
static double cadetTime = 0;
static int upCadetIndex = 0;
static int downCadetIndex = 0;
static double cadetSpawnTime = 5;
static double cadetPaceTime = 0;
static double cadetDeployTime = 0.4;
static int spawnPoint = 0;

static int turretPos = 0;
static int turretIndex = 0;

void GameInitialize(){
    //player variables
    player.position = (Vector2){ 100, VSCREEN_HEIGHT/2};
    player.speed = 250;
    player.sprite = LoadTexture("src/graphics/spaceship.png");

     //initialize peashooter
    for(int i = 0; i < MAX_NUM_PEASHOOTER; i++){
        peashooter[i].position.x = GetRandomValue(VSCREEN_WIDTH, VSCREEN_WIDTH + 100);
        peashooter[i].position.y = GetRandomValue(50, VSCREEN_HEIGHT - 50);
        peashooter[i].hitbox.height = 10;
        peashooter[i].hitbox.width = 10;
        peashooter[i].hitbox.x = peashooter[i].position.x;
        peashooter[i].hitbox.y = peashooter[i].position.y;
        peashooter[i].speed = GetRandomValue(250, 500);
        peashooter[i].health = 1;
        peashooter[i].active = false;
    }

    //initialize up cadet
    for(int i = 0; i < MAX_NUM_UP_CADET; i++){
        upCadet[i].position.x = 400;
        upCadet[i].position.y = -100;
        upCadet[i].speed = 150;
        upCadet[i].active = false;
        upCadet[i].spawnLocation = 0;
    }

    //initialize down cadet
    for(int i = 0; i < MAX_NUM_DOWN_CADET; i++){
        downCadet[i].position.x = 400;
        downCadet[i].position.y = VSCREEN_HEIGHT + 100;
        downCadet[i].speed = 150;
        downCadet[i].active = false;
        downCadet[i].spawnLocation = 0;
    }

    for(int i = 0; i < MAX_NUM_TURRET; i++){
        turret[i].position.x = GetRandomValue(VSCREEN_WIDTH, VSCREEN_WIDTH + 100);
        turret[i].position.y = GetRandomValue(-200, VSCREEN_HEIGHT + 200);
        turret[i].targetPos.x = VSCREEN_WIDTH/4 * 3;
        if(turretPos == 0){
            turret[i].targetPos.y = 50;
            turretPos = 1;
        }
        else if(turretPos == 1){
            turret[i].targetPos.y = VSCREEN_HEIGHT - 50 - 32;
            turretPos = 0;
        }
        
        turret[i].speed = 300;
        turret[i].startShoot = false;
        turret[i].active = false;
    }
    
}


void GameUpdate(){
    // Enemy spawner timer
    Spawner();
    //std::cout << "Cadet Time: " << cadetTime << std::endl;
    //std::cout << "Cadet Pace Time: " << cadetPaceTime << std::endl;

    //Player Input
    if (IsKeyDown(KEY_LEFT)){player.position.x -= player.speed * GetFrameTime();}
    if (IsKeyDown(KEY_RIGHT)){player.position.x += player.speed * GetFrameTime();}
    if (IsKeyDown(KEY_UP)){player.position.y -= player.speed * GetFrameTime();}
    if (IsKeyDown(KEY_DOWN)){player.position.y += player.speed * GetFrameTime();}


    //Enemy Movement
    //Peashooter:
    for (int i = 0; i < MAX_NUM_PEASHOOTER; i++){
        if (peashooter[i].active){
            //std::cout << "Im moving master!\n";
            peashooter[i].position.x -= peashooter[i].speed * GetFrameTime();
        }
        if (peashooter[i].position.x < -50){
            peashooter[i].active = false; 
            peashooter[i].position.x = GetRandomValue(VSCREEN_WIDTH, VSCREEN_WIDTH + 100);
            peashooter[i].position.y = GetRandomValue(50, VSCREEN_HEIGHT - 50);
            //peashooter[i].targetPos.x = -100;
            //peashooter[i].targetPos.y = peashooter[i].position.y;
        }
    }

    //Cadet:
    //Up Cadet
    for (int i = 0; i < MAX_NUM_UP_CADET; i++){
        if (upCadet[i].active){
            upCadet[i].time += GetFrameTime();
            upCadet[i].position.y += upCadet[i].speed * GetFrameTime();
            upCadet[i].position.x = 350 + cos(upCadet[i].time) * 50;
        }
        if (upCadet[i].position.y > VSCREEN_HEIGHT + 50){
            upCadet[i].position.x = 350;
            upCadet[i].position.y = -100;
            upCadet[i].active = false;
        }
    }
    //Down Cadet
    for (int i = 0; i < MAX_NUM_DOWN_CADET; i++){
        if (downCadet[i].active){
            downCadet[i].time += GetFrameTime();
            downCadet[i].position.y -= downCadet[i].speed * GetFrameTime();
            downCadet[i].position.x = 350 + sin(downCadet[i].time) * 50;
        }
        if (downCadet[i].position.y < -50){
            downCadet[i].position.x = 350;
            downCadet[i].position.y = VSCREEN_HEIGHT + 100;
            downCadet[i].time = 0;
            downCadet[i].active = false;
        }
    }

    //Turret:
    for (int i = 0; i < MAX_NUM_TURRET; i++){
        if(turret[i].active){
            if(!turret[i].startShoot){
                turret[i].position = Vector2MoveTowards(turret[i].position, turret[i].targetPos, turret[i].speed * GetFrameTime());

                if (Vector2Distance(turret[i].position, turret[i].targetPos) < 5){
                    turret[i].startShoot = true;
                }
            }
            else if(turret[i].startShoot){
                turret[i].time += GetFrameTime();
            }
        }
    }
    if(IsKeyPressed(KEY_ONE)){
        turret[turretIndex].active = true;
        turretIndex++;
    }
    
    
}

void GameDraw(){

    ClearBackground(BLACK);
    for (int i = 0; i < MAX_NUM_TURRET; i++){
        if (turret[i].active) DrawRectangle(turret[i].position.x, turret[i].position.y, 32, 32, GREEN);
    }
    for (int i = 0; i < MAX_NUM_PEASHOOTER; i++){
        if (peashooter[i].active) DrawRectangle(peashooter[i].position.x, peashooter[i].position.y, 32, 32, WHITE);
    }
    for (int i = 0; i < MAX_NUM_UP_CADET; i++){
        if (upCadet[i].active) DrawRectangle(upCadet[i].position.x, upCadet[i].position.y, 32, 32, RED);
    }
    for (int i = 0; i < MAX_NUM_DOWN_CADET; i++){
        if (downCadet[i].active) DrawRectangle(downCadet[i].position.x, downCadet[i].position.y, 32, 32, RED);
    }
    DrawTexture(player.sprite, player.position.x, player.position.y, WHITE);
}

static void Spawner(){
    peashooterTime += GetFrameTime();
    cadetTime += GetFrameTime();

    //Peashooter spawner
    if(peashooterTime >= peashooterSpawnTime){
        peashooterTime = 0;
        peashooter[peashooterIndex].active = true;
        peashooterIndex++;
    }
     if (peashooterIndex >= MAX_NUM_PEASHOOTER){
        peashooterIndex = 0;
    }

    
    if(cadetTime >= cadetSpawnTime){
        static int counter = 0;
        cadetPaceTime += GetFrameTime();
    
        if (spawnPoint == 0){
            if(counter < 5){
                if(cadetPaceTime >= cadetDeployTime){
                    std::cout << "Counter: " << counter;
                    upCadet[upCadetIndex].active = true;
                    cadetPaceTime = 0;
                    upCadetIndex++;
                    counter++;
                }
            }
            else {
                cadetTime = 0;
                counter = 0;
                spawnPoint = 1;
                std::cout << "Resetting now\n";
            }
        }
        else if (spawnPoint == 1){
            if(counter < 5){
                if(cadetPaceTime >= cadetDeployTime){
                    std::cout << "Counter: " << counter;
                    downCadet[downCadetIndex].active = true;
                    cadetPaceTime = 0;
                    downCadetIndex++;
                    counter++;
                }
            }
            else {
                cadetTime = 0;
                counter = 0;
                spawnPoint = 0;
                std::cout << "Resetting now\n";
            }
        }
        
    }
    if(upCadetIndex >= MAX_NUM_UP_CADET){
        upCadetIndex = 0;
    }
    if(downCadetIndex >= MAX_NUM_DOWN_CADET){
        downCadetIndex = 0;
    }
}