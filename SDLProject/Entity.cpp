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
    : m_position(0.0f), m_movement(0.0f), m_model_matrix(1.0f), m_scale(glm::vec3(1.0f, 1.0f, 1.0f)),
    m_speed(0.0f), m_animation_columns(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(0), m_velocity(0.0f), m_acceleration(0.0f)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0;
}
// Parameterized constructor
Entity::Entity(GLuint texture_id, float speed, int m_walking[4][2], float animation_time,
    int animation_frames, int animation_index, int animation_cols,
    int animation_rows) :

    //m_texture_id(texture_id), m_speed(speed), m_animation_indices(nullptr), m_animation_time(animation_time),
    //m_animation_frames(animation_frames), m_animation_index(animation_index), m_animation_columns(animation_cols),
    //m_animation_rows(animation_rows)


    m_position(0.0f), m_movement(0.0f), m_model_matrix(1.0f), m_scale(glm::vec3(1.0f, 1.0f, 1.0f)),
    m_speed(speed), m_animation_columns(animation_cols),
    m_animation_frames(animation_frames), m_animation_index(animation_index),
    m_animation_rows(animation_rows), m_animation_indices(nullptr),
    m_animation_time(animation_time), m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f)
{
    set_walking(m_walking);
}

// Simpler constructor for partial initialization
Entity::Entity(GLuint texture_id, float speed)
    : m_position(0.0f), m_movement(0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_animation_columns(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_scale(glm::vec3(1.0f, 1.0f, 1.0f))
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0;
}


//destructorqq
Entity::~Entity() { }

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    // **If using a single texture (not sprite sheet), use full UV range**
    float u_coord = 0.0f;
    float v_coord = 0.0f;
    float width = 1.0f;
    float height = 1.0f;

    float tex_coords[] =
    {
        u_coord, v_coord + height,   // Bottom left
        u_coord + width, v_coord + height, // Bottom right
        u_coord + width, v_coord,  // Top right

        u_coord, v_coord + height,   // Bottom left
        u_coord + width, v_coord,  // Top right
        u_coord, v_coord   // Top left
    };

    float vertices[] =
    {
        -0.5, -0.5,  0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5,  0.5,  0.5, -0.5, 0.5
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


bool const Entity::check_collision(Entity* other) const
{
    float collision_factor = 0.28f;
    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width * collision_factor + other->m_width * collision_factor) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height * collision_factor + other->m_height * collision_factor) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

void Entity::update(float delta_time, Entity* collidable_entities, int collidable_entity_count, bool is_moving_h, bool is_moving_v)
{
    const float GRAVITY = -0.5f;  // Normal gravity
    const float TERMINAL_VELOCITY = -1.0f;  // Cap fall speed
    const float DRAG = 0.85f;  // Smooth horizontal slowdown
    const float GROUND_LEVEL = -2.8f;  // Adjust based on your game world
    
    // Always apply gravity
    m_acceleration.y = GRAVITY;
    
    // If pressing down, increase downward acceleration
    if (is_moving_v) {
        m_acceleration.y -= 0.5;
    }
    
    // Apply acceleration to velocity
    m_velocity.y += m_acceleration.y * delta_time;
    m_velocity.x += m_acceleration.x * delta_time;
    
    // Cap downward velocity (prevents infinite acceleration)
    if (m_velocity.y < TERMINAL_VELOCITY) {
        m_velocity.y = TERMINAL_VELOCITY;
    }
    
    // Apply drag to horizontal movement
    if (!is_moving_h) {
        m_velocity.x *= DRAG;
    }
    
    // Apply velocity to position
    m_position.y += m_velocity.y * delta_time;
    m_position.x += m_velocity.x * delta_time;
    
    // **Check if spaceship has hit the ground**
    if (m_position.y <= GROUND_LEVEL) {
        m_position.y = GROUND_LEVEL;  // Stop falling
        m_velocity.y = 0.0f;  // Stop movement;
    }
    
    // **Fix: Stop extra acceleration when down key is released**
    if (!is_moving_v && m_velocity.y < 0) {
        m_velocity.y *= 0.95f;  // Slow down the downward drift when key is released
    }
    
    // Update model matrix
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
    const float LEFT_BOUND = -4.9f;   // Left screen limit
    const float RIGHT_BOUND = 4.9f;   // Right screen limit
    const float TOP_BOUND = 3.5f;     // Top screen limit
    const float BOTTOM_BOUND = GROUND_LEVEL; // Bottom (already defined)
    
    // **Clamp the spaceship inside the screen boundaries**
    if (m_position.x < LEFT_BOUND) m_position.x = LEFT_BOUND;
    if (m_position.x > RIGHT_BOUND) m_position.x = RIGHT_BOUND;
    if (m_position.y > TOP_BOUND) m_position.y = TOP_BOUND;
    if (m_position.y < BOTTOM_BOUND) {
        m_position.y = BOTTOM_BOUND;
        m_velocity.y = 0.0f;  // Stop falling
    }
}


void Entity::update(float delta_time)
{
    if (m_animation_indices != NULL)
    {
        if (glm::length(m_movement) != 0)
        {
            m_animation_time += delta_time;
            float frames_per_second = (float)1 / SECONDS_PER_FRAME;

            if (m_animation_time >= frames_per_second)
            {
                m_animation_time = 0.0f;
                m_animation_index++;

                if (m_animation_index >= m_animation_frames)
                {
                    m_animation_index = 0;
                }
            }
        }
    }

    m_velocity.x += m_acceleration.x * delta_time * 0.01f;
    m_position.x += m_velocity.x * delta_time;
    //m_position += m_movement * m_speed * delta_time;
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}

void Entity::render(ShaderProgram* program)
{
    program->set_model_matrix(m_model_matrix);

    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id,
            m_animation_indices[m_animation_index]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
    
}

// Getter for fuel
float Entity::get_fuel() const {
    return m_fuel;
}

// Reduce fuel safely (ensures it never goes below 0)
void Entity::reduce_fuel(float amount) {
    m_fuel = std::max(0.0f, m_fuel - amount);
}

