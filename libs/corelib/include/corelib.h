#ifndef CORELIB_H
#define CORELIB_H

#include "raylib.h"
#include <stdbool.h>

typedef struct {
    Rectangle source;
    float duration;
} AnimationFrame;

typedef struct {
    Texture2D spritesheet;
    AnimationFrame* frames;
    int frameCount;
    int currentFrame;
    float frameTimer;
    bool loop;
} Animation;

Animation CreateAnimation(Texture2D spritesheet, Rectangle* frames, int frameCount, float frameDuration, bool loop);
void UpdateAnimation(Animation* anim, float deltaTime);
void DrawAnimation(Animation anim, Vector2 position, float rotation);

#endif