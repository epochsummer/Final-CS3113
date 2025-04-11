#pragma once

#include "ShaderProgram.h"
#include "Entity.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum GameMode { MENU_MODE, GAMEPLAY_MODE };

class Scene {
public:
    Scene();
    ~Scene(); // cleanup happens here

    void Initialize();
    GLuint LoadTexture(const char* filepath);
    void ProcessInput(SDL_Event& event);
    void Render();
    void Update(float deltaTime);
    bool IsRunning() const;
    void StopRunning();

    // Public access to these members for Level1/2/3 loading
    Entity* ship;
    Entity* map;
    Entity* target;
    Entity* platforms;
    Entity* obstacles;

    GLuint shipTextureID;
    GLuint mapTextureID;
    GLuint targetTextureID;
    GLuint fontTextureID;
    GLuint obstacleTextureID;

    int obstacleCount;
    int level;
    bool showFailureMessage;
    bool showWinMessage;
    glm::mat4 viewMatrix;
    int platformCount;
    int obstacleHitCount;
    std::vector<float> aiDirections;

private:
    SDL_Window* window;
    SDL_GLContext context;

    ShaderProgram shaderProgram;
    glm::mat4 projectionMatrix;
    const int maxObstacleHits = 3;
    const int maxLevel = 3;
    int lives;

    float previousTicks;
    float timeAccumulator;
    const float fixedTimestep = 0.0166666f;
    float timeSinceLastHit;

    bool isRunning;
    bool succeed;
    bool failed;
    bool hasLanded;

    bool isMovingH;
    bool isMovingV;
    bool canThrust;
    bool justJumped = false;

    GameMode mode;

    Mix_Music* bgm;
    Mix_Music* winMusic;
    Mix_Music* loseMusic;
    Mix_Music* painMusic;
    Mix_Chunk* jumpSFX;
    Mix_Chunk* painSFX;

    


    void HandleGameplayInput(const Uint8* keyState);
    void DrawText(ShaderProgram* program, GLuint textureID, std::string text, float size, float spacing, glm::vec3 position);
    void LoadLevel(int levelNum);
};
