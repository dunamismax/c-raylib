/**
 * @file main.c
 * @brief A simple Flappy Bird-style game.
 * 
 * This game is a simple clone of the popular mobile game Flappy Bird.
 * The player controls a bird and must navigate it through a series of pipes.
 * The game is over if the bird hits a pipe or the ground.
 * 
 */

#include "raylib.h"
#include "corelib.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief The current state of the game.
 * 
 */
typedef enum {
    READY,      /**< The game is ready to be played. */
    PLAYING,    /**< The game is currently being played. */
    GAME_OVER   /**< The game is over. */
} GameState;

/**
 * @brief A struct that represents the bird.
 * 
 */
typedef struct {
    Vector2 position;       /**< The bird's position. */
    Vector2 velocity;       /**< The bird's velocity. */
    float radius;           /**< The bird's radius. */
    Animation animation;    /**< The bird's animation. */
} Bird;

/**
 * @brief A struct that represents a pipe.
 * 
 */
typedef struct {
    Rectangle top;      /**< The top pipe. */
    Rectangle bottom;   /**< The bottom pipe. */
    bool scored;        /**< Whether the player has scored a point for this pipe. */
} Pipe;

/**
 * @brief A struct that manages the pipes.
 * 
 */
typedef struct {
    Pipe pipes[4];      /**< The pipes. */
    int pipeCount;      /**< The number of pipes. */
    float pipeTimer;    /**< A timer that is used to spawn new pipes. */
} PipeManager;

/**
 * @brief A struct that represents the game's state.
 * 
 */
typedef struct {
    Bird bird;                  /**< The bird. */
    PipeManager pipeManager;    /**< The pipe manager. */
    GameState gameState;        /**< The current game state. */
    int score;                  /**< The player's score. */
    int highScore;              /**< The player's high score. */
    Texture2D birdTexture;      /**< The bird's texture. */
    Sound flapSound;            /**< The sound played when the bird flaps. */
    Sound hitSound;             /**< The sound played when the bird hits something. */
    Texture2D pipeTexture;      /**< The pipe's texture. */
} Game;

void InitGame(Game *game);
void UpdateGame(Game *game);
void DrawGame(Game *game);

bool CheckCollision(Bird bird, Pipe pipe);

/**
 * @brief The main entry point for the game.
 * 
 * @return int The exit code.
 */
int main(void) {
    // Initialize the window and the game.
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "FOSS Flapper");
    SetTargetFPS(60);
    
    // Create the game.
    Game game = {0};
    
    // Load the bird's texture and sounds.
    game.birdTexture = LoadTexture("assets/foss_flapper/textures/bird.png");
    game.pipeTexture = LoadTexture("assets/foss_flapper/textures/pipe.png");

    InitAudioDevice();
    game.flapSound = LoadSound("assets/foss_flapper/audio/flap.wav");
    game.hitSound = LoadSound("assets/foss_flapper/audio/hit.wav");
    
    // Initialize the game.
    InitGame(&game);
    
    // Main game loop.
    while (!WindowShouldClose()) {
        // Update and draw the game.
        UpdateGame(&game);
        DrawGame(&game);
    }
    
    // Unload the bird's texture and close the window.
    UnloadTexture(game.birdTexture);
    UnloadTexture(game.pipeTexture);
    UnloadSound(game.flapSound);
    UnloadSound(game.hitSound);
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}

/**
 * @brief Initializes the game.
 * 
 * @param game A pointer to the game.
 */
void InitGame(Game *game) {
    // Initialize the bird.
    game->bird.position = (Vector2){ SCREEN_WIDTH / 4.0f, SCREEN_HEIGHT / 2.0f };
    game->bird.velocity = (Vector2){ 0, 0 };
    game->bird.radius = BIRD_RADIUS;
    
    // Initialize the bird's animation.
    Rectangle birdFrame = { 0, 0, 32, 32 };
    game->bird.animation = CreateAnimation(game->birdTexture, &birdFrame, 1, 0.1f, true);
    
    // Initialize the pipe manager.
    game->pipeManager.pipeCount = 4;
    game->pipeManager.pipeTimer = 0.0f;
    
    // Initialize the pipes.
    for (int i = 0; i < game->pipeManager.pipeCount; i++) {
        float gapY = GetRandomValue(100, SCREEN_HEIGHT - PIPE_GAP - 100);
        game->pipeManager.pipes[i].top = (Rectangle){ 
            SCREEN_WIDTH + i * 200, 
            0, 
            PIPE_WIDTH, 
            gapY 
        };
        game->pipeManager.pipes[i].bottom = (Rectangle){ 
            SCREEN_WIDTH + i * 200, 
            gapY + PIPE_GAP, 
            PIPE_WIDTH, 
            SCREEN_HEIGHT - (gapY + PIPE_GAP) 
        };
        game->pipeManager.pipes[i].scored = false;
    }
    
    // Initialize the score and game state.
    game->score = 0;
    game->gameState = READY;
}

/**
 * @brief Updates the game.
 * 
 * @param game A pointer to the game.
 */
void UpdateGame(Game *game) {
    // Get the time since the last frame.
    float deltaTime = GetFrameTime();
    
    // If the game is ready, wait for the player to start the game.
    if (game->gameState == READY) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_SPACE)) {
            game->gameState = PLAYING;
            game->bird.velocity.y = JUMP_FORCE;
        }
        return;
    }
    
    // If the game is over, wait for the player to restart the game.
    if (game->gameState == GAME_OVER) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_SPACE)) {
            InitGame(game);
        }
        return;
    }
    
    // If the player jumps, apply an upward force to the bird.
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_SPACE)) {
        game->bird.velocity.y = JUMP_FORCE;
        PlaySound(game->flapSound);
    }
    
    // Apply gravity to the bird.
    game->bird.velocity.y += GRAVITY * deltaTime;
    game->bird.position.y += game->bird.velocity.y * deltaTime;
    
    // Update the bird's animation.
    UpdateAnimation(&game->bird.animation, deltaTime);
    
    // If the bird hits the top or bottom of the screen, the game is over.
    if (game->bird.position.y <= game->bird.radius || game->bird.position.y >= SCREEN_HEIGHT - game->bird.radius) {
        game->gameState = GAME_OVER;
        PlaySound(game->hitSound);
        if (game->score > game->highScore) game->highScore = game->score;
        return;
    }
    
    // Update the pipes.
    for (int i = 0; i < game->pipeManager.pipeCount; i++) {
        // Move the pipes to the left.
        game->pipeManager.pipes[i].top.x -= PIPE_SPEED * deltaTime;
        game->pipeManager.pipes[i].bottom.x -= PIPE_SPEED * deltaTime;
        
        // If a pipe is off the screen, reset it.
        if (game->pipeManager.pipes[i].top.x + PIPE_WIDTH < 0) {
            float gapY = GetRandomValue(100, SCREEN_HEIGHT - PIPE_GAP - 100);
            game->pipeManager.pipes[i].top.x = SCREEN_WIDTH;
            game->pipeManager.pipes[i].bottom.x = SCREEN_WIDTH;
            game->pipeManager.pipes[i].top.y = 0;
            game->pipeManager.pipes[i].top.height = gapY;
            game->pipeManager.pipes[i].bottom.y = gapY + PIPE_GAP;
            game->pipeManager.pipes[i].bottom.height = SCREEN_HEIGHT - (gapY + PIPE_GAP);
            game->pipeManager.pipes[i].scored = false;
        }
        
        // If the bird collides with a pipe, the game is over.
        if (CheckCollision(game->bird, game->pipeManager.pipes[i])) {
            game->gameState = GAME_OVER;
            PlaySound(game->hitSound);
            if (game->score > game->highScore) game->highScore = game->score;
            return;
        }
        
        // If the bird passes a pipe, increment the score.
        if (!game->pipeManager.pipes[i].scored && 
            game->bird.position.x > game->pipeManager.pipes[i].top.x + PIPE_WIDTH) {
            game->score++;
            game->pipeManager.pipes[i].scored = true;
        }
    }
}

/**
 * @brief Draws the game.
 * 
 * @param game A pointer to the game.
 */
void DrawGame(Game *game) {
    // Begin drawing.
    BeginDrawing();
    
    // Clear the background.
    ClearBackground(SKYBLUE);
    
    // Draw the pipes.
    for (int i = 0; i < game->pipeManager.pipeCount; i++) {
        DrawTextureRec(game->pipeTexture, game->pipeManager.pipes[i].top, (Vector2){game->pipeManager.pipes[i].top.x, game->pipeManager.pipes[i].top.y}, WHITE);
        DrawTextureRec(game->pipeTexture, game->pipeManager.pipes[i].bottom, (Vector2){game->pipeManager.pipes[i].bottom.x, game->pipeManager.pipes[i].bottom.y}, WHITE);
    }
    
    // Draw the bird.
    DrawTextureRec(game->birdTexture, (Rectangle){0, 0, game->birdTexture.width, game->birdTexture.height}, game->bird.position, WHITE);
    
    // Draw the score.
    DrawText(TextFormat("Score: %d", game->score), 10, 10, 30, BLACK);
    DrawText(TextFormat("High: %d", game->highScore), 10, 50, 20, DARKGRAY);
    
    // If the game is ready, draw the title screen.
    if (game->gameState == READY) {
        DrawText("FOSS FLAPPER", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 - 100, 30, BLACK);
        DrawText("Click or Press SPACE to start", SCREEN_WIDTH/2 - 140, SCREEN_HEIGHT/2 - 50, 20, DARKGRAY);
    } else if (game->gameState == GAME_OVER) {
        // If the game is over, draw the game over screen.
        DrawText("GAME OVER", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 50, 30, RED);
        DrawText(TextFormat("Final Score: %d", game->score), SCREEN_WIDTH/2 - 70, SCREEN_HEIGHT/2, 20, BLACK);
        DrawText("Click or Press SPACE to restart", SCREEN_WIDTH/2 - 130, SCREEN_HEIGHT/2 + 30, 20, DARKGRAY);
    }
    
    // End drawing.
    EndDrawing();
}

/**
 * @brief Checks for a collision between the bird and a pipe.
 * 
 * @param bird The bird.
 * @param pipe The pipe.
 * @return true if there is a collision, false otherwise.
 */
bool CheckCollision(Bird bird, Pipe pipe) {
    // Create a rectangle for the bird.
    Rectangle birdRect = {
        bird.position.x - bird.radius,
        bird.position.y - bird.radius,
        bird.radius * 2,
        bird.radius * 2
    };

    // Check for a collision between the bird and the top and bottom pipes.
    return CheckCollisionRecs(birdRect, pipe.top) || CheckCollisionRecs(birdRect, pipe.bottom);
}
