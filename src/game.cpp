#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <cmath>

#include "game.h"
#include "anim.h"

typedef struct Player {
    Vector2 position;
    Texture2D sprite;
    Rectangle hitbox;
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

//Shoots at player
typedef struct Turret {
    Vector2 position;
    Vector2 targetPos;
    Texture2D sprite;
    Rectangle hitbox;
    int health;
    int shootRate;
    double speed;
    double time;
    bool startShoot;
    bool active;
} Turret;

typedef struct Bullet{
    Vector2 position;
    Vector2 direction;
    Texture2D sprite;
    Rectangle hitbox;
    int speed;
    bool active;
    bool dirCalc;
} Bullet;

//Declaration of local functions
static void Spawner();
static void PeashooterDeactivate(int i);
static void UpCadetDeactivate(int i);
static void DownCadetDeactivate(int i);
static void TurretDeactivate(int i);
static void PlayerUpdate();
static void PeashooterUpdate();
static void CadetUpdate();
static void TurretUpdate();
static void PlayerBulletUpdate();

//-------------------------------------
//Local variables
//------------------------------------

//Entity declaration
static Player player = { 0 };
static Peashooter peashooter[MAX_NUM_PEASHOOTER] = {0};
static Cadet upCadet[MAX_NUM_UP_CADET] = {0};
static Cadet downCadet[MAX_NUM_DOWN_CADET] = {0};
static Turret turret[MAX_NUM_TURRET] = {0};
static Bullet pShot[MAX_PLAYER_BULLETS] = {0};
static Bullet tShot[MAX_TURRET_BULLETS] = {0};
static SpriteAnimate shipThrust = {0};

//General variables
static double spawnerTime = 0;

//Player related variables
static bool canShoot = true;
static double shootTime = 0;
static double shootRate = 1;
static int bulletIndex = 0;

//Peashooter spawner variables
static int peashooterStartSpawn = 0;
static bool canPeashooterSpawn = false;
static double peashooterTime = peashooterStartSpawn;
static int peashooterIndex = 0;
static double peashooterSpawnTime = 1;

//Cadet spawner variables
static double cadetXPos = 300;
static int cadetStartSpawn = 10;
static bool canCadetSpawn = false;
static double cadetTime = cadetStartSpawn;
static int upCadetIndex = 0;
static int downCadetIndex = 0;
static double cadetSpawnTime = 5;
static double cadetPaceTime = 0;
static double cadetDeployTime = 0.4;
static int spawnPoint = 0;

//Turret spawner variables
static int turretStartSpawn = 20;
static bool canTurretSpawn = false;
static double turretTime = turretStartSpawn;
static double turretSpawnTime = 10;
static int turretIndex = 0;
static int turretPos = 0;
static int turretSpawned = 0;
static int turretMax = 2;

void GameInitialize(){
    //player variables
    player.position = (Vector2){ 100, VSCREEN_HEIGHT/2};
    player.speed = 250;
    player.sprite = LoadTexture("src/graphics/spaceship.png");
    player.hitbox.height = player.sprite.height;
    player.hitbox.width = player.sprite.width;
    player.hitbox.x = player.position.x;
    player.hitbox.y = player.position.y;

    shipThrust.atlas = LoadTexture("src/graphics/spaceship_thrust.png");
    shipThrust.position = player.position;
    shipThrust.currentFrame = 0;
    shipThrust.numFrames = 4;
    shipThrust.framesPerSecond = 16;
    shipThrust.source = {0.0f, 0.0f, (float)shipThrust.atlas.width/shipThrust.numFrames, (float)shipThrust.atlas.height};

     //initialize peashooter
    for(int i = 0; i < MAX_NUM_PEASHOOTER; i++){
        peashooter[i].position.x = GetRandomValue(VSCREEN_WIDTH, VSCREEN_WIDTH + 100);
        peashooter[i].position.y = GetRandomValue(50, VSCREEN_HEIGHT - 50);
        peashooter[i].hitbox.height = 32;           //Change value if sprite is made
        peashooter[i].hitbox.width = 32;
        peashooter[i].hitbox.x = peashooter[i].position.x;
        peashooter[i].hitbox.y = peashooter[i].position.y;
        peashooter[i].speed = GetRandomValue(250, 500);
        peashooter[i].health = 2;
        peashooter[i].active = false;
    }

    //initialize up cadet
    for(int i = 0; i < MAX_NUM_UP_CADET; i++){
        upCadet[i].position.x = cadetXPos;
        upCadet[i].position.y = -100;
        upCadet[i].hitbox.height = 32;           //Change value if sprite is made
        upCadet[i].hitbox.width = 32;
        upCadet[i].hitbox.x = upCadet[i].position.x;
        upCadet[i].hitbox.y = upCadet[i].position.y;
        upCadet[i].health = 2;
        upCadet[i].speed = 100;
        upCadet[i].active = false;
        upCadet[i].spawnLocation = 0;
    }

    //initialize down cadet
    for(int i = 0; i < MAX_NUM_DOWN_CADET; i++){
        downCadet[i].position.x = cadetXPos;
        downCadet[i].position.y = VSCREEN_HEIGHT + 100;
        downCadet[i].hitbox.height = 32;           //Change value if sprite is made
        downCadet[i].hitbox.width = 32;
        downCadet[i].hitbox.x = downCadet[i].position.x;
        downCadet[i].hitbox.y = downCadet[i].position.y;
        downCadet[i].health = 2;
        downCadet[i].speed = 100;
        downCadet[i].active = false;
        downCadet[i].spawnLocation = 0;
    }

    //initialize turrets
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
        turret[i].hitbox.height = 32;           //Change value if sprite is made
        turret[i].hitbox.width = 32;
        turret[i].shootRate = 3;
        turret[i].hitbox.x = turret[i].position.x;
        turret[i].hitbox.y = turret[i].position.y;
        turret[i].health = 4;
        turret[i].speed = 200;
        turret[i].startShoot = false;
        turret[i].active = false;
    }
    //Initialize turret bullets
    for(int i = 0; i < MAX_TURRET_BULLETS; i++){
        tShot[i].position.x = VSCREEN_WIDTH/2;  //Temporary position: Remove if done testing
        tShot[i].position.y = VSCREEN_HEIGHT/2;
        tShot[i].sprite = LoadTexture("src/graphics/enemy_proj.png");
        tShot[i].hitbox.height = tShot[i].sprite.height;
        tShot[i].hitbox.width = tShot[i].sprite.width;
        tShot[i].speed = 250;
        tShot[i].active = false;
        tShot[i].dirCalc = false;
    }

    //initialize player bullets
    for(int i = 0; i < MAX_PLAYER_BULLETS; i++){
        pShot[i].sprite = LoadTexture("src/graphics/player_proj.png");
        pShot[i].hitbox.height = pShot[i].sprite.height;
        pShot[i].hitbox.width = pShot[i].sprite.width;
        pShot[i].speed = 500;
        pShot[i].active = false;
    }
}


void GameUpdate(){
    // Handles enemy spawning
    Spawner();

    //Thruster animation
    AnimationUpdate(shipThrust.atlas, shipThrust.source, shipThrust.currentFrame, shipThrust.framesPerSecond, shipThrust.numFrames);
    shipThrust.position.x = player.position.x - shipThrust.atlas.width/shipThrust.numFrames;
    shipThrust.position.y = player.position.y + (player.sprite.height/2 - shipThrust.atlas.height/2);

    //Player input and update
    PlayerUpdate();

    //Enemy Movement
    //Peashooter:
    PeashooterUpdate();

    //Cadet:
    CadetUpdate();

    //Turret:
    TurretUpdate();

    //Player Bullet:
    PlayerBulletUpdate();

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
    for (int i = 0; i < MAX_PLAYER_BULLETS; i++){
        if (pShot[i].active) DrawTexture(pShot[i].sprite, pShot[i].position.x, pShot[i].position.y, WHITE);
    }
    for(int i = 0; i < MAX_TURRET_BULLETS; i++){
        if(tShot[i].active) DrawTexture(tShot[i].sprite, tShot[i].position.x, tShot[i].position.y, WHITE);
    }
    DrawTextureRec(shipThrust.atlas, shipThrust.source, shipThrust.position, WHITE);
    DrawTexture(player.sprite, player.position.x, player.position.y, WHITE);
}

static void Spawner(){
    spawnerTime += GetFrameTime();
    std::cout << spawnerTime << "\n";

    if(spawnerTime > peashooterStartSpawn) canPeashooterSpawn = true;
    if(spawnerTime > cadetStartSpawn) canCadetSpawn = true;
    if(spawnerTime > turretStartSpawn) canTurretSpawn = true;

    //Peashooter Spawner: Spawn at the start
    if(canPeashooterSpawn){
        peashooterTime += GetFrameTime();
        if(peashooterTime >= peashooterSpawnTime){
            peashooterTime = 0;
            peashooter[peashooterIndex].active = true;
            peashooterIndex++;
        }
        if (peashooterIndex >= MAX_NUM_PEASHOOTER){
            peashooterIndex = 0;
        }
    }

    //Cadet Spawner: Spawns at 10 seconds after start
    if(canCadetSpawn){
        cadetTime += GetFrameTime();
        if(cadetTime >= cadetSpawnTime){
            static int counter = 0;
            cadetPaceTime += GetFrameTime();

            if (spawnPoint == 0){
                if(counter < 5){
                    if(cadetPaceTime >= cadetDeployTime){
                        //std::cout << "Counter: " << counter;
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
                    //std::cout << "Resetting now\n";
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

    //Turret Spawner: spawns 20 seconds after start
    if(canTurretSpawn){
        std::cout << "Spawned turrets: " << turretSpawned << "\n";
        if(turretSpawned < turretMax){
            turretTime += GetFrameTime();

            if(turretTime > turretSpawnTime){
                turret[turretIndex].active = true;
                turretTime = 0;
                turretIndex++;
                turretSpawned++;
            }
        }
        if(turretIndex >= MAX_NUM_TURRET){
            turretIndex = 0;
        }
    }
}

static void PeashooterDeactivate(int i){
    //std::cout << "Peashooter " << i << " is deactivated\n";
    peashooter[i].active = false;
    peashooter[i].position.x = GetRandomValue(VSCREEN_WIDTH, VSCREEN_WIDTH + 100);
    peashooter[i].position.y = GetRandomValue(50, VSCREEN_HEIGHT - 50);
}
static void UpCadetDeactivate(int i){
    upCadet[i].position.x = cadetXPos;
    upCadet[i].position.y = -100;
    upCadet[i].active = false;
}
static void DownCadetDeactivate(int i){
    downCadet[i].position.x = cadetXPos;
    downCadet[i].position.y = VSCREEN_HEIGHT + 100;
    downCadet[i].time = 0;
    downCadet[i].active = false;
}
static void TurretDeactivate(int i){
    turret[i].active = false;
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
}

static void PlayerUpdate(){
    if (IsKeyDown(KEY_LEFT)){player.position.x -= player.speed * GetFrameTime();}
    if (IsKeyDown(KEY_RIGHT)){player.position.x += player.speed * GetFrameTime();}
    if (IsKeyDown(KEY_UP)){player.position.y -= player.speed * GetFrameTime();}
    if (IsKeyDown(KEY_DOWN)){player.position.y += player.speed * GetFrameTime();}

    if(player.position.x < 0){player.position.x = (float)0;}
    if(player.position.x > VSCREEN_WIDTH - player.sprite.width){player.position.x = VSCREEN_WIDTH - player.sprite.width;}
    if(player.position.y < 0){player.position.y = (float)0;}
    if(player.position.y > VSCREEN_HEIGHT - player.sprite.height){player.position.y = VSCREEN_HEIGHT - player.sprite.height;}

    if(IsKeyDown(KEY_SPACE)){
        for(int i = 0; i < MAX_PLAYER_BULLETS; i++){
            if(canShoot){
                if (!pShot[i].active){
                    pShot[i].position.x = player.position.x + player.sprite.width;
                    pShot[i].position.y = player.position.y + (player.sprite.height/2 - pShot[i].sprite.height/2);
                    pShot[i].active = true;
                    canShoot = false;
                    break;
                }
            }
                if(!canShoot){
                    shootTime += GetFrameTime();
                    if(shootTime > 10){
                        shootTime = 0;
                        canShoot = true;
                    }
                }
        }
    }
    else if(!IsKeyDown(KEY_SPACE)){
         canShoot = true;
         shootTime = 0;
    }
}

static void PeashooterUpdate(){
    for (int i = 0; i < MAX_NUM_PEASHOOTER; i++){
        if (peashooter[i].active){
            peashooter[i].position.x -= peashooter[i].speed * GetFrameTime();
            peashooter[i].hitbox.x = peashooter[i].position.x;
            peashooter[i].hitbox.y = peashooter[i].position.y;
        }
        if (peashooter[i].position.x < -50){
            PeashooterDeactivate(i);
        }
    }
}

static void CadetUpdate(){
    //Up Cadet
    for (int i = 0; i < MAX_NUM_UP_CADET; i++){
        if (upCadet[i].active){
            upCadet[i].time += GetFrameTime();
            upCadet[i].position.y += upCadet[i].speed * GetFrameTime();
            upCadet[i].position.x = cadetXPos + cos(upCadet[i].time) * 50;
            upCadet[i].hitbox.x = upCadet[i].position.x;
            upCadet[i].hitbox.y = upCadet[i].position.y;
        }
        if (upCadet[i].position.y > VSCREEN_HEIGHT + 50){
            UpCadetDeactivate(i);
        }
    }
    //Down Cadet
    for (int i = 0; i < MAX_NUM_DOWN_CADET; i++){
        if (downCadet[i].active){
            downCadet[i].time += GetFrameTime();
            downCadet[i].position.y -= downCadet[i].speed * GetFrameTime();
            downCadet[i].position.x = cadetXPos + cos(downCadet[i].time) * 50;
            downCadet[i].hitbox.x = downCadet[i].position.x;
            downCadet[i].hitbox.y = downCadet[i].position.y;
        }
        if (downCadet[i].position.y < -50){
            DownCadetDeactivate(i);
        }
    }
}

static void TurretUpdate(){
    for (int i = 0; i < MAX_NUM_TURRET; i++){
        if(turret[i].active){

            turret[i].hitbox.x = turret[i].position.x;
            turret[i].hitbox.y = turret[i].position.y;

            if(!turret[i].startShoot){
                turret[i].position = Vector2MoveTowards(turret[i].position, turret[i].targetPos, turret[i].speed * GetFrameTime());

                if (Vector2Distance(turret[i].position, turret[i].targetPos) < 5){
                    turret[i].startShoot = true;
                }
            }
            else if(turret[i].startShoot){
                turret[i].time += GetFrameTime();
                if(turret[i].time > turret[i].shootRate){
                    tShot[bulletIndex].position = turret[i].position;
                    tShot[bulletIndex].active = true;
                    bulletIndex++;
                    turret[i].time = 0;
                }
            }
        }
    }
    if(bulletIndex > MAX_TURRET_BULLETS){
        bulletIndex = 0;
    }

    for(int i = 0; i < MAX_TURRET_BULLETS; i++){
        if(tShot[i].active){
            if(!tShot[i].dirCalc){
                tShot[i].direction = Vector2Normalize(Vector2Subtract(player.position, tShot[i].position));
                tShot[i].dirCalc = true;
            }
            else if(tShot[i].dirCalc){
                tShot[i].position.x += tShot[i].direction.x * tShot[i].speed * GetFrameTime();
                tShot[i].position.y += tShot[i].direction.y * tShot[i].speed * GetFrameTime();
            }
        }
    }
}

static void PlayerBulletUpdate(){
    for(int i = 0; i < MAX_PLAYER_BULLETS; i++){
        if(pShot[i].active){
            pShot[i].position.x += pShot[i].speed * GetFrameTime();
            pShot[i].hitbox.x = pShot[i].position.x;
            pShot[i].hitbox.y = pShot[i].position.y;

            if(pShot[i].position.x > VSCREEN_WIDTH){
                pShot[i].active = false;
            }

            //Bullet enemy collision
            for (int j = 0; j < MAX_NUM_PEASHOOTER; j++){
                if(peashooter[j].active){
                    if(CheckCollisionRecs(pShot[i].hitbox, peashooter[j].hitbox)){
                        peashooter[j].health--;
                        if(peashooter[j].health <= 0){
                            PeashooterDeactivate(j);
                        }
                        std::cout << "Peashooter " << i << " collided\n";
                        pShot[i].active = false;
                    }
                }
            }
            for (int j = 0; j < MAX_NUM_UP_CADET; j++){
                if(upCadet[j].active){
                    if(CheckCollisionRecs(pShot[i].hitbox, upCadet[j].hitbox)){
                        upCadet[j].health--;
                        if(upCadet[j].health <= 0){
                            UpCadetDeactivate(j);
                        }
                        pShot[i].active = false;
                    }
                }
            }
            for (int j = 0; j < MAX_NUM_DOWN_CADET; j++){
                if(downCadet[j].active){
                    if(CheckCollisionRecs(pShot[i].hitbox, downCadet[j].hitbox)){
                        downCadet[j].health--;
                        if(downCadet[j].health <= 0){
                            DownCadetDeactivate(j);
                        }
                        pShot[i].active = false;
                    }
                }
            }
            for (int j = 0; j < MAX_NUM_TURRET; j++){
                if(turret[j].active){
                    if(CheckCollisionRecs(pShot[i].hitbox, turret[j].hitbox)){
                        turret[j].health--;
                        if(turret[j].health <=0){
                        TurretDeactivate(j);
                        turretSpawned--;
                        }
                        pShot[i].active = false;
                    }
                }
            }
        }
    }
}