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
    Color color;
    int speed;
    bool hit;
} Player;

//Kamekaze at player
typedef struct Peashooter {
    Vector2 position;
    Vector2 targetPos;
    Rectangle hitbox;
    SpriteAnimate sprite;
    Color color;
    int health;
    float speed;
    bool active;
    bool hasShot;
} Peashooter;

//Moves at a line
typedef struct Cadet {
    Vector2 position;
    Rectangle hitbox;
    SpriteAnimate sprite;
    Color color;
    float speed;
    int health;
    int spawnLocation;
    float time;
    bool active;
    bool hasShot;
} Cadet;

//Shoots at player
typedef struct Turret {
    Vector2 position;
    Vector2 targetPos;
    SpriteAnimate sprite;
    Rectangle hitbox;
    Color color;
    int health;
    int shootRate;
    float speed;
    float time;
    bool startShoot;
    bool active;
    bool hasShot;
} Turret;

typedef struct Bullet{
    Vector2 position;
    Vector2 direction;
    Texture2D sprite;
    Rectangle hitbox;
    Color color;
    int speed;
    bool active;
    bool dirCalc;
    bool hasShot;
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
static void PlayerCollision();
static void DamageFlash(Color& color, bool& shot);
static void ParalaxBGUpdate();
static void ParalaxBGDraw();

//-------------------------------------
//    Local variables
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
static float spawnerTime = 0;
static bool gameOver = false;
static float damageTime = 0;
static Color blue_space = { 34,34,52,255 };

//Variables for parallax backgrounds
float scrollingBack = 0.0f;
float scrollingMid1 = 0.0f;
float scrollingMid2 = 0.0f;
float scrollingFore = 0.0f;

Texture2D background;
Texture2D midground1;
Texture2D midground2;
Texture2D foreground;

static Vector2 bg1;
static Vector2 bg2;
static Vector2 mg11;
static Vector2 mg12;
static Vector2 mg21;
static Vector2 mg22;
static Vector2 fg1;
static Vector2 fg2;

//Player related variables
static bool canShoot = true;
static float shootTime = 0;
static int shootRate = 5;
static int bulletIndex = 0;
static int playerScore = 0;
static int playerKills= 0;
static int playerHealth = 3;

//Peashooter spawner variables
static int peashooterStartSpawn = 0;
static bool canPeashooterSpawn = false;
static float peashooterTime = peashooterStartSpawn;
static int peashooterIndex = 0;
static float peashooterSpawnTime = 1;

//Cadet spawner variables
static float cadetXPos = 300;
static int cadetStartSpawn = 10;
static bool canCadetSpawn = false;
static float cadetTime = cadetStartSpawn;
static int upCadetIndex = 0;
static int downCadetIndex = 0;
static float cadetSpawnTime = 5;
static float cadetPaceTime = 0;
static float cadetDeployTime = 0.4;
static int spawnPoint = 0;

//Turret spawner variables
static int turretStartSpawn = 20;
static bool canTurretSpawn = false;
static float turretTime = turretStartSpawn;
static float turretSpawnTime = 10;
static int turretIndex = 0;
static int turretPos = 0;
static int turretSpawned = 0;
static int turretMax = 2;

void GameInitialize(){
    //player variables
    player.position.x = 100;
    player.position.y = VSCREEN_HEIGHT / 2;
    player.speed = 250;
    player.sprite = LoadTexture("game/src/graphics/spaceship.png");
    player.hitbox.height = player.sprite.height;
    player.hitbox.width = player.sprite.width;
    player.hitbox.x = player.position.x;
    player.hitbox.y = player.position.y;
    player.color = WHITE;
    player.hit = false;

    shipThrust.atlas = LoadTexture("game/src/graphics/spaceship_thrust.png");
    shipThrust.position = player.position;
    shipThrust.currentFrame = 0;
    shipThrust.numFrames = 4;
    shipThrust.framesPerSecond = 16;
    shipThrust.source = {0.0f, 0.0f, (float)shipThrust.atlas.width/shipThrust.numFrames, (float)shipThrust.atlas.height};

     //initialize peashooter
    for(int i = 0; i < MAX_NUM_PEASHOOTER; i++){
        peashooter[i].position.x = GetRandomValue(VSCREEN_WIDTH, VSCREEN_WIDTH + 100);
        peashooter[i].position.y = GetRandomValue(50, VSCREEN_HEIGHT - 50);
        peashooter[i].speed = GetRandomValue(250, 500);
        peashooter[i].health = 2;
        peashooter[i].active = false;
        peashooter[i].color = WHITE;
        peashooter[i].hasShot = false;

        peashooter[i].sprite.atlas = LoadTexture("game/src/graphics/peashooter.png");
        peashooter[i].sprite.position = peashooter[i].position;
        peashooter[i].sprite.currentFrame = 0;
        peashooter[i].sprite.numFrames = 4;
        peashooter[i].sprite.framesPerSecond = 16;

        peashooter[i].sprite.source = {0.0f, 0.0f, (float)peashooter[i].sprite.atlas.width/peashooter[i].sprite.numFrames, (float)peashooter[i].sprite.atlas.height};
        peashooter[i].hitbox.height = peashooter[i].sprite.atlas.height;          
        peashooter[i].hitbox.width = peashooter[i].sprite.atlas.width;
        peashooter[i].hitbox.x = peashooter[i].position.x;
        peashooter[i].hitbox.y = peashooter[i].position.y;
    }

    //initialize up cadet
    for(int i = 0; i < MAX_NUM_UP_CADET; i++){
        upCadet[i].position.x = cadetXPos;
        upCadet[i].position.y = -100;
        upCadet[i].health = 3;
        upCadet[i].speed = 100;
        upCadet[i].active = false;
        upCadet[i].spawnLocation = 0;
        upCadet[i].color = WHITE;
        upCadet[i].hasShot = false;

        upCadet[i].sprite.atlas = LoadTexture("game/src/graphics/cadet.png");
        upCadet[i].sprite.position = upCadet[i].position;
        upCadet[i].sprite.currentFrame = 0;
        upCadet[i].sprite.numFrames = 3;
        upCadet[i].sprite.framesPerSecond = 9;
        upCadet[i].sprite.source = { 0.0f, 0.0f, (float)upCadet[i].sprite.atlas.width / upCadet[i].sprite.numFrames, (float)upCadet[i].sprite.atlas.height };

        upCadet[i].hitbox.height = upCadet[i].sprite.atlas.height;          
        upCadet[i].hitbox.width = upCadet[i].sprite.atlas.width;
        upCadet[i].hitbox.x = upCadet[i].position.x;
        upCadet[i].hitbox.y = upCadet[i].position.y;
    }

    //initialize down cadet
    for(int i = 0; i < MAX_NUM_DOWN_CADET; i++){
        downCadet[i].position.x = cadetXPos;
        downCadet[i].position.y = VSCREEN_HEIGHT + 100;
        downCadet[i].health = 3;
        downCadet[i].speed = 100;
        downCadet[i].active = false;
        downCadet[i].spawnLocation = 0;
        downCadet[i].color = WHITE;
        downCadet[i].hasShot = false;

        downCadet[i].sprite.atlas = LoadTexture("game/src/graphics/cadet.png");
        downCadet[i].sprite.position = downCadet[i].position;
        downCadet[i].sprite.currentFrame = 0;
        downCadet[i].sprite.numFrames = 3;
        downCadet[i].sprite.framesPerSecond = 9;
        downCadet[i].sprite.source = { 0.0f, 0.0f, (float)downCadet[i].sprite.atlas.width / downCadet[i].sprite.numFrames, (float)downCadet[i].sprite.atlas.height };

        downCadet[i].hitbox.height = downCadet[i].sprite.atlas.height;          
        downCadet[i].hitbox.width = downCadet[i].sprite.atlas.width;
        downCadet[i].hitbox.x = downCadet[i].position.x;
        downCadet[i].hitbox.y = downCadet[i].position.y;
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
        turret[i].color = WHITE;

        turret[i].sprite.atlas = LoadTexture("game/src/graphics/turret.png");
        turret[i].sprite.position = turret[i].position;
        turret[i].sprite.currentFrame = 0;
        turret[i].sprite.numFrames = 4;
        turret[i].sprite.framesPerSecond = 16;
        turret[i].sprite.source = { 0.0f, 0.0f, (float)turret[i].sprite.atlas.width / turret[i].sprite.numFrames, (float)turret[i].sprite.atlas.height };

        turret[i].hitbox.height = turret[i].sprite.atlas.height;
        turret[i].hitbox.width = turret[i].sprite.atlas.width;
        turret[i].hitbox.x = turret[i].position.x;
        turret[i].hitbox.y = turret[i].position.y;
        turret[i].shootRate = 3;
        turret[i].health = 4;
        turret[i].speed = 200;
        turret[i].startShoot = false;
        turret[i].active = false;
        turret[i].hasShot = false;
    }
    //Initialize turret bullets
    for(int i = 0; i < MAX_TURRET_BULLETS; i++){
        tShot[i].position.x = VSCREEN_WIDTH/2;  //Temporary position: Remove if done testing
        tShot[i].position.y = VSCREEN_HEIGHT/2;
        tShot[i].sprite = LoadTexture("game/src/graphics/enemy_proj.png");
        tShot[i].hitbox.height = tShot[i].sprite.height;
        tShot[i].hitbox.width = tShot[i].sprite.width;
        tShot[i].speed = 250;
        tShot[i].active = false;
        tShot[i].dirCalc = false;
    }

    //initialize player bullets
    for(int i = 0; i < MAX_PLAYER_BULLETS; i++){
        pShot[i].sprite = LoadTexture("game/src/graphics/player_proj.png");
        pShot[i].hitbox.height = pShot[i].sprite.height;
        pShot[i].hitbox.width = pShot[i].sprite.width;
        pShot[i].speed = 500;
        pShot[i].active = false;
    }

    background = LoadTexture("game/src/graphics/background_1.png");
    midground1 = LoadTexture("game/src/graphics/background_4.png");
    midground2 = LoadTexture("game/src/graphics/background_3.png");
    foreground = LoadTexture("game/src/graphics/background_2.png");
}


void GameUpdate(){


    if(!gameOver){
        // Handles enemy spawning
        ParalaxBGUpdate();
        Spawner();

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

        PlayerCollision();

        if(playerHealth == 0){
            gameOver = true;
        }
    }

}

void GameDraw(){

    ClearBackground(blue_space);
    ParalaxBGDraw();

    for (int i = 0; i < MAX_NUM_TURRET; i++){
        if (turret[i].active) DrawTextureRec(turret[i].sprite.atlas, turret[i].sprite.source, turret[i].sprite.position, turret[i].color);
    }
    for (int i = 0; i < MAX_NUM_PEASHOOTER; i++){
        if (peashooter[i].active) DrawTextureRec(peashooter[i].sprite.atlas, peashooter[i].sprite.source, peashooter[i].sprite.position, peashooter[i].color);
    }
    for (int i = 0; i < MAX_NUM_UP_CADET; i++){
        if (upCadet[i].active) DrawTextureRec(upCadet[i].sprite.atlas, upCadet[i].sprite.source, upCadet[i].sprite.position, upCadet[i].color);
    }
    for (int i = 0; i < MAX_NUM_DOWN_CADET; i++){
        if (downCadet[i].active) DrawTextureRec(downCadet[i].sprite.atlas, downCadet[i].sprite.source, downCadet[i].sprite.position, downCadet[i].color);
    }
    for (int i = 0; i < MAX_PLAYER_BULLETS; i++){
        if (pShot[i].active) DrawTexture(pShot[i].sprite, pShot[i].position.x, pShot[i].position.y, WHITE);
    }
    for(int i = 0; i < MAX_TURRET_BULLETS; i++){
        if(tShot[i].active) DrawTexture(tShot[i].sprite, tShot[i].position.x, tShot[i].position.y, WHITE);
    }
    DrawTextureRec(shipThrust.atlas, shipThrust.source, shipThrust.position, WHITE);
    DrawTexture(player.sprite, player.position.x, player.position.y, player.color);

    DrawText(TextFormat("Score: %06i", playerScore), 10, 20, 5, WHITE);
    DrawText(TextFormat("Kills: %d", playerKills), 10, 35, 5, WHITE);
    DrawText(TextFormat("Health: %d", playerHealth), 10, 50, 5, WHITE);
}

void GameDeInitialize() {

    // Unloads texture before game will close
    UnloadTexture(player.sprite);
    UnloadTexture(shipThrust.atlas);
    for (int i = 0; i < MAX_NUM_PEASHOOTER; i++) { UnloadTexture(peashooter[i].sprite.atlas); };
    for (int i = 0; i < MAX_NUM_UP_CADET; i++) { UnloadTexture(upCadet[i].sprite.atlas); };
    for (int i = 0; i < MAX_NUM_DOWN_CADET; i++) { UnloadTexture(downCadet[i].sprite.atlas); };
    for (int i = 0; i < MAX_NUM_TURRET; i++) { UnloadTexture(turret[i].sprite.atlas); };
    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) { UnloadTexture(pShot[i].sprite); };
    for (int i = 0; i < MAX_TURRET_BULLETS; i++) { UnloadTexture(tShot[i].sprite); };

    UnloadTexture(background);
    UnloadTexture(midground1);
    UnloadTexture(midground2);
    UnloadTexture(foreground);
   

}

static void Spawner(){
    spawnerTime += GetFrameTime();
    //std::cout << spawnerTime << "\n";

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
                        //std::cout << "Counter: " << counter;
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
                    //std::cout << "Resetting now\n";
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
        //std::cout << "Spawned turrets: " << turretSpawned << "\n";
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
    peashooter[i].health = 2;
}
static void UpCadetDeactivate(int i){
    upCadet[i].position.x = cadetXPos;
    upCadet[i].position.y = -100;
    upCadet[i].active = false;
    upCadet[i].health = 3;
}
static void DownCadetDeactivate(int i){
    downCadet[i].position.x = cadetXPos;
    downCadet[i].position.y = VSCREEN_HEIGHT + 100;
    downCadet[i].time = 0;
    downCadet[i].active = false;
    downCadet[i].health = 3;
}
static void TurretDeactivate(int i){
    turret[i].active = false;
    turret[i].position.x = GetRandomValue(VSCREEN_WIDTH, VSCREEN_WIDTH + 100);
    turret[i].position.y = GetRandomValue(-200, VSCREEN_HEIGHT + 200);
    turret[i].health = 4;
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

    player.hitbox.x = player.position.x;
    player.hitbox.y = player.position.y;

    //Thruster animation
    AnimationUpdate(shipThrust.atlas, shipThrust.source, shipThrust.currentFrame, shipThrust.framesPerSecond, shipThrust.numFrames);
    shipThrust.position.x = player.position.x - shipThrust.atlas.width / shipThrust.numFrames;
    shipThrust.position.y = player.position.y + (player.sprite.height / 2 - shipThrust.atlas.height / 2);

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
                    if(shootTime > shootRate){
                        shootTime = 0;
                        canShoot = true;
                    }
                }
        }
    }
        else if(!IsKeyDown(KEY_SPACE)){
            shootTime += GetFrameTime();
            if(shootTime > shootRate){
                shootTime = 0;
                canShoot = true;
            }
        }
    if (player.hit) {
        DamageFlash(player.color, player.hit);
    }
}

static void PeashooterUpdate(){
    for (int i = 0; i < MAX_NUM_PEASHOOTER; i++){
        if (peashooter[i].active){
            peashooter[i].position.x -= peashooter[i].speed * GetFrameTime();
            peashooter[i].hitbox.x = peashooter[i].position.x;
            peashooter[i].hitbox.y = peashooter[i].position.y;

            AnimationUpdate(peashooter[i].sprite.atlas, peashooter[i].sprite.source, peashooter[i].sprite.currentFrame, peashooter[i].sprite.framesPerSecond, peashooter[i].sprite.numFrames);
            peashooter[i].sprite.position = peashooter[i].position;
        }
        if (peashooter[i].position.x < -50){
            PeashooterDeactivate(i);
        }
        if (peashooter[i].hasShot) {
            DamageFlash(peashooter[i].color, peashooter[i].hasShot);
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

            AnimationUpdate(upCadet[i].sprite.atlas, upCadet[i].sprite.source, upCadet[i].sprite.currentFrame, upCadet[i].sprite.framesPerSecond, upCadet[i].sprite.numFrames);
            upCadet[i].sprite.position = upCadet[i].position;
        }
        if (upCadet[i].position.y > VSCREEN_HEIGHT + 50){
            UpCadetDeactivate(i);
        }
        if (upCadet[i].hasShot) {
            DamageFlash(upCadet[i].color, upCadet[i].hasShot);
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

            AnimationUpdate(downCadet[i].sprite.atlas, downCadet[i].sprite.source, downCadet[i].sprite.currentFrame, downCadet[i].sprite.framesPerSecond, downCadet[i].sprite.numFrames);
            downCadet[i].sprite.position = downCadet[i].position;
        }
        if (downCadet[i].position.y < -50){
            DownCadetDeactivate(i);
        }
        if (downCadet[i].hasShot) {
            DamageFlash(downCadet[i].color, downCadet[i].hasShot);
        }
    }
}

static void TurretUpdate(){
    for (int i = 0; i < MAX_NUM_TURRET; i++){
        if(turret[i].active){

            turret[i].hitbox.x = turret[i].position.x;
            turret[i].hitbox.y = turret[i].position.y;

            AnimationUpdate(turret[i].sprite.atlas, turret[i].sprite.source, turret[i].sprite.currentFrame, turret[i].sprite.framesPerSecond, turret[i].sprite.numFrames);
            turret[i].sprite.position = turret[i].position;

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
            if (turret[i].hasShot) {
                DamageFlash(turret[i].color, turret[i].hasShot);
            }
        }
    }
    if(bulletIndex > MAX_TURRET_BULLETS){
        bulletIndex = 0;
    }

    for(int i = 0; i < MAX_TURRET_BULLETS; i++){
        if(tShot[i].active){
            tShot[i].hitbox.x = tShot[i].position.x;
            tShot[i].hitbox.y = tShot[i].position.y;

            if(!tShot[i].dirCalc){
                tShot[i].direction = Vector2Normalize(Vector2Subtract(player.position, tShot[i].position));
                tShot[i].dirCalc = true;
            }
            else if(tShot[i].dirCalc){
                tShot[i].position.x += tShot[i].direction.x * tShot[i].speed * GetFrameTime();
                tShot[i].position.y += tShot[i].direction.y * tShot[i].speed * GetFrameTime();
            }

            if (tShot[i].position.x < -50 || tShot[i].position.x > VSCREEN_WIDTH + 50 || tShot[i].position.y < -50 || tShot[i].position.y > VSCREEN_HEIGHT + 50) {
                tShot[i].active = false;
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
                            playerKills++;
                            playerScore += 100;
                        }
                        //std::cout << "Peashooter " << i << " collided\n";
                        pShot[i].active = false;
                        peashooter[j].hasShot = true;
                    }
                    
                }
            }
            for (int j = 0; j < MAX_NUM_UP_CADET; j++){
                if(upCadet[j].active){
                    if(CheckCollisionRecs(pShot[i].hitbox, upCadet[j].hitbox)){
                        upCadet[j].health--;
                        if(upCadet[j].health <= 0){
                            UpCadetDeactivate(j);
                            playerKills++;
                            playerScore += 150;
                        }
                        pShot[i].active = false;
                        upCadet[j].hasShot = true;
                    }
                }
            }
            for (int j = 0; j < MAX_NUM_DOWN_CADET; j++){
                if(downCadet[j].active){
                    if(CheckCollisionRecs(pShot[i].hitbox, downCadet[j].hitbox)){
                        downCadet[j].health--;
                        if(downCadet[j].health <= 0){
                            DownCadetDeactivate(j);
                            playerKills++;
                            playerScore += 150;
                        }
                        pShot[i].active = false;
                        downCadet[j].hasShot = true;
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
                            playerKills++;
                            playerScore += 200;
                        }
                        pShot[i].active = false;
                        turret[j].hasShot = true;
                    }
                }
            }
        }
    }
}

static void PlayerCollision(){

    for (int j = 0; j < MAX_NUM_PEASHOOTER; j++){
        if(peashooter[j].active){
            if(CheckCollisionRecs(player.hitbox, peashooter[j].hitbox)){
                PeashooterDeactivate(j);
                playerHealth--;
                player.hit = true;
            }   
        }
    }
    for (int j = 0; j < MAX_NUM_UP_CADET; j++){
        if(upCadet[j].active){
            if(CheckCollisionRecs(player.hitbox, upCadet[j].hitbox)){
                UpCadetDeactivate(j);
                playerHealth--;
                player.hit = true;
            }
        }
    }
    for (int j = 0; j < MAX_NUM_DOWN_CADET; j++){
        if(downCadet[j].active){
            if(CheckCollisionRecs(player.hitbox, downCadet[j].hitbox)){
                DownCadetDeactivate(j);
                playerHealth--;
                player.hit = true;
            }
                        
        }
    }
    for (int j = 0; j < MAX_NUM_TURRET; j++){
        if(turret[j].active){
            if(CheckCollisionRecs(player.hitbox, turret[j].hitbox)){
                PeashooterDeactivate(j);
                playerHealth--;
                player.hit = true;
            }
        }
    }
    for (int j = 0; j < MAX_TURRET_BULLETS; j++) {
        if (tShot[j].active) {
            if (CheckCollisionRecs(player.hitbox, tShot[j].hitbox)) {
                tShot[j].active = false;
                playerHealth--;
                player.hit = true;
            }
        }
    }
}

static void DamageFlash(Color& color, bool& shot) {
    damageTime += GetFrameTime();
    //std::cout << damageTime << "\n";
    color = RED;

    if (damageTime > 0.1 && shot) {
        color = WHITE;
        shot = false;
        damageTime = 0;
        return;
    }
 
}

static void ParalaxBGUpdate() {
    scrollingBack -= 0.1f;
    scrollingMid1 -= 0.5f;
    scrollingMid2 -= 1.0f;
    scrollingFore -= 1.5f;

    std::cout << bg1.x << "\n";
    // NOTE: Texture is scaled twice its size, so it sould be considered on scrolling
    if (scrollingBack <= -background.width) scrollingBack = 0;
    if (scrollingMid1 <= -midground1.width) scrollingMid1 = 0;
    if (scrollingMid2 <= -midground2.width) scrollingMid2 = 0;
    if (scrollingFore <= -foreground.width) scrollingFore = 0;
}

static void ParalaxBGDraw() {
    bg1  = { scrollingBack, 0 };
    bg2 = { background.width + scrollingBack, 0 };
    mg11 = { scrollingMid1, 0 };
    mg12 = { midground1.width + scrollingMid1, 0 };
    mg21 = { scrollingMid2, 0 };
    mg22 = { midground2.width + scrollingMid2, 0 };
    fg1 = { scrollingFore, 0 };
    fg2 = { foreground.width  + scrollingFore, 0 };
    

    DrawTexture(background, bg1.x, bg1.y,  WHITE);
    DrawTexture(background, bg2.x, bg2.y, WHITE);

    // Draw midground image twice
    DrawTexture(midground1, mg11.x, mg11.y, WHITE);
    DrawTexture(midground1, mg12.x, mg12.y, WHITE);

    DrawTexture(midground2, mg21.x, mg21.y, WHITE);
    DrawTexture(midground2, mg22.x, mg22.y,WHITE);

    // Draw foreground image twice
    DrawTexture(foreground, fg1.x, fg1.y, WHITE);
    DrawTexture(foreground, fg2.x, fg2.y, WHITE);
}