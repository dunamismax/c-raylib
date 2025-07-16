#include "corelib.h"
#include <stdlib.h>

Animation CreateAnimation(Texture2D spritesheet, Rectangle* frames, int frameCount, float frameDuration, bool loop) {
    Animation anim = {0};
    anim.spritesheet = spritesheet;
    anim.frameCount = frameCount;
    anim.currentFrame = 0;
    anim.frameTimer = 0.0f;
    anim.loop = loop;
    
    anim.frames = (AnimationFrame*)malloc(sizeof(AnimationFrame) * frameCount);
    for (int i = 0; i < frameCount; i++) {
        anim.frames[i].source = frames[i];
        anim.frames[i].duration = frameDuration;
    }
    
    return anim;
}

void UpdateAnimation(Animation* anim, float deltaTime) {
    if (anim->frameCount <= 1) return;
    
    anim->frameTimer += deltaTime;
    
    if (anim->frameTimer >= anim->frames[anim->currentFrame].duration) {
        anim->frameTimer = 0.0f;
        anim->currentFrame++;
        
        if (anim->currentFrame >= anim->frameCount) {
            if (anim->loop) {
                anim->currentFrame = 0;
            } else {
                anim->currentFrame = anim->frameCount - 1;
            }
        }
    }
}

void DrawAnimation(Animation anim, Vector2 position, float rotation) {
    if (anim.frameCount > 0 && anim.currentFrame < anim.frameCount) {
        Rectangle source = anim.frames[anim.currentFrame].source;
        Rectangle dest = { position.x, position.y, source.width, source.height };
        Vector2 origin = { source.width / 2.0f, source.height / 2.0f };
        
        DrawTexturePro(anim.spritesheet, source, dest, origin, rotation, WHITE);
    }
}