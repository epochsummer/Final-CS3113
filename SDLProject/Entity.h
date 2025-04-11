#pragma once

#include "ShaderProgram.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum AnimationDirection { UP, RIGHT, LEFT, DOWN };

class Entity {
private:
    // Animation
    int m_walking[4][2];
    int* m_animation_indices = nullptr;
    int m_animation_frames = 0;
    int m_animation_index = 0;
    int m_animation_rows = 0;
    int m_animation_columns = 0;
    float m_animation_time = 0.0f;

    // Transformations
    glm::vec3 m_position = glm::vec3(0.0f);
    glm::vec3 m_movement = glm::vec3(0.0f);
    glm::vec3 m_scale = glm::vec3(1.0f);
    float m_speed = 0.0f;
    glm::mat4 m_model_matrix = glm::mat4(1.0f);

    // Texture
    GLuint m_texture_id = 0;

    // Physics
    glm::vec3 m_velocity = glm::vec3(0.0f);
    glm::vec3 m_acceleration = glm::vec3(0.0f);

    // Collision dimensions
    float m_width = 1.0f;
    float m_height = 1.0f;

    // Fuel and lives
    float m_fuel = 100.0f;
    int m_lives = 3;

public:
    static constexpr int SECONDS_PER_FRAME = 4;

    // Constructors
    Entity();
    Entity(GLuint texture_id, float speed);
    Entity(GLuint texture_id, float speed, int m_walking[4][2], float animation_time,
           int animation_frames, int animation_index, int animation_cols, int animation_rows);
    ~Entity();

    // Updates
    void update(float delta_time);
    void update(float delta_time, Entity* collidable_entities, int collidable_entity_count, bool is_moving_h, bool is_moving_v);

    // Rendering
    void render(ShaderProgram* program);
    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);

    // Collision
    bool check_collision(Entity* other) const;

    // Animation helpers
    void set_walking(int walking[4][2]);
    void normalise_movement() { m_movement = glm::normalize(m_movement); }
    void face_left() { m_animation_indices = m_walking[LEFT]; }
    void face_right() { m_animation_indices = m_walking[RIGHT]; }
    void face_up() { m_animation_indices = m_walking[UP]; }
    void face_down() { m_animation_indices = m_walking[DOWN]; }

    // Movement controls
    void move_left() { m_velocity.x = -1.5f; face_left(); }
    void move_right() { m_velocity.x = 1.5f; face_right(); }
    void move_up();
    void move_down();

    // Fuel
    float get_fuel() const;
    void reduce_fuel(float amount);

    // Lives
    int get_lives() const { return m_lives; }
    void lose_life() { if (m_lives > 0) m_lives--; }
    void reset_lives() { m_lives = 3; }

    // Getters
    glm::vec3 get_position() const { return m_position; }
    glm::vec3 get_movement() const { return m_movement; }
    glm::vec3 get_scale() const { return m_scale; }
    GLuint get_texture_id() const { return m_texture_id; }
    float get_speed() const { return m_speed; }
    glm::vec3 get_acceleration() const { return m_acceleration; }
    glm::vec3 get_velocity() const { return m_velocity; }
    float get_width() const { return m_width; }
    float get_height() const { return m_height; }

    // Setters
    void set_position(glm::vec3 pos) { m_position = pos; }
    void set_movement(glm::vec3 move) { m_movement = move; }
    void set_scale(glm::vec3 scale) { m_scale = scale; }
    void set_texture_id(GLuint id) { m_texture_id = id; }
    void set_velocity(glm::vec3 vel) { m_velocity = vel; }
    void set_acceleration(glm::vec3 acc) { m_acceleration = acc; }
    void set_width(float w) { m_width = w; }
    void set_height(float h) { m_height = h; }
    void set_speed(float s) { m_speed = s; }
    void set_animation_cols(int c) { m_animation_columns = c; }
    void set_animation_rows(int r) { m_animation_rows = r; }
    void set_animation_frames(int f) { m_animation_frames = f; }
    void set_animation_index(int i) { m_animation_index = i; }
    void set_animation_time(float t) { m_animation_time = t; }
};
