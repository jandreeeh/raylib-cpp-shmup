#include <raylib.h>

static int framesCounter = 0;

void AnimationUpdate(Texture2D atlas, Rectangle& rec, int& currentFrame, int FPS, int numFrames){
    framesCounter++;
    if(framesCounter >= (60/FPS)){
        framesCounter = 0;
        currentFrame++;

        if (currentFrame > numFrames-1) currentFrame = 0;
        rec.x = (float)currentFrame*(float)atlas.width/numFrames; 
    }

}