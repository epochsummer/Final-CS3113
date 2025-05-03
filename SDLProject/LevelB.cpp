#include "LevelB.h"
#include "Utility.h"

#define LEVEL_WIDTH 40
#define LEVEL_HEIGHT 20

constexpr char SPRITESHEET_FILEPATH[] = "/Users/epochsum/Desktop/Gaming-Project/assets/craft_racer.png",
PLATFORM_FILEPATH[] = "/Users/epochsum/Desktop/Gaming-Project/assets/spaceMeteors_002.png",
ENEMY_FILEPATH[] = "/Users/epochsum/Desktop/Gaming-Project/assets/alien.png",
BULLET_FILEPATH[] = "/Users/epochsum/Desktop/Gaming-Project/assets/bullet.png",
FONT_SPRITE_FILEPATH[] = "/Users/epochsum/Desktop/Gaming-Project/assets/font1.png",
BACKGROUND_FILEPATH[] = "/Users/epochsum/Desktop/Gaming-Project/assets/uncolored_talltrees.png";

unsigned int LEVEL_DATA0[] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,1,0,1,0,1,0,0,0,1,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,
    1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
    1,1,1,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,1,1,1,0,0,1,1,1,1,1,
    1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,
    1,0,1,0,1,0,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
    1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,
    1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,
    1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,
    1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

LevelB::~LevelB()
{
    delete[] m_game_state.enemies;
    delete m_game_state.player;
    delete m_game_state.map;
    delete m_game_state.bullet;
    Mix_FreeChunk(m_game_state.shoot_sfx);
    Mix_FreeChunk(m_game_state.walking_sfx);
    Mix_FreeChunk(m_game_state.death_sfx);
}

void LevelB::initialise()
{
    GLuint map_texture_id = Utility::load_texture("/Users/epochsum/Desktop/Gaming-Project/assets/spaceMeteors_002.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA0, map_texture_id, 1.0f, 1, 1);

    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);

    int player_walking_animation[4][4] =
    {
        { 1, 5, 9, 13 },
        { 3, 7, 11, 15 },
        { 2, 6, 10, 14 },
        { 0, 4, 8, 12 }
    };

    m_game_state.player = new Entity(
        player_texture_id,
        3.5f,
        0.3f,
        0.3f,
        PLAYER
    );
    m_game_state.player->set_position(glm::vec3(2.0f, -3.0f, 0.0f));
    m_game_state.player->set_scale(glm::vec3(0.7f, 0.7f, 0.0f));

    GLuint bullet_texture_id = Utility::load_texture(BULLET_FILEPATH);
    m_game_state.bullet = new Entity(bullet_texture_id, 10.0f, 0.4f, 0.125f, BULLET);
    m_game_state.bullet->set_position(m_game_state.player->get_position());
    m_game_state.bullet->set_scale(glm::vec3(0.4f, 0.125f, 0.0f));

    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i] = Entity(enemy_texture_id, 1.5f, 0.7f, 0.7f, ENEMY, GUARD, IDLE);
    }

    m_game_state.enemies[0].set_position(glm::vec3(5.0f, -2.0f, 0.0f));
    m_game_state.enemies[1].set_position(glm::vec3(20.0f, -6.0f, 0.0f));
    m_game_state.enemies[2].set_position(glm::vec3(34.0f, -10.0f, 0.0f));

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].set_scale(glm::vec3(0.8f, 0.8f, 0.0f));
    }

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_AllocateChannels(16);

    m_game_state.shoot_sfx = Mix_LoadWAV("/Users/epochsum/Desktop/Gaming-Project/assets/317480__djfroyd__laser-sound.wav");
    m_game_state.walking_sfx = Mix_LoadWAV("/Users/epochsum/Desktop/Gaming-Project/assets/268868__cydon__future-car_001-or-spaceship.wav");
    m_game_state.death_sfx = Mix_LoadWAV("/Users/epochsum/Desktop/Gaming-Project/assets/703542__yoshicakes77__dead.wav");
}

void LevelB::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    m_game_state.bullet->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, m_game_state.player, 1, m_game_state.map);
    }

    if (!m_game_state.enemies[0].get_is_active() &&
        !m_game_state.enemies[1].get_is_active() &&
        !m_game_state.enemies[2].get_is_active())
    {
        level_win = true;
    }
}

void LevelB::render(ShaderProgram* g_shader_program)
{
    if (m_game_state.player->get_position().x != m_game_state.bullet->get_position().x)
    {
        m_game_state.bullet->render(g_shader_program);
    }

    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].render(g_shader_program);
    }
}
