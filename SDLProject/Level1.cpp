#include "Scene.h"

void LoadLevel1(Scene* scene) {
    scene->obstacleCount = 1;
    scene->platformCount = 11;  

    delete[] scene->platforms;
    delete[] scene->obstacles;
    delete scene->ship;
    delete scene->map;

    scene->platforms = new Entity[scene->platformCount];
    for (int i = 0; i < scene->platformCount; ++i) {
        scene->platforms[i].set_texture_id(scene->mapTextureID);
        scene->platforms[i].set_position(glm::vec3(i * 1.0f - 5.0f, -2.8f, 0.0f));
        scene->platforms[i].set_scale(glm::vec3(1.0f, 0.5f, 1.0f));
        scene->platforms[i].set_width(1.0f);
        scene->platforms[i].set_height(0.5f);
    }

    scene->obstacles = new Entity[scene->obstacleCount];
    for (int i = 0; i < scene->obstacleCount; ++i) {
        scene->obstacles[i].set_texture_id(scene->obstacleTextureID);
        scene->obstacles[i].set_position(glm::vec3(-2.0f + i * 1.8f, -2.3f, 0.0f));
        scene->obstacles[i].set_scale(glm::vec3(0.3f));
        scene->obstacles[i].set_width(0.3f);
        scene->obstacles[i].set_height(0.3f);
    }

    scene->aiDirections.clear();
    for (int i = 0; i < scene->obstacleCount; ++i) {
        scene->aiDirections.push_back(1.0f);  // start moving right
    }

    scene->ship = new Entity();
    scene->ship->set_texture_id(scene->shipTextureID);
    scene->ship->set_position(glm::vec3(-4.5f, 3.0f, 0.0f));
    scene->ship->set_scale(glm::vec3(0.3f));
    scene->ship->set_width(0.6f);
    scene->ship->set_height(0.6f);

    scene->map = new Entity();
    scene->map->set_texture_id(scene->mapTextureID);
    scene->viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
}
