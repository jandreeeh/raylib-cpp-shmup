#pragma once
#include <raylib.h>


void AnimationUpdate(Texture2D atlas, Rectangle& rec, int& currentFrame, int FPS, int numFrames);

//Struct for animation atlas
typedef struct SpriteAnimate{
    Texture2D atlas;
    Rectangle source;
    Vector2 position;
    int numFrames;
    int framesPerSecond;
    int currentFrame;
}SpriteAnimate;