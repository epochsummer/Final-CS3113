#define STB_IMAGE_IMPLEMENTATION
#include "Scene.h"
#include "stb_image.h"
#include <iostream>
#include <cassert>

#define FONTBANK_SIZE 16

Scene::Scene() :
    ship(nullptr), map(nullptr), target(nullptr), platforms(nullptr),
    shipTextureID(0), mapTextureID(0), targetTextureID(0), fontTextureID(0),
    previousTicks(0.0f), timeAccumulator(0.0f),
    isRunning(true), succeed(false), failed(false),
    isMovingH(false), isMovingV(false), mode(MENU_MODE) {}

Scene::~Scene() {
    delete[] platforms;
    delete ship;
    delete map;
    delete target;
    SDL_Quit();
}

void Scene::Initialize() {
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Lunar Lander",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1000, 800, SDL_WINDOW_OPENGL);

    context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, context);

    shaderProgram.load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    shaderProgram.set_projection_matrix(projectionMatrix);
    shaderProgram.set_view_matrix(viewMatrix);

    glUseProgram(shaderProgram.get_program_id());

    shipTextureID = LoadTexture("/Users/epochsum/Desktop/triangle/assets/lunar.jpg");
    mapTextureID = LoadTexture("/Users/epochsum/Desktop/triangle/assets/moonground.jpeg");
    targetTextureID = LoadTexture("/Users/epochsum/Desktop/triangle/assets/target.png");
    fontTextureID = LoadTexture("/Users/epochsum/Desktop/triangle/assets/font.png");

    int frames[4][2] = {{0,4}, {1,5}, {2,6}, {3,7}};
    ship = new Entity(shipTextureID, 1.0f, frames, 0.0f, 4, 0, 4, 2);
    ship->set_position(glm::vec3(0.0f, 3.0f, 0.0f));
    ship->set_acceleration(glm::vec3(0.0f, gravity, 0.0f));
    ship->face_down();

    map = new Entity(mapTextureID, 1.0f);
    target = new Entity(targetTextureID, 0);
    platforms = new Entity[platformCount];

    for (int i = 0; i < platformCount; i++) {
        platforms[i].set_texture_id(mapTextureID);
        platforms[i].set_position(glm::vec3(i - 1.0f, -2.8f, 0.0f));
        platforms[i].set_scale(glm::vec3(10.0f, 0.5f, 1.0f));
        platforms[i].update(0.0f);
    }

    glClearColor(0.96f, 0.96f, 0.96f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Scene::ProcessInput(SDL_Event& event) {
    if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) StopRunning();

    if (event.type == SDL_KEYDOWN && mode == MENU_MODE) {
        if (event.key.keysym.sym == SDLK_RETURN) {
            mode = GAMEPLAY_MODE;
        }
    }

    const Uint8* keyState = SDL_GetKeyboardState(NULL);

    if (mode == GAMEPLAY_MODE) {
        HandleGameplayInput(keyState);
    }
}

void Scene::HandleGameplayInput(const Uint8* keyState) {
    ship->set_movement(glm::vec3(0.0f));

    if (keyState[SDL_SCANCODE_LEFT]) {
        ship->set_acceleration(glm::vec3(-0.2f, ship->get_acceleration().y, 0.0f));
        isMovingH = true;
    } else if (keyState[SDL_SCANCODE_RIGHT]) {
        ship->set_acceleration(glm::vec3(0.2f, ship->get_acceleration().y, 0.0f));
        isMovingH = true;
    } else {
        ship->set_acceleration(glm::vec3(0.0f, ship->get_acceleration().y, 0.0f));
        isMovingH = false;
    }

    if (keyState[SDL_SCANCODE_UP]) {
        ship->move_up();
        isMovingV = true;
    } else if (keyState[SDL_SCANCODE_DOWN]) {
        ship->move_down();
        isMovingV = true;
    } else {
        isMovingV = false;
    }

    if (glm::length(ship->get_movement()) > 1.0f) {
        ship->normalise_movement();
    }
}

void Scene::Update(float deltaTime) {
    if (mode == MENU_MODE) return;

    deltaTime += timeAccumulator;
    if (deltaTime < fixedTimestep) {
        timeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= fixedTimestep) {
        if (ship->get_fuel() > 0 && (isMovingH || isMovingV)) {
            ship->reduce_fuel(0.1f);
        } else {
            ship->set_acceleration(glm::vec3(0.0f));
        }

        ship->update(fixedTimestep, platforms, platformCount, isMovingH, isMovingV);
        target->update(fixedTimestep);
        for (int i = 0; i < platformCount; i++) platforms[i].update(fixedTimestep);

        deltaTime -= fixedTimestep;
    }

    if (ship->check_collision(target)) {
        ship->set_velocity(glm::vec3(0.0f));
        succeed = true;
    }

    bool landed = false;
    for (int i = 0; i < platformCount; i++) {
        if (ship->check_collision(&platforms[i])) landed = true;
    }

    if (ship->get_position().y <= -2.5f && !landed) failed = true;

    if (failed || succeed) {
        ship->set_velocity(glm::vec3(0.0f));
        ship->set_acceleration(glm::vec3(0.0f));
    }

    target->set_position(glm::vec3(-1.0f, -2.4f, 0.0f));
    target->set_scale(glm::vec3(0.8f));
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
    target->render(&shaderProgram);
    for (int i = 0; i < platformCount; i++) platforms[i].render(&shaderProgram);

    DrawText(&shaderProgram, fontTextureID, "FUEL: " + std::to_string((int)ship->get_fuel()), 0.2f, 0.01f, glm::vec3(-4.5f, 3.0f, 0));
    if (succeed) DrawText(&shaderProgram, fontTextureID, "MISSION SUCCESSFUL", 0.3f, 0.01f, glm::vec3(-3.5f, 0.0f, 0));
    if (failed) DrawText(&shaderProgram, fontTextureID, "MISSION FAILED", 0.3f, 0.01f, glm::vec3(-2.5f, 0.0f, 0));

    SDL_GL_SwapWindow(window);
}

void Scene::DrawText(ShaderProgram* program, GLuint textureID, std::string text, float size, float spacing, glm::vec3 position) {
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;
    std::vector<float> vertices;
    std::vector<float> texCoords;

    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v = (float)(index / FONTBANK_SIZE) / FONTBANK_SIZE;

        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size
        });

        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height
        });
    }

    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
    program->set_model_matrix(modelMatrix);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(text.size() * 6));

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

GLuint Scene::LoadTexture(const char* filepath) {
    int w, h, comp;
    unsigned char* img = stbi_load(filepath, &w, &h, &comp, STBI_rgb_alpha);
    if (!img) {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
        assert(false);
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

bool Scene::IsRunning() const { return isRunning; }
void Scene::StopRunning() { isRunning = false; }
