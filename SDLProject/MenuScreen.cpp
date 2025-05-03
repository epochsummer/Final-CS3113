#include "MenuScreen.h"
#include "Utility.h"

#define LEVEL_WIDTH 2
#define LEVEL_HEIGHT 2

constexpr char SPRITESHEET_FILEPATH[] = "/Users/epochsum/Desktop/Gaming-Project/assets/craft_racer.png",
PLATFORM_FILEPATH[] = "/Users/epochsum/Desktop/Gaming-Project/assets/spaceMeteors_002.png",
ENEMY_FILEPATH[] = "/Users/epochsum/Desktop/Gaming-Project/assets/alien.png",
BULLET_FILEPATH[] = "/Users/epochsum/Desktop/Gaming-Project/assets/bullet.png",
FONT_SPRITE_FILEPATH[] = "/Users/epochsum/Desktop/Gaming-Project/assets/font1.png",
BACKGROUND_FILEPATH[] = "/Users/epochsum/Desktop/Gaming-Project/assets/uncolored_talltrees.png";

GLuint g_font_texture_id;

unsigned int LEVEL_DATA1[] =
{
    0, 0,
    0, 0
};

MenuScreen::~MenuScreen()
{
    delete m_game_state.player;
    delete m_game_state.map;
    Mix_FreeChunk(m_game_state.shoot_sfx);
}

void MenuScreen::initialise()
{
    GLuint map_texture_id = Utility::load_texture("/Users/epochsum/Desktop/Gaming-Project/assets/spaceMeteors_002.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA1, map_texture_id, 1.0f, 20, 10);

    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    g_font_texture_id = Utility::load_texture(FONT_SPRITE_FILEPATH);

    int player_walking_animation[4][4] =
    {
        { 1, 5, 9, 13 },
        { 3, 7, 11, 15 },
        { 2, 6, 10, 14 },
        { 0, 4, 8, 12 }
    };

    glm::vec3 acceleration = glm::vec3(0.0f, -4.905f, 0.0f);

    m_game_state.player = new Entity(
        player_texture_id,
        5.0f,
        0.9f,
        0.9f,
        PLAYER
    );
    m_game_state.player->set_position(glm::vec3(5.0f, -3.0f, 0.0f));
}

void MenuScreen::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
}

void MenuScreen::render(ShaderProgram* g_shader_program)
{
    Utility::draw_text(g_shader_program, g_font_texture_id, "Man in Black", 0.32, 0.00001f, glm::vec3(0.5f, -2.0f, 0.0f));
    Utility::draw_text(g_shader_program, g_font_texture_id, "Kill all the Aliens", 0.32, 0.03f, glm::vec3(2.0f, -3.0f, 0.0f));
    Utility::draw_text(g_shader_program, g_font_texture_id, "Press Enter to Start", 0.3, 0.03f, glm::vec3(2.0f, -5.0f, 0.0f));
}
