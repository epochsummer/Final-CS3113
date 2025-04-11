#define STB_IMAGE_IMPLEMENTATION
#include "Scene.h"
#include "stb_image.h"
#include <iostream>
#include <cassert>
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"


const float gravityForce = -9.8f;
const float jumpVelocity = 4.5f;
const float hitCooldownDuration = 1.0f;

Scene::Scene()
    : ship(nullptr), map(nullptr), platforms(nullptr), obstacles(nullptr),
      shipTextureID(0), mapTextureID(0), fontTextureID(0), obstacleTextureID(0),
      previousTicks(0.0f), timeAccumulator(0.0f),
      isRunning(true), succeed(false), failed(false),
      isMovingH(false), isMovingV(false), justJumped(false),
      obstacleHitCount(0), hasLanded(true), mode(MENU_MODE),
      obstacleCount(2), lives(3), level(1), timeSinceLastHit(hitCooldownDuration),
      showFailureMessage(false) {}

Scene::~Scene() {
    delete[] platforms;
    if (obstacles != nullptr) {
        delete[] obstacles;
        obstacles = nullptr;
    }
    delete ship;
    delete map;
    SDL_Quit();
}

GLuint Scene::LoadTexture(const char* filepath) {
    int w, h, comp;
    unsigned char* img = stbi_load(filepath, &w, &h, &comp, STBI_rgb_alpha);
    if (!img) {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
        exit(1);
    }
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(img);
    return texID;
}

void Scene::Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 800, SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, context);

    shaderProgram.load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    shaderProgram.set_projection_matrix(projectionMatrix);
    shaderProgram.set_view_matrix(viewMatrix);
    glUseProgram(shaderProgram.get_program_id());

    shipTextureID = LoadTexture("/Users/epochsum/Desktop/triangle/assets/hello.png");
    mapTextureID = LoadTexture("/Users/epochsum/Desktop/triangle/assets/ground.jpg");
    fontTextureID = LoadTexture("/Users/epochsum/Desktop/triangle/assets/font.png");
    obstacleTextureID = LoadTexture("/Users/epochsum/Desktop/triangle/assets/target.jpg");
    platformCount = 10;
    LoadLevel(level);
    glClearColor(0.96f, 0.96f, 0.96f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Scene::LoadLevel(int levelNum) {
    if (levelNum == 1) LoadLevel1(this);
    else if (levelNum == 2) LoadLevel2(this);
    else if (levelNum == 3) LoadLevel3(this);
}
void Scene::ProcessInput(SDL_Event& event) {
    if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) StopRunning();
    if (event.type == SDL_KEYDOWN && mode == MENU_MODE && event.key.keysym.sym == SDLK_RETURN) {
        mode = GAMEPLAY_MODE;
        showFailureMessage = false;
    }

    if (event.type == SDL_KEYDOWN && lives <= 0 && event.key.keysym.sym == SDLK_r) {
        level = 1;
        lives = 3;
        showFailureMessage = false;
        LoadLevel(level);
    }

    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    if (mode == GAMEPLAY_MODE && !showFailureMessage) {
        if (keyState[SDL_SCANCODE_UP] && hasLanded && !justJumped) {
            ship->set_velocity(glm::vec3(ship->get_velocity().x, jumpVelocity, 0.0f));
            hasLanded = false;
            justJumped = true;
        }
        if (!keyState[SDL_SCANCODE_UP]) justJumped = false;
        HandleGameplayInput(keyState);
    }
}

void Scene::HandleGameplayInput(const Uint8* keyState) {
    isMovingH = false;
    glm::vec3 velocity = ship->get_velocity();

    if (keyState[SDL_SCANCODE_LEFT]) {
        velocity.x = -2.0f;
        isMovingH = true;
    } else if (keyState[SDL_SCANCODE_RIGHT]) {
        velocity.x = 2.0f;
        isMovingH = true;
    } else {
        velocity.x = 0.0f;
    }
    ship->set_velocity(velocity);
}

bool Scene::IsRunning() const { return isRunning; }
void Scene::StopRunning() { isRunning = false; }

void Scene::Update(float deltaTime) {
    if (mode == MENU_MODE || showFailureMessage) return;

    timeSinceLastHit += deltaTime;
    deltaTime += timeAccumulator;
    if (deltaTime < fixedTimestep) {
        timeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= fixedTimestep) {
        glm::vec3 accel(0.0f, gravityForce, 0.0f);
        ship->set_acceleration(accel);

        hasLanded = false;
        for (int i = 0; i < platformCount; i++) {
            if (ship->check_collision(&platforms[i])) {
                hasLanded = true;
                glm::vec3 velocity = ship->get_velocity();
                if (velocity.y < 0.0f) velocity.y = 0.0f;
                ship->set_velocity(glm::vec3(velocity.x, velocity.y, 0.0f));

                float top = platforms[i].get_position().y + platforms[i].get_height() / 2.0f;
                float h = ship->get_height() / 2.0f;
                ship->set_position(glm::vec3(ship->get_position().x, top + h, 0.0f));
                break;
            }
        }

        bool hitThisFrame = false;
        for (int i = 0; i < obstacleCount; i++) {
            if (ship->check_collision(&obstacles[i])) {
                hitThisFrame = true;
                break;
            }
        }

        if (hitThisFrame && timeSinceLastHit >= hitCooldownDuration) {
            lives--;
            timeSinceLastHit = 0.0f;
            std::cout << "Hit! Lives left: " << lives << std::endl;
            if (lives <= 0) {
                showFailureMessage = true;
                ship->set_velocity(glm::vec3(0.0f));
                ship->set_acceleration(glm::vec3(0.0f));
            }
        }

        ship->update(fixedTimestep, platforms, platformCount, isMovingH, false);
        for (int i = 0; i < platformCount; i++) platforms[i].update(fixedTimestep);
        for (int i = 0; i < obstacleCount; i++) obstacles[i].update(fixedTimestep);

        deltaTime -= fixedTimestep;
    }
    if (ship->get_position().x >= 4.7f && level < 3) {
        std::cout << "Loading level " << level << std::endl;
        level++;
        LoadLevel(level);
    } else if (ship->get_position().x >= 4.7f && level == 3) {
        showWinMessage = true;
    }
    ship->set_scale(glm::vec3(0.3f));
}

void Scene::Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    shaderProgram.set_view_matrix(viewMatrix);

    if (mode == MENU_MODE) {
        DrawText(&shaderProgram, fontTextureID, "LUNAR LANDER", 0.5f, 0.05f, glm::vec3(-3.0f, 1.0f, 0));
        DrawText(&shaderProgram, fontTextureID, "Press Enter to Start", 0.3f, 0.05f, glm::vec3(-3.0f, -1.0f, 0));
        SDL_GL_SwapWindow(window);
        return;
    }

    ship->render(&shaderProgram);
    for (int i = 0; i < platformCount; i++) platforms[i].render(&shaderProgram);
    for (int i = 0; i < obstacleCount; i++) obstacles[i].render(&shaderProgram);
    DrawText(&shaderProgram, fontTextureID, "LIVES: " + std::to_string(lives), 0.2f, 0.01f, glm::vec3(-4.5f, 3.4f, 0));

    if (showFailureMessage) {
        DrawText(&shaderProgram, fontTextureID, "YOU HAVE FAILED", 0.5f, 0.05f, glm::vec3(-3.5f, 0.0f, 0));
        DrawText(&shaderProgram, fontTextureID, "Press R to Restart", 0.3f, 0.05f, glm::vec3(-3.2f, -1.0f, 0));
    }
    else if (showWinMessage) {
        DrawText(&shaderProgram, fontTextureID, "CONGRATS YOU WIN!", 0.3f, 0.05f, glm::vec3(-3.2f, 0.0f, 0));
    }


    SDL_GL_SwapWindow(window);
}

void Scene::DrawText(ShaderProgram* program, GLuint textureID, std::string text, float size, float spacing, glm::vec3 position) {
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    std::vector<float> vertices;
    std::vector<float> texCoords;

    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;

        vertices.insert(vertices.end(), {
            offset + (-0.5f * size),  0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size),   0.5f * size,
            offset + (0.5f * size),  -0.5f * size,
            offset + (0.5f * size),   0.5f * size,
            offset + (-0.5f * size), -0.5f * size
        });

        texCoords.insert(texCoords.end(), {
            u,         v,
            u,         v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u,         v + height
        });
    }


    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
    program->set_model_matrix(modelMatrix);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(text.size() * 6));

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
} 

