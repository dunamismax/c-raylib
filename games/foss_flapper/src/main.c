#include "raylib.h"
#include "corelib.h"
#include <stdlib.h>
#include <stdio.h>

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 768
#define GRAVITY 980.0f
#define JUMP_FORCE -400.0f
#define PIPE_WIDTH 80
#define PIPE_GAP 200
#define PIPE_SPEED 200.0f
#define BIRD_RADIUS 16.0f

typedef enum {
    READY,
    PLAYING,
    GAME_OVER
} GameState;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float radius;
    Animation animation;
} Bird;

typedef struct {
    Rectangle top;
    Rectangle bottom;
    bool scored;
} Pipe;

typedef struct {
    Pipe pipes[4];
    int pipeCount;
    float pipeTimer;
} PipeManager;

Bird bird = {0};
PipeManager pipeManager = {0};
GameState gameState = READY;
int score = 0;
int highScore = 0;
Texture2D birdTexture = {0};

void InitGame(void) {
    bird.position = (Vector2){ SCREEN_WIDTH / 4.0f, SCREEN_HEIGHT / 2.0f };
    bird.velocity = (Vector2){ 0, 0 };
    bird.radius = BIRD_RADIUS;
    
    Rectangle birdFrame = { 0, 0, 32, 32 };
    bird.animation = CreateAnimation(birdTexture, &birdFrame, 1, 0.1f, true);
    
    pipeManager.pipeCount = 4;
    pipeManager.pipeTimer = 0.0f;
    
    for (int i = 0; i < pipeManager.pipeCount; i++) {
        float gapY = GetRandomValue(100, SCREEN_HEIGHT - PIPE_GAP - 100);
        pipeManager.pipes[i].top = (Rectangle){ 
            SCREEN_WIDTH + i * 200, 
            0, 
            PIPE_WIDTH, 
            gapY 
        };
        pipeManager.pipes[i].bottom = (Rectangle){ 
            SCREEN_WIDTH + i * 200, 
            gapY + PIPE_GAP, 
            PIPE_WIDTH, 
            SCREEN_HEIGHT - (gapY + PIPE_GAP) 
        };
        pipeManager.pipes[i].scored = false;
    }
    
    score = 0;
    gameState = READY;
}

bool CheckCollision(Vector2 center, float radius, Rectangle rect) {
    Vector2 closest = {
        (center.x < rect.x) ? rect.x : (center.x > rect.x + rect.width) ? rect.x + rect.width : center.x,
        (center.y < rect.y) ? rect.y : (center.y > rect.y + rect.height) ? rect.y + rect.height : center.y
    };
    
    float distance = sqrtf((center.x - closest.x) * (center.x - closest.x) + 
                          (center.y - closest.y) * (center.y - closest.y));
    
    return distance <= radius;
}

void UpdateGame(void) {
    float deltaTime = GetFrameTime();
    
    if (gameState == READY) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_SPACE)) {
            gameState = PLAYING;
            bird.velocity.y = JUMP_FORCE;
        }
        return;
    }
    
    if (gameState == GAME_OVER) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_SPACE)) {
            InitGame();
        }
        return;
    }
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_SPACE)) {
        bird.velocity.y = JUMP_FORCE;
    }
    
    bird.velocity.y += GRAVITY * deltaTime;
    bird.position.y += bird.velocity.y * deltaTime;
    
    UpdateAnimation(&bird.animation, deltaTime);
    
    if (bird.position.y <= bird.radius || bird.position.y >= SCREEN_HEIGHT - bird.radius) {
        gameState = GAME_OVER;
        if (score > highScore) highScore = score;
        return;
    }
    
    for (int i = 0; i < pipeManager.pipeCount; i++) {
        pipeManager.pipes[i].top.x -= PIPE_SPEED * deltaTime;
        pipeManager.pipes[i].bottom.x -= PIPE_SPEED * deltaTime;
        
        if (pipeManager.pipes[i].top.x + PIPE_WIDTH < 0) {
            float gapY = GetRandomValue(100, SCREEN_HEIGHT - PIPE_GAP - 100);
            pipeManager.pipes[i].top.x = SCREEN_WIDTH;
            pipeManager.pipes[i].bottom.x = SCREEN_WIDTH;
            pipeManager.pipes[i].top.y = 0;
            pipeManager.pipes[i].top.height = gapY;
            pipeManager.pipes[i].bottom.y = gapY + PIPE_GAP;
            pipeManager.pipes[i].bottom.height = SCREEN_HEIGHT - (gapY + PIPE_GAP);
            pipeManager.pipes[i].scored = false;
        }
        
        if (CheckCollision(bird.position, bird.radius, pipeManager.pipes[i].top) ||
            CheckCollision(bird.position, bird.radius, pipeManager.pipes[i].bottom)) {
            gameState = GAME_OVER;
            if (score > highScore) highScore = score;
            return;
        }
        
        if (!pipeManager.pipes[i].scored && 
            bird.position.x > pipeManager.pipes[i].top.x + PIPE_WIDTH) {
            score++;
            pipeManager.pipes[i].scored = true;
        }
    }
}

void DrawGame(void) {
    BeginDrawing();
    ClearBackground(SKYBLUE);
    
    for (int i = 0; i < pipeManager.pipeCount; i++) {
        DrawRectangleRec(pipeManager.pipes[i].top, GREEN);
        DrawRectangleRec(pipeManager.pipes[i].bottom, GREEN);
    }
    
    DrawCircleV(bird.position, bird.radius, YELLOW);
    
    DrawText(TextFormat("Score: %d", score), 10, 10, 30, BLACK);
    DrawText(TextFormat("High: %d", highScore), 10, 50, 20, DARKGRAY);
    
    if (gameState == READY) {
        DrawText("FOSS FLAPPER", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 - 100, 30, BLACK);
        DrawText("Click or Press SPACE to start", SCREEN_WIDTH/2 - 140, SCREEN_HEIGHT/2 - 50, 20, DARKGRAY);
    } else if (gameState == GAME_OVER) {
        DrawText("GAME OVER", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 50, 30, RED);
        DrawText(TextFormat("Final Score: %d", score), SCREEN_WIDTH/2 - 70, SCREEN_HEIGHT/2, 20, BLACK);
        DrawText("Click or Press SPACE to restart", SCREEN_WIDTH/2 - 130, SCREEN_HEIGHT/2 + 30, 20, DARKGRAY);
    }
    
    EndDrawing();
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "FOSS Flapper");
    SetTargetFPS(60);
    
    Image birdImage = GenImageColor(32, 32, YELLOW);
    birdTexture = LoadTextureFromImage(birdImage);
    UnloadImage(birdImage);
    
    InitGame();
    
    while (!WindowShouldClose()) {
        UpdateGame();
        DrawGame();
    }
    
    UnloadTexture(birdTexture);
    CloseWindow();
    
    return 0;
}