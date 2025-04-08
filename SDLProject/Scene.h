#pragma once

#include "ShaderProgram.h"
#include "Entity.h"
#include <SDL.h>
#include <vector>
#include <string>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum SceneMode {
    MENU_MODE,
    GAMEPLAY_MODE
};

class Scene {
public:
    Scene();
    ~Scene();

    void Initialize();
    void ProcessInput(SDL_Event& event);
    void Update(float deltaTime);
    void Render();
    bool IsRunning() const;
    void StopRunning();

private:
    SceneMode mode;
    SDL_Window* window;
    SDL_GLContext context;

    ShaderProgram shaderProgram;

    GLuint shipTextureID;
    GLuint mapTextureID;
    GLuint targetTextureID;
    GLuint fontTextureID;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    Entity* ship;
    Entity* map;
    Entity* target;
    Entity* platforms;

    float previousTicks;
    float timeAccumulator;

    bool isRunning;
    bool succeed;
    bool failed;
    bool isMovingH;
    bool isMovingV;

    const float gravity = -1.5f;
    const float fixedTimestep = 1.0f / 60.0f;
    const int platformCount = 5;

    GLuint LoadTexture(const char* filepath);
    void HandleGameplayInput(const Uint8* keyState);
    void DrawText(ShaderProgram* program, GLuint textureID, std::string text, float size, float spacing, glm::vec3 position);
};
