#define GL_SILENCE_DEPRECATION
//#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include <vector>
#include "Entity.h"



//Default constructor
Entity::Entity()
    : m_position(0.0f), m_movement(0.0f), m_model_matrix(1.0f), m_scale(glm::vec3(1.0f)),
      m_speed(0.0f), m_animation_columns(0), m_animation_frames(0), m_animation_index(0),
      m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
      m_texture_id(0), m_velocity(0.0f), m_acceleration(0.0f)
{
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j)
            m_walking[i][j] = 0;
}

// Parameterized constructor
Entity::Entity(GLuint texture_id, float speed, int m_walking[4][2], float animation_time,
               int animation_frames, int animation_index, int animation_cols, int animation_rows)
    : m_position(0.0f), m_movement(0.0f), m_model_matrix(1.0f), m_scale(glm::vec3(1.0f)),
      m_speed(speed), m_animation_columns(animation_cols), m_animation_frames(animation_frames),
      m_animation_index(animation_index), m_animation_rows(animation_rows),
      m_animation_indices(nullptr), m_animation_time(animation_time), m_texture_id(texture_id),
      m_velocity(0.0f), m_acceleration(0.0f)
{
    set_walking(m_walking);
}

Entity::Entity(GLuint texture_id, float speed)
    : m_position(0.0f), m_movement(0.0f), m_model_matrix(1.0f), m_scale(glm::vec3(1.0f)),
      m_speed(speed), m_animation_columns(0), m_animation_frames(0), m_animation_index(0),
      m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
      m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f)
{
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j)
            m_walking[i][j] = 0;
}

Entity::~Entity() {}

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    float u = 0.0f;
    float v = 0.0f;
    float width = 1.0f;
    float height = 1.0f;

    float tex_coords[] = {
        u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v
    };

    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

bool Entity::check_collision(Entity* other) const {
    float x_distance = fabs(m_position.x - other->m_position.x);
    float y_distance = fabs(m_position.y - other->m_position.y);
    float x_overlap = (m_width + other->m_width) / 2.0f;
    float y_overlap = (m_height + other->m_height) / 2.0f;

    return x_distance < x_overlap && y_distance < y_overlap;
}

void Entity::update(float delta_time, Entity* collidable_entities, int collidable_entity_count, bool is_moving_h, bool is_moving_v) {
    const float TERMINAL_VELOCITY = -5.0f;
    const float DRAG = 0.8f;

    m_velocity += m_acceleration * delta_time;

    if (!is_moving_h) m_velocity.x *= DRAG;
    if (m_velocity.y < TERMINAL_VELOCITY) m_velocity.y = TERMINAL_VELOCITY;

    m_position += m_velocity * delta_time;

    const float LEFT_BOUND = -4.9f;
    const float RIGHT_BOUND = 4.9f;
    if (m_position.x < LEFT_BOUND) m_position.x = LEFT_BOUND;
    if (m_position.x > RIGHT_BOUND) m_position.x = RIGHT_BOUND;

    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}

void Entity::update(float delta_time) {
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}

void Entity::render(ShaderProgram* program) {
    program->set_model_matrix(m_model_matrix);

    if (m_animation_indices != nullptr) {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        return;
    }

    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };

    float tex_coords[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f
    };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

float Entity::get_fuel() const {
    return m_fuel;
}

void Entity::reduce_fuel(float amount) {
    m_fuel = std::max(0.0f, m_fuel - amount);
}

// Real vertical movement logic
void Entity::move_up() {
    m_acceleration.y += 30.0f; // add upward thrust to gravity
    face_up();
}

void Entity::move_down() {
    m_velocity.y = -2.5f;  // Optional downward thrust
    face_down();
}

void Entity::set_walking(int walking[4][2]) {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 2; ++j)
            m_walking[i][j] = walking[i][j];
}



